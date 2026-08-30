#pragma once
#include <vector>
#include <cstddef>

namespace sysmon {
struct SystemInfo {
    float ramUsage{0.0f};
    std::vector<float> coresUsages;
    int totalTasks{0};
    int runningTasks{0};
    int sleepingTasks{0};
};
}