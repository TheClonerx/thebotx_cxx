#include <ch/get_server.hpp>

#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstdio>
#include <numeric>
#include <version>

#if defined(__cpp_lib_format)
#include <format>
#endif

using namespace std::string_view_literals;
std::unordered_map<std::string_view, int> const ch::specials = {
    { "mitvcanal"sv, 56 },
    { "animeultimacom"sv, 34 },
    { "cricket365live"sv, 21 },
    { "pokemonepisodeorg"sv, 22 },
    { "animelinkz"sv, 20 },
    { "sport24lt"sv, 56 },
    { "narutowire"sv, 10 },
    { "watchanimeonn"sv, 22 },
    { "cricvid-hitcric-"sv, 51 },
    { "narutochatt"sv, 70 },
    { "leeplarp"sv, 27 },
    { "stream2watch3"sv, 56 },
    { "ttvsports"sv, 56 },
    { "ver-anime"sv, 8 },
    { "vipstand"sv, 21 },
    { "eafangames"sv, 56 },
    { "soccerjumbo"sv, 21 },
    { "myfoxdfw"sv, 67 },
    { "kiiiikiii"sv, 21 },
    { "de-livechat"sv, 5 },
    { "rgsmotrisport"sv, 51 },
    { "dbzepisodeorg"sv, 10 },
    { "watch-dragonball"sv, 8 },
    { "peliculas-flv"sv, 69 },
    { "tvanimefreak"sv, 54 },
    { "tvtvanimefreak"sv, 54 }
};

std::vector<std::pair<int, int>> const ch::tsweights = { { 5, 75 },
    { 6, 75 },
    { 7, 75 },
    { 8, 75 },
    { 16, 75 },
    { 17, 75 },
    { 18, 75 },
    { 9, 95 },
    { 11, 95 },
    { 12, 95 },
    { 13, 95 },
    { 14, 95 },
    { 15, 95 },
    { 19, 110 },
    { 23, 110 },
    { 24, 110 },
    { 25, 110 },
    { 26, 110 },
    { 28, 104 },
    { 29, 104 },
    { 30, 104 },
    { 31, 104 },
    { 32, 104 },
    { 33, 104 },
    { 35, 101 },
    { 36, 101 },
    { 37, 101 },
    { 38, 101 },
    { 39, 101 },
    { 40, 101 },
    { 41, 101 },
    { 42, 101 },
    { 43, 101 },
    { 44, 101 },
    { 45, 101 },
    { 46, 101 },
    { 47, 101 },
    { 48, 101 },
    { 49, 101 },
    { 50, 101 },
    { 52, 110 },
    { 53, 110 },
    { 55, 110 },
    { 57, 110 },
    { 58, 110 },
    { 59, 110 },
    { 60, 110 },
    { 61, 110 },
    { 62, 110 },
    { 63, 110 },
    { 64, 110 },
    { 65, 110 },
    { 66, 110 },
    { 68, 95 },
    { 71, 116 },
    { 72, 116 },
    { 73, 116 },
    { 74, 116 },
    { 75, 116 },
    { 76, 116 },
    { 77, 116 },
    { 78, 116 },
    { 79, 116 },
    { 80, 116 },
    { 81, 116 },
    { 82, 116 },
    { 83, 116 },
    { 84, 116 } };

std::string ch::get_server(std::string group)
{
    int sn = 0;
    if (auto it = ch::specials.find(group); it != ch::specials.end()) {
        sn = it->second;
    } else {
        std::replace(group.begin(), group.end(), '_', 'q');
        std::replace(group.begin(), group.end(), '-', 'q');
        int fnv {};
        std::from_chars(group.data(), group.data() + std::min(static_cast<std::string::size_type>(5), group.size()), fnv, 36);
        int lnv {};
        std::from_chars(group.data() + std::min(static_cast<std::string::size_type>(6), group.size()), group.data() + std::min(static_cast<std::string::size_type>(9), group.size()), lnv, 36);
        lnv = lnv ? std::max(lnv, 1000) : 1000;
        double const num = std::fmod(fnv, lnv) / lnv;
        static double const max_num = std::accumulate(ch::tsweights.begin(), ch::tsweights.end(), 0, [](int sum, std::pair<int, int> const& p) { return sum + p.second; });
        double cumfreq = 0;
        for (auto const& [x, y] : ch::tsweights) {
            cumfreq += y / max_num;
            if (num <= cumfreq) {
                sn = x;
                break;
            }
        }
    }

#if defined(__cpp_lib_format)
    return std::format("s{}.chatango.com", sn);
#else
    int len = std::snprintf(nullptr, 0, "s%d.chatango.com", sn)+1;
    group.resize(len);
    std::snprintf(group.data(), group.size(), "s%d.chatango.com", sn);
    return group;
#endif
}