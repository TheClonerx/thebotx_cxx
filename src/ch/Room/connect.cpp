#include <ch/Room.hpp>
#include <ch/RoomManager.hpp>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/ssl/rfc2818_verification.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <spdlog/spdlog.h>
#include <tcx/unmangled_name.hpp>

using namespace std::literals;

static constexpr auto WSS_PORT = "8081"sv;

#define CALL_CALLBACK(name, ...)                                                                                   \
    do {                                                                                                           \
        try {                                                                                                      \
            boost::asio::co_spawn(m_mgr->io_context(), this->m_mgr->on##name(__VA_ARGS__), boost::asio::detached); \
        } catch (std::exception & e) {                                                                             \
            spdlog::error("on" #name " {}: {}"sv, tcx::unmangled_name(typeid(e)), e.what());                       \
        } catch (...) {                                                                                            \
            spdlog::error("on" #name " unkown error"sv);                                                           \
        }                                                                                                          \
    } while (0)

boost::asio::awaitable<void> ch::Room::connect()
{
    if (auto errc = co_await this->do_connect(); errc) {
        CALL_CALLBACK(ConnectFail, *this, errc);
        co_return;
    }

    if (m_status != Status::CONNECTED)
        co_return;
    boost::asio::co_spawn(m_mgr->io_context(), do_read(), boost::asio::detached);
    boost::asio::co_spawn(m_mgr->io_context(), do_ping(), boost::asio::detached);
    enqueue_command("v"sv);
    enqueue_command("bauth"sv, this->m_name, ""sv, ""sv, ""sv);
}

boost::asio::awaitable<void> ch::Room::connect(std::string_view user, std::string_view password)
{
    if (auto errc = co_await this->do_connect(); errc) {
        CALL_CALLBACK(ConnectFail, *this, errc);
    }
    if (m_status != Status::CONNECTED)
        co_return;
    boost::asio::co_spawn(m_mgr->io_context(), do_read(), boost::asio::detached);
    boost::asio::co_spawn(m_mgr->io_context(), do_ping(), boost::asio::detached);
    enqueue_command("v"sv);
    enqueue_command("bauth"sv, this->m_name, this->m_uid, user, password);
}

boost::asio::awaitable<std::error_code> ch::Room::do_connect()
{
    if (m_status != Status::DISCONNECTED)
        co_return std::error_code {};

    m_status = Status::CONNECTING;
    try {
        {
            auto results = co_await m_mgr->resolver().async_resolve(m_host, WSS_PORT, boost::asio::use_awaitable);
            auto endpoint = co_await boost::beast::get_lowest_layer(m_socket).async_connect(results, boost::asio::use_awaitable);
        }

        boost::beast::get_lowest_layer(m_socket).socket().set_option(boost::asio::ip::tcp::no_delay { false });
        m_socket.next_layer().set_verify_mode(boost::asio::ssl::verify_peer);
        m_socket.next_layer().set_verify_callback(boost::asio::ssl::rfc2818_verification { m_host });

        co_await m_socket.next_layer().async_handshake(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>::client, boost::asio::use_awaitable);
        m_socket.set_option(boost::beast::websocket::stream_base::decorator([](boost::beast::websocket::request_type& request) {
            request.set(boost::beast::http::field::user_agent, "thebotx_cxx/1.0");
            request.set(boost::beast::http::field::origin, "https://st.chatango.com");
        }));

        co_await m_socket.async_handshake(m_host, "/", boost::asio::use_awaitable);

        m_status = Status::CONNECTED;
    } catch (boost::system::system_error const& e) {
        m_status = Status::DISCONNECTED;
        CALL_CALLBACK(ConnectFail, *this, e.code());
        co_return e.code();
    }

    if (m_status == Status::CONNECTED) {
        CALL_CALLBACK(Connect, *this);
    }

    co_return std::error_code {};
}