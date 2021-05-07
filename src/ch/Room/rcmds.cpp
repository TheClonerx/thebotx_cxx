#include <ch/Room.hpp>

using namespace std::literals;

boost::container::flat_map<std::u8string_view, decltype(&ch::Room::rcmd_pong)> const ch::Room::cmd_to_func {
#define RCMD(name) std::make_pair(u8## #name##sv, &ch::Room::rcmd_##name)
    RCMD(pong),
    RCMD(v),
    RCMD(i),
    RCMD(ok)
#undef RCMD
};
