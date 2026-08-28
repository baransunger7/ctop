#pragma once
#include <unordered_map>
#include <vector>
#include "sysmon/models/process_info.hpp"
#include <chrono>
namespace sysmon {
class SystemReader;


class ProcessReader {
public:
    ProcessReader() = default;

    std::vector<ProcessInfo> getAllProcesses(const SystemReader& sysReader);

    ProcessInfo getProcessById(pid_t pid);
    bool parseProcessInfo(pid_t pid, ProcessInfo& Info);

private:
    std::chrono::steady_clock::time_point lastReadTime;
    std::unordered_map<pid_t, ProcessInfo> processCache;
    std::unordered_map<uid_t, std::string> userCache;
    bool static parseStatus(int dirfd,ProcessInfo& info);
    bool static parseCmdline(int dirfd,ProcessInfo& info);
    uid_t static getUID(int dirfd);
    std::string  getUserName(uid_t uid);
    bool  parseUserName(int dirfd,ProcessInfo& info);
    bool static parseStatm(int dirfd,ProcessInfo& info);
    bool static parseStat(int dirfd,ProcessInfo& info);
};
}