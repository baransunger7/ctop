#include "sysmon/core/system_reader.hpp"
#include "sysmon/utils/time_helpers.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
namespace sysmon {
SystemReader::SystemReader():memCache{readMemInfo()}{}
void SystemReader::refreshMemCache() {
    std::ifstream memInfo("/proc/meminfo");
    memCache.clear();
    std::string line,label;
    long long value;
    while (std::getline(memInfo, line)) {
        std::stringstream ss(line);
        if (!(ss >> label >> value)) break;
        label.pop_back();
        memCache[label] = value;
    }
    memInfo.close();
}
const std::unordered_map<std::string, long long>& SystemReader::getMemCache() const {
    return SystemReader::memCache;
}
std::unordered_map<std::string, long long> SystemReader::readMemInfo() {
    std::ifstream memInfo("/proc/meminfo");
    std::string line,label;
    long long value;
    std::unordered_map<std::string, long long> result;
    while (std::getline(memInfo, line)) {
        std::stringstream ss(line);
        if (!(ss >> label >> value)) break;
        label.pop_back();
        result.insert({label, value});
    }
    memInfo.close();
    return result;
}


std::string SystemReader::readUpTimeStr() {
    std::ifstream uptimeFile("/proc/uptime");
    double totalSeconds;
    uptimeFile >> totalSeconds;
    uptimeFile.close();
    return utils::formatTime(static_cast<long long>(totalSeconds));
}

std::string SystemReader::readKernelVersion() {
    std::ifstream osRelease("/proc/sys/kernel/osrelease");
    std::string kernelVersion;
    std::getline(osRelease, kernelVersion);
    osRelease.close();
    return kernelVersion;
}

std::string SystemReader::readHostname() {
    std::ifstream hostnameFile("/proc/sys/kernel/hostname");
    std::string hostname;
    std::getline(hostnameFile, hostname);
    hostnameFile.close();
    return hostname;
}


long long CpuStats::getActiveTime() const {
    return user + nice + system + irq + softirq;
}

long long CpuStats::getIdleTime() const {
    return idle + iowait;
}

long long CpuStats::getTotalTime() const {
    return getActiveTime() + getIdleTime();
}


std::vector<CpuStats> SystemReader::readCpuStats() {
    std::vector<CpuStats> currentSnapshot;
    std::ifstream statsFile("/proc/stat");
    std::string line;
    if (!statsFile) return currentSnapshot;
    while (std::getline(statsFile, line)) {
        std::stringstream ss(line);
        std::string coreId;
        ss >> coreId;
        if (line.compare(0,3,"cpu") == 0) {
            continue;
        }
        CpuStats stats;
        stats.id = coreId;
        ss >> stats.user >> stats.nice >> stats.system >> stats.idle
            >> stats.iowait >> stats.irq >> stats.softirq;

        currentSnapshot.push_back(stats);
    }
    statsFile.close();
    return currentSnapshot;
}

long long SystemReader::getGlobalTickDelta() const {
    if (prevCoresStats.empty() || currCoresStats.empty()) return 0;
    return lastGlobalDelta;
}


std::vector<double> SystemReader::getCpuUsages(){
    currCoresStats = readCpuStats();
    std::vector<double> cpuUsages;


    if (prevCoresStats.empty() || prevCoresStats.size() != currCoresStats.size()) {
        prevCoresStats = currCoresStats;
        return std::vector<double>(currCoresStats.size() , 0.0);
    }

    lastGlobalDelta = currCoresStats[0].getTotalTime() - prevCoresStats[0].getTotalTime();
    for (size_t i = 0; i != currCoresStats.size(); i++) {
        const auto& curr = currCoresStats[i];
        const auto& prev = prevCoresStats[i];

        long long totalDelta = curr.getTotalTime() - prev.getTotalTime();
        long long idleDelta = curr.getIdleTime() - prev.getIdleTime();

        double usage = 0.0;

        if (totalDelta > 0) {
            long long activeDelta = totalDelta - idleDelta;
            usage = (static_cast<double>(activeDelta)*100)/static_cast<double>(totalDelta);
        }
        cpuUsages.push_back(usage);
    }
    prevCoresStats=currCoresStats;
    return cpuUsages;
}


}



