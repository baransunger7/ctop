#include "sysmon/utils/time_helpers.hpp"
#include <sstream>


namespace sysmon::utils {
    std::string formatTime(long long totalSeconds) {
        constexpr long long SECONDS_IN_YEAR = 31536000;
        constexpr long long SECONDS_IN_MONTH = 2592000;
        constexpr long long SECONDS_IN_DAY = 86400;
        constexpr long long SECONDS_IN_HOUR = 3600;
        constexpr long long SECONDS_IN_MINUTE = 60;

        long long years = totalSeconds / SECONDS_IN_YEAR;
        totalSeconds %= SECONDS_IN_YEAR;
        long long months = totalSeconds / SECONDS_IN_MONTH;
        totalSeconds %= SECONDS_IN_MONTH;
        long long days = totalSeconds / SECONDS_IN_DAY;
        totalSeconds %= SECONDS_IN_DAY;
        long long hours = totalSeconds / SECONDS_IN_HOUR;
        totalSeconds %= SECONDS_IN_HOUR;
        long long minutes = totalSeconds / SECONDS_IN_MINUTE;
        long long seconds = totalSeconds % SECONDS_IN_MINUTE;

        std::stringstream ss;
        if (years > 0) ss << years << "y ";
        if (months > 0) ss << months << "m ";
        if (days > 0) ss << days << "d ";
        if (hours > 0) ss << hours << "h ";
        if (minutes > 0) ss << minutes << "min ";
        if (seconds > 0) ss << seconds << "s ";

        return ss.str();
    }
}