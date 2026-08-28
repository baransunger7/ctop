#pragma once
#include <cstdint>
#include <sched.h>
#include <string>

namespace sysmon {
struct ProcessInfo {
    std::string name;
    std::string user;
    std::string command;
    uint64_t virtMemKb{0};
    uint64_t resMemKb{0};
    uint64_t startTime{0};
    uint64_t totalCpuTicks{0};
    double cpuPercent{0.0};
    double memPercent{0.0};
    pid_t pid;
    int priority{0};
    int nice{0};
    char state{'R'};

};
}
