#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "sysmon/models/cpu_stats.hpp"
namespace sysmon {
class SystemReader {
public:
    SystemReader();
    void refreshMemCache();
    static std::string readUpTimeStr();
    const std::unordered_map<std::string, long long>& getMemCache() const;
    static std::string readKernelVersion();
    static std::string readHostname();
    std::vector<double> getCpuUsages();
    long long getGlobalTickDelta() const;

private:
    long long lastGlobalDelta{0};
    std::unordered_map<std::string, long long> memCache;
    static std::unordered_map<std::string, long long> readMemInfo();
    std::vector<CpuStats> prevCoresStats;
    std::vector<CpuStats> currCoresStats;
    std::vector<CpuStats> readCpuStats();

};






}

