#include <ch/Room.hpp>
#include <ch/RoomManager.hpp>

#include <string>
#include <string_view>

void ch::RoomManager::exec()
{
    for (std::string_view name : m_rooms_to_connect) {
        auto room = make_unique<ch::Room>(this, name);
        room->connect();
        m_rooms.emplace_back(std::move(room));
    }
    // destroy everything
    m_rooms_to_connect = decltype(m_rooms_to_connect) {};
}

void ch::RoomManager::joinRoom(std::string_view name)
{
    m_rooms_to_connect.emplace_back(name);
}

void ch::RoomManager::onInit() { }
void ch::RoomManager::onConnectFail(ch::Room&,std::error_code) { }
void ch::RoomManager::onConnect(ch::Room&) { }