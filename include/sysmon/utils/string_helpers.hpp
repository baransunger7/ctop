#pragma once
#include <string>
#include <sstream>
#include <string_view>
#include <iomanip>

namespace sysmon::utils {
bool isNumber(std::string_view str);

inline std::string formatPercent(double value,int precision=1) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}
}