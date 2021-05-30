#include <ch/Room.hpp>
#include <ch/RoomManager.hpp>

#include <string>
#include <string_view>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>

void ch::RoomManager::exec()
{
    for (std::string_view name : m_rooms_to_connect) {
        auto room = std::make_unique<ch::Room>(this, name);
        boost::asio::co_spawn(*m_io_context, room->connect(), boost::asio::detached);
        m_rooms.emplace_back(std::move(room));
    }
    // destroy everything
    m_rooms_to_connect = decltype(m_rooms_to_connect) {};
    m_running = true;
    boost::asio::co_spawn(*m_io_context, this->onInit(), boost::asio::detached);
}

void ch::RoomManager::joinRoom(std::string_view name)
{
    if (!m_running) {
        m_rooms_to_connect.emplace_back(name);
    } else {
        auto room = std::make_unique<ch::Room>(this, name);
        boost::asio::co_spawn(*m_io_context, room->connect(), boost::asio::detached);
        m_rooms.emplace_back(std::move(room));
    }
}

boost::asio::awaitable<void> ch::RoomManager::onInit() { co_return; }
boost::asio::awaitable<void> ch::RoomManager::onConnectFail(ch::Room&, std::error_code) { co_return; }
boost::asio::awaitable<void> ch::RoomManager::onConnect(ch::Room&) { co_return; }
