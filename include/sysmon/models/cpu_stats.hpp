#pragma once
#include <cstdint>
#include <string>

namespace sysmon {
    struct CpuStats {
        std::string id;
        uint64_t user{0};
        uint64_t nice{0};
        uint64_t system{0};
        uint64_t idle{0};
        uint64_t iowait{0};
        uint64_t irq{0};
        uint64_t softirq{0};

        long long getActiveTime() const;
        long long getIdleTime() const;
        long long getTotalTime() const;
    };
}