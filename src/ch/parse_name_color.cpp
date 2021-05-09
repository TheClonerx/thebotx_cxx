#include <ch/parse_name_color.hpp>

#include <ctre.hpp>

static constexpr ctll::fixed_string name_color_pattern = "^<n([0-9a-fA-F]{3}|[0-9a-fA-F]{6})/>.*";

std::optional<ch::Color> ch::parse_name_color(std::string_view s) noexcept
{
    auto match = ctre::match<name_color_pattern>(s);
    if (!match)
        return std::nullopt;
    std::string_view sc = match.get<1>().to_view();
    ch::Color result {};
    if (sc.size() == 3) {
        if ('0' <= sc[0] && sc[0] <= '9')
            result.r = sc[0] - '0';
        else if ('a' <= sc[0] && sc[0] <= 'f')
            result.r = sc[0] - '0' + 10;
        else if ('A' <= sc[0] && sc[0] <= 'A')
            result.r = sc[0] - '0' + 10;

        if ('0' <= sc[1] && sc[1] <= '9')
            result.g = sc[1] - '0';
        else if ('a' <= sc[1] && sc[1] <= 'f')
            result.g = sc[1] - '0' + 10;
        else if ('A' <= sc[1] && sc[1] <= 'A')
            result.g = sc[1] - '0' + 10;

        if ('0' <= sc[2] && sc[2] <= '9')
            result.b = sc[2] - '0';
        else if ('a' <= sc[2] && sc[2] <= 'f')
            result.b = sc[2] - '0' + 10;
        else if ('A' <= sc[2] && sc[2] <= 'A')
            result.b = sc[2] - '0' + 10;

        result.r = result.r << 4 | result.r;
        result.g = result.g << 4 | result.g;
        result.b = result.b << 4 | result.b;
        return result;
    } else if (sc.size() == 6) {
        if ('0' <= sc[0] && sc[0] <= '9')
            result.r = (sc[0] - '0') << 4;
        else if ('a' <= sc[0] && sc[0] <= 'f')
            result.r = (sc[0] - '0' + 10) << 4;
        else if ('A' <= sc[0] && sc[0] <= 'A')
            result.r = (sc[0] - '0' + 10) << 4;

        if ('0' <= sc[1] && sc[1] <= '9')
            result.r |= sc[1] - '0';
        else if ('a' <= sc[1] && sc[1] <= 'f')
            result.r |= sc[1] - '0' + 10;
        else if ('A' <= sc[1] && sc[1] <= 'A')
            result.r |= sc[1] - '0' + 10;

        if ('0' <= sc[2] && sc[2] <= '9')
            result.g = (sc[2] - '0') << 4;
        else if ('a' <= sc[2] && sc[2] <= 'f')
            result.g = (sc[2] - '0' + 10) << 4;
        else if ('A' <= sc[2] && sc[2] <= 'A')
            result.g = (sc[2] - '0' + 10) << 4;

        if ('0' <= sc[3] && sc[3] <= '9')
            result.g = sc[3] - '0';
        else if ('a' <= sc[3] && sc[3] <= 'f')
            result.g = sc[3] - '0' + 10;
        else if ('A' <= sc[3] && sc[3] <= 'A')
            result.g = sc[3] - '0' + 10;

        if ('0' <= sc[4] && sc[4] <= '9')
            result.b = (sc[4] - '0') << 4;
        else if ('a' <= sc[4] && sc[4] <= 'f')
            result.b = (sc[4] - '0' + 10) << 4;
        else if ('A' <= sc[4] && sc[4] <= 'A')
            result.b = (sc[4] - '0' + 10) << 4;

        if ('0' <= sc[5] && sc[5] <= '9')
            result.b |= sc[5] - '0';
        else if ('a' <= sc[5] && sc[5] <= 'f')
            result.b |= sc[5] - '0' + 10;
        else if ('A' <= sc[5] && sc[5] <= 'A')
            result.b |= sc[5] - '0' + 10;
        return result;
    }
    return std::nullopt;
}