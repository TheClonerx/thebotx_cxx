#include <ch/Room.hpp>

#include <boost/asio/use_awaitable.hpp>

boost::asio::awaitable<void> ch::Room::disconnect()
{
    if (m_status != Status::CONNECTED)
        co_return;

    try {
        co_await m_socket.async_close(boost::beast::websocket::close_code::normal, boost::asio::use_awaitable);
    } catch (boost::system::system_error const&) {
        // ignore
    }

    co_return;
}
