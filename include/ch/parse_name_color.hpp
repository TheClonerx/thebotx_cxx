#pragma once

#include <ch/Color.hpp>

#include <optional>
#include <string_view>

namespace ch {

std::optional<ch::Color> parse_name_color(std::string_view s) noexcept;

} // namespace ch
