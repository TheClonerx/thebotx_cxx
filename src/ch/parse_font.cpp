#include <ch/parse_font.hpp>

#include <ctre.hpp>

#include <charconv>

static constexpr ctll::fixed_string pattern = " x([0-9]{2})([a-fA-F0-9]{3}|[a-fA-F0-9]{6})=\"(.*)\"";

std::optional<ch::parse_font_result> ch::parse_font(std::string_view f) noexcept
{
    auto match = ctre::match<pattern>(f);
    if (match) {
        parse_font_result result;
        auto sz = match.get<1>().to_view();
        std::from_chars(sz.data(), sz.data() + sz.size(), result.size);
        result.color = match.get<2>().to_view();
        result.font = match.get<3>().to_view();
        return result;
    }
    return std::nullopt;
}
