#include <ch/HistoryMessage.hpp>
#include <ch/Room.hpp>
#include <ch/parse_name_color.hpp>

#include <ctre.hpp>
#include <spdlog/spdlog.h>
#include <tcx/no_utf8.hpp>
#include <tcx/parse_int.hpp>
#include <tcx/split.hpp>

#include <chrono>
#include <cstdlib>

using namespace std::literals;

static constexpr ctll::fixed_string anon_number_pattern = "^<n([0-9]{4})/>.*";

static auto parse_n(std::string_view s)
{
    auto match = ctre::match<anon_number_pattern>(s);
    //if (!match)
    //    return UINT_MAX;
    std::string_view sn = match.get<1>().to_view();
    return sn;
    // std::uint32_t n = (sn[0] - '0') * 1000 + (sn[1] - '0') * 100 + (sn[2] - '0') * 10 + (sn[3] - '0');
    // return n;
}

static std::string get_anon_id(std::string_view n, std::string_view ssid)
{
    std::string result;
    if (n.empty())
        n = "5504"sv;

    ssid.remove_prefix(std::min<std::size_t>(4, ssid.size()));
    for (std::size_t i = 0; i < std::min(n.size(), ssid.size()); ++i) {
        int sum = (n[i] - '0') + (ssid[i] - '0');
        int last_digit = sum % 10;
        result += last_digit + '0';
    }
    if (result.empty())
        result += "NNNN"sv;
    return result;
}

void ch::Room::rcmd_i(std::u8string cmd_args)
{
    ch::HistoryMessage ilog;
    auto argv = tcx::split(cmd_args, u8':', 10);

    {
        char buf[std::numeric_limits<double>::digits10 + 1] {};
        if (std::size(argv[0]) > std::size(buf))
            throw std::length_error("too big to fit in buffer");
        std::memcpy(+buf, argv[0].data(), argv[0].size());

        char* str_end;
        double time = std::strtod(+buf, &str_end);
        if (!time && !str_end)
            throw std::runtime_error("can't convert to double");

#if defined(__cpp_lib_chrono) && __cpp_lib_chrono > 201907L
        ilog.time = MessageLog::time_stamp { MessageLog::time_stamp::typename duration { time } };
#else
        ilog.time = time;
#endif
    }

    if (!argv[1].empty()) {
        // log in name
        auto name_color = parse_name_color(tcx::no_utf8(argv[9]));
        if (name_color)
            ilog.name_color = *name_color;
        ilog.user_name += tcx::no_utf8(argv[1]);
    } else if (!argv[2].empty()) {
        // temporary name
        ilog.user_name += '#';
        ilog.user_name += tcx::no_utf8(argv[2]);
    } else {
        ilog.user_name += "!anon";
        ilog.user_name += get_anon_id(parse_n(tcx::no_utf8(argv[9])), tcx::no_utf8(argv[3]));
    }
    ilog.puid = *tcx::parse_int<std::uint64_t>(tcx::no_utf8(argv[3]));
    ilog.ip = argv[6].empty() ? boost::asio::ip::address {} : boost::asio::ip::make_address(tcx::no_utf8(argv[6]));
    ilog.raw_body = std::u8string { argv[9] };

    m_ilogs.emplace_back(std::move(ilog));
}