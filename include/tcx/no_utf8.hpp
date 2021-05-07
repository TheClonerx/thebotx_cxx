#pragma once

#include <string_view>

namespace tcx {

inline std::string_view no_utf8(std::u8string_view sv) noexcept
{
    return { reinterpret_cast<char const*>(sv.data()), sv.size() };
}

} // namespace tcx
