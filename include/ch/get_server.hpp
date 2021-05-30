#ifndef TCX_CH_GET_SERVER_HPP
#define TCX_CH_GET_SERVER_HPP

#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ch {

extern std::unordered_map<std::string_view, int> const specials;
extern std::vector<std::pair<int, int>> const tsweights;

std::string get_server(std::string group);

} // namespace ch

#endif
