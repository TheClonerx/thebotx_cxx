#ifndef TCX_CH_PARSE_FONT_HPP
#define TCX_CH_PARSE_FONT_HPP

#include <optional>
#include <string_view>

namespace ch {

struct parse_font_result {
    int size;
    std::string_view color;
    std::string_view font;
};

std::optional<parse_font_result> parse_font(std::string_view f) noexcept;

} // namespace ch

#endif
