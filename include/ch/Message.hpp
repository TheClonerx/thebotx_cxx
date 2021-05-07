#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include <boost/asio/ip/address.hpp>

namespace ch {
class Room;
} // namespace ch

namespace ch {
class Message {
public:
    Message(Room& room);

    Room& room() noexcept { return *m_room; }

    std::uint64_t time() const noexcept { return m_time; }
    std::string_view user() const noexcept { return m_user; }
    std::u8string_view body() const noexcept { return m_body; }
    std::u8string_view rawmsg() const noexcept { return m_rawmsg; }
    boost::asio::ip::address const& ip() const noexcept { return m_ip; }

private:
    Room* m_room;
    std::uint64_t m_time;
    std::string m_user;
    std::u8string m_body;
    std::u8string m_rawmsg;
    boost::asio::ip::address m_ip;
};
} // namespace ch
