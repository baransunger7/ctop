#pragma once
#include <vector>
#include <cstddef>

namespace sysmon {
    struct SystemInfo {
        double globalCpuPercent{0.0};
        std::vector<double> coresPercent;
        double ramPercent{0.0};
        size_t totalTasks{0};
    };
}