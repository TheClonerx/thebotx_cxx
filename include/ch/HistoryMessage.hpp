#pragma once

#include <ch/Color.hpp>

#include <boost/asio/ip/address.hpp>

#include <chrono>
#include <cstdint>
#include <string>

namespace ch {
struct HistoryMessage {
#if defined(__cpp_lib_chrono) && __cpp_lib_chrono > 201907L
    using time_stamp = std::chrono::utc_time<std::duration<double>>;
#else
    // seconds since the unix epoch
    using time_stamp = double;
#endif
    time_stamp time;
    std::string user_name;
    std::uint64_t puid;
    boost::asio::ip::address ip;
    std::u8string raw_body;
    ch::Color name_color;
};
} // namespace ch
