#pragma once

#include <charconv>
#include <optional>
#include <string_view>

namespace tcx {
template <typename T>
constexpr std::optional<T> parse_int(std::string_view s, int base = 10) requires(std::is_integral_v<T>)
{
    T result;
    auto const begin = s.data();
    auto const end = begin + s.size();
    auto [ptr, errc] = std::from_chars(begin, end, result, base);
    if (ptr != end || errc != std::errc {})
        return std::nullopt;
    else
        return result;
}

} // namespace tcx
