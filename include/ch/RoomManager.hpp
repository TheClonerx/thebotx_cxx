#pragma once

#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>

#include <system_error>

namespace ch {
class Room;

} // namespace ch

namespace ch {

class RoomManager {

public:
    RoomManager(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_context)
        : m_io_context { &io_context }
        , m_ssl_context { &ssl_context }
        , m_resolver { *m_io_context }
    {
    }
    
    virtual ~RoomManager() = default;

    auto& resolver() noexcept { return m_resolver; }

    boost::asio::io_context& io_context() noexcept { return *m_io_context; }
    boost::asio::io_context const& io_context() const noexcept { return *m_io_context; }

    boost::asio::ssl::context& ssl_context() noexcept { return *m_ssl_context; }
    boost::asio::ssl::context const& ssl_context() const noexcept { return *m_ssl_context; }

    void exec();
    void joinRoom(std::string_view name);

    /*
    *   CALLBACKS
    */

    virtual boost::asio::awaitable<void> onInit();
    virtual boost::asio::awaitable<void> onConnectFail(ch::Room&, std::error_code);
    virtual boost::asio::awaitable<void> onConnect(ch::Room&);

private:
    boost::asio::io_context* m_io_context;
    boost::asio::ssl::context* m_ssl_context;
    bool m_running = false;

    boost::asio::ip::tcp::resolver m_resolver;
    std::vector<std::string> m_rooms_to_connect;
    std::vector<std::unique_ptr<ch::Room>> m_rooms;
};
} // namespace ch
