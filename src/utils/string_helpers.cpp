#include "sysmon/utils/string_helpers.hpp"
#include <algorithm>
#include <cctype>


namespace sysmon::utils {
bool isNumber(std::string_view str) {
    return std::ranges::all_of(str, [](unsigned char c) { return std::isdigit(c); });
}
}
