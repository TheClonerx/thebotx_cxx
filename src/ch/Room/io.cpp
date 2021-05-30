#include <ch/Room.hpp>
#include <ch/RoomManager.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <spdlog/spdlog.h>

#include <tcx/no_utf8.hpp>
#include <tcx/repr.hpp>
#include <tcx/unmangled_name.hpp>

using namespace std::literals;
using boost::asio::use_awaitable;

boost::asio::awaitable<void> ch::Room::do_read()
{
    std::vector<std::byte> read_vec;
    while (m_status == Status::CONNECTED) {
        std::error_code ec;
        read_vec.clear();
        try {
            auto buf = boost::asio::dynamic_buffer(read_vec);
            co_await m_socket.async_read(buf, use_awaitable);
        } catch (boost::system::system_error const& e) {
            ec = e.code();
        }
        if (ec) {
            spdlog::error("Error reading in room {}: [{}] {}", m_name, ec.category().name(), ec.message());
            m_status = Status::DISCONNECTING;
            try {
                co_await m_socket.async_close(boost::beast::websocket::close_code::internal_error, use_awaitable);
            } catch (boost::system::system_error const&) {
                // ignore
            }
            m_status = Status::DISCONNECTED;
            co_return;
        }
        // sanity check, chatango only uses text messages
        if (!m_socket.got_text()) {
            spdlog::error("Received data isn't text!!");
            co_await m_socket.async_close(boost::beast::websocket::close_code::unknown_data, use_awaitable);
            co_return;
        }
        // websocket standar ensures that this is utf8
        std::u8string_view command { reinterpret_cast<char8_t const*>(read_vec.data()), read_vec.size() };

        spdlog::info("recv'd from {}: {}", m_name, tcx::repr({ reinterpret_cast<char const*>(read_vec.data()), read_vec.size() }));

        std::u8string_view rcmd;
        std::u8string args;
        if (auto pos = command.find(u8':'); pos == std::u8string_view::npos) {
            rcmd = u8"pong"sv;
        } else {
            rcmd = command.substr(0, pos);
            args = std::u8string(command.cbegin() + pos + 1, command.end());
        }
        if (auto it = cmd_to_func.find(rcmd); it == cmd_to_func.end()) {
            spdlog::warn("Unhandled rcmd {}\t{}"sv, tcx::no_utf8(rcmd), tcx::no_utf8(args));
        } else {
            try {
                std::invoke(it->second, this, std::move(args));
            } catch (boost::system::system_error& e) {
                spdlog::error("Error handling rcmd {} in room {}: [{}] {}", tcx::no_utf8(rcmd), m_name, e.code().category().name(), e.code().message());
            } catch (std::system_error& e) {
                spdlog::error("Error handling rcmd {} in room {}: [{}] {}", tcx::no_utf8(rcmd), m_name, e.code().category().name(), e.code().message());
            } catch (std::exception& e) {
                spdlog::error("Error handling rcmd {} in room {}: [{}] {}", tcx::no_utf8(rcmd), m_name, tcx::unmangled_name(typeid(e)), e.what());
            }
        }
    }
    co_await m_socket.async_close(boost::beast::websocket::close_code::normal, use_awaitable);
    co_return;
}

boost::asio::awaitable<void> ch::Room::do_ping()
{
    boost::asio::steady_timer ping_timer { co_await boost::asio::this_coro::executor };
    while (m_status == Status::CONNECTED) {
        ping_timer.expires_after(std::chrono::seconds(20));
        try {
            co_await ping_timer.async_wait(use_awaitable);
            // sending an empty command is equivalent to ping
            // receiving an emtpy command is equivalent to pong
            co_await send_command(""sv);
        } catch (boost::system::system_error& e) {
            if (e.code() != boost::asio::error::make_error_code(boost::asio::error::operation_aborted))
                throw;
            break;
        }
    }
    co_return;
}
