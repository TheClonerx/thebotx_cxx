#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>

#include <memory_resource>
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

    auto& resolver() noexcept
    {
        return m_resolver;
    }

private:
    void setup_allocator();

public:
    boost::asio::io_context& io_context() noexcept
    {
        return *m_io_context;
    }

    boost::asio::ssl::context& ssl_context() noexcept
    {
        return *m_ssl_context;
    }

    void exec();
    void joinRoom(std::string_view name);

    /*
    *   CALLBACKS
    */

    virtual void onInit();
    virtual void onConnectFail(ch::Room&, std::error_code);
    virtual void onConnect(ch::Room&);

private:
    boost::asio::io_context* m_io_context;
    boost::asio::ssl::context* m_ssl_context;

    boost::asio::ip::tcp::resolver m_resolver;
    std::vector<std::string> m_rooms_to_connect;
    std::vector<std::unique_ptr<ch::Room>> m_rooms;
};
} // namespace ch
