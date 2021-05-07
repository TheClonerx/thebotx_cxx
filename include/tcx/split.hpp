#include <string_view>
#include <vector>

namespace tcx::impl {

template <typename It, typename CharT, typename Traits>
constexpr It split_to(It it, std::basic_string_view<CharT, Traits> hay, std::basic_string_view<CharT, Traits> needle, std::size_t max) noexcept(noexcept(*it = std::declval<std::basic_string_view<CharT, Traits>>()) && noexcept(++it))
{
    using view_type = std::basic_string_view<CharT, Traits>;
    while (!hay.empty() && max) {
        if (auto pos = hay.find(needle); pos == view_type::npos) {
            *it = hay;
            break;
        } else {
            *it = hay.substr(0, pos);
            hay.remove_prefix(pos + needle.size());
        }
        ++it;
        --max;
    }
    return it;
}

} // namespace tcx::impl

namespace tcx {

#define VECTOR_SPLIT(view_type)                                                                           \
    std::vector<view_type> split(view_type hay, view_type needle, std::size_t max = SIZE_MAX)             \
    {                                                                                                     \
        std::vector<view_type> result;                                                                    \
        tcx::impl::split_to(std::back_inserter(result), hay, needle, max);                                \
        return result;                                                                                    \
    }                                                                                                     \
    std::vector<view_type> split(view_type hay, view_type::value_type needle, std::size_t max = SIZE_MAX) \
    {                                                                                                     \
        std::vector<view_type> result;                                                                    \
        tcx::impl::split_to(std::back_inserter(result), hay, view_type { &needle, 1 }, max);              \
        return result;                                                                                    \
    }
VECTOR_SPLIT(std::string_view)
VECTOR_SPLIT(std::wstring_view)
VECTOR_SPLIT(std::u8string_view)
VECTOR_SPLIT(std::u16string_view)
VECTOR_SPLIT(std::u32string_view)
#undef VECTOR_SPLIT

#define SPLIT_TO(view_type)                                                                                         \
    template <typename It>                                                                                          \
    std::vector<view_type> split_to(It it, view_type hay, view_type needle, std::size_t max = SIZE_MAX)             \
    {                                                                                                               \
        return tcx::impl::split_to(it, hay, needle, max);                                                           \
    }                                                                                                               \
    template <typename It>                                                                                          \
    std::vector<view_type> split_to(It it, view_type hay, view_type::value_type needle, std::size_t max = SIZE_MAX) \
    {                                                                                                               \
        return tcx::impl::split_to(it, hay, view_type { &needle, 1 }, max);                                         \
    }
SPLIT_TO(std::string_view)
SPLIT_TO(std::wstring_view)
SPLIT_TO(std::u8string_view)
SPLIT_TO(std::u16string_view)
SPLIT_TO(std::u32string_view)
#undef SPLIT_TO

} // namespace tcx
