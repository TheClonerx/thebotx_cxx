#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace ch {

std::optional<std::string> get_anon_id(std::string_view n, std::string_view ssid);

} // namespace ch
