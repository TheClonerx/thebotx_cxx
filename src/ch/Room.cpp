#include <ch/Room.hpp>
#include <ch/RoomManager.hpp>
#include <ch/gen_uid.hpp>
#include <ch/get_server.hpp>

#include <boost/asio/ssl/rfc2818_verification.hpp>

using namespace std::literals;

static constexpr auto WSS_PORT = "8081"sv;

#define CALL_CALLBACK(name, ...)                \
    do {                                        \
        try {                                   \
            this->m_mgr->on##name(__VA_ARGS__); \
        } catch (std::exception const& e) {     \
        } catch (...) {                         \
        }                                       \
    } while (0)

ch::Room::Room(ch::RoomManager* mgr, std::string_view name)
    : m_mgr { mgr }
    , m_socket { m_mgr->io_context(), m_mgr->ssl_context() }
    , m_name { name }
    , m_host { ch::get_server(m_name) }
    , m_uid { ch::gen_uid() }
    , m_receiving_buf { m_receiving }
{
    m_socket.text(true); // chatango only uses text messages
}

#include <spdlog/spdlog.h>

void ch::Room::connect()
{
    if (m_connected || m_connecting)
        return;

    m_connecting = true;
    spdlog::info("Resolving"sv);
    m_mgr->resolver().async_resolve(m_host, WSS_PORT, std::bind_front(&Room::cb_on_resolve, this));
}

void ch::Room::cb_on_resolve(std::error_code ec, boost::asio::ip::tcp::resolver::results_type results)
{
    if (ec) {
        m_connecting = false;
        CALL_CALLBACK(ConnectFail, *this, ec);
        return;
    }

    spdlog::info("Connecting"sv);
    boost::beast::get_lowest_layer(m_socket).async_connect(results, boost::beast::bind_front_handler(&Room::cb_on_connect, this));
}

void ch::Room::cb_on_connect(std::error_code ec, boost::asio::ip::tcp::endpoint)
{
    if (ec) {
        m_connecting = false;
        CALL_CALLBACK(ConnectFail, *this, ec);
        return;
    }
    boost::beast::get_lowest_layer(m_socket).socket().set_option(boost::asio::ip::tcp::no_delay { false });
    m_socket.next_layer().set_verify_mode(boost::asio::ssl::verify_peer);
    m_socket.next_layer().set_verify_callback(boost::asio::ssl::rfc2818_verification { m_host });

    spdlog::info("Perfoming SSL/TLS handshake"sv);
    m_socket.next_layer().async_handshake(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>::client, boost::beast::bind_front_handler(&Room::cb_on_tls_handshake, this));
}

void ch::Room::cb_on_tls_handshake(std::error_code ec)
{
    if (ec) {
        m_connecting = false;
        CALL_CALLBACK(ConnectFail, *this, ec);
        return;
    }
    m_socket.set_option(boost::beast::websocket::stream_base::decorator([](boost::beast::websocket::request_type& request) {
        request.set(boost::beast::http::field::user_agent, "thebotx_cxx/1.0");
        request.set(boost::beast::http::field::origin, "https://st.chatango.com");
    }));

    spdlog::info("Performing HTTP handshake"sv);
    m_socket.async_handshake(m_host, "/", boost::beast::bind_front_handler(&Room::cb_on_ws_handshake, this));
}

void ch::Room::cb_on_ws_handshake(std::error_code ec)
{
    if (ec) {
        m_connecting = false;
        CALL_CALLBACK(ConnectFail, *this, ec);
    } else {
        m_connected = std::exchange(m_connecting, false);
        CALL_CALLBACK(Connect, *this);
        m_socket.async_read(m_receiving_buf, boost::beast::bind_front_handler(&Room::cb_on_read, this));
    }
}

void ch::Room::cb_on_read(std::error_code ec, std::size_t bytes_read)
{
    if (ec)
        spdlog::error("read error: {} {}"sv, ec.category().name(), ec.message());
    spdlog::info("received: \"{}\"", std::string_view { m_receiving.data(), bytes_read });
    m_socket.async_read(m_receiving_buf, boost::beast::bind_front_handler(&Room::cb_on_read, this));
}

void ch::Room::ensure_writing()
{
    if (!m_writing) {
        auto const& buf = m_sending.front();
        m_socket.async_write(boost::asio::const_buffer(buf.data(), buf.size()), boost::beast::bind_front_handler(&Room::cb_on_write, this));
        m_writing = true;
    }
}

void ch::Room::cb_on_write(std::error_code ec, std::size_t bytes_transferred)
{
    if (ec)
        spdlog::error("write error: {} {}", ec.category().name(), ec.message());

    m_sending.pop();
    if (m_sending.empty()) {
        m_writing = false;
        return;
    } else {
        auto const& buf = m_sending.front();
        m_socket.async_write(boost::asio::const_buffer(buf.data(), buf.size()), boost::beast::bind_front_handler(&Room::cb_on_write, this));
    }
}