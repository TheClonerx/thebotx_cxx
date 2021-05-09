#include <ch/get_anon_id.hpp>

#include <algorithm>

using namespace std::literals;

std::optional<std::string> ch::get_anon_id(std::string_view n, std::string_view ssid)
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
        return std::nullopt;
    return result;
}