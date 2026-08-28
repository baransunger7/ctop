#include "sysmon/core/system_reader.hpp"
#include "sysmon/core/process_reader.hpp"
#include "sysmon/utils/string_helpers.hpp"
#include <filesystem>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <array>
#include <sys/stat.h>
#include <pwd.h>
#include <cstdlib>
#include <unordered_set>
#include <chrono>

namespace fs = std::filesystem;
namespace sysmon {

    bool ProcessReader::parseStatus(int dirfd,ProcessInfo& info) {
        if (dirfd == -1) {return false;}
        int fd = openat(dirfd,"status",O_RDONLY);
        if (fd == -1) {
            close(fd);
            return false;
        }
        std::array<char,512> buffer;
        ssize_t bytesRead = read(fd, buffer.data(), buffer.size()-1);
        close(fd);
        if (bytesRead <= 0) {return false;}
        buffer[bytesRead] = '\0';
        std::string_view content(buffer.data(),static_cast<size_t>(bytesRead));
        bool foundName = false;
        bool foundState = false;

        while (!content.empty() && (!foundName || !foundState)) {
            size_t lineEnd =content.find('\n');
            std::string_view line = (lineEnd != std::string_view::npos) ? content.substr(0, lineEnd) : content;
            if (!foundName && line.rfind("Name:",0) == 0) {
                line.remove_prefix(5);

                while (!line.empty() && (line.front() == ' ' || line.front() == '\t')) {
                    line.remove_prefix(1);
                }
                if (!line.empty()) {
                    info.name = std::string(line);
                    foundName = true;
                }
            }
            else if (!foundState && line.rfind("State:",0) == 0) {
                line.remove_prefix(6);
                while (!line.empty() && (line.front() == ' ' || line.front() == '\t')) {
                    line.remove_prefix(1);
                }
                if (!line.empty()) {
                    info.state = line.front();
                    foundState = true;
                }
            }
            if (lineEnd == std::string_view::npos) {break;}
            content.remove_prefix(lineEnd+1);
        }
    return true;
    }

    bool ProcessReader::parseCmdline(int dirfd, ProcessInfo &info) {
        if (dirfd == -1) {return false;}
        int fd = openat(dirfd,"cmdline",O_RDONLY);
        if (fd == -1) { return false; }
        std::array<char,1024> buffer;
        ssize_t bytesRead = read(fd, buffer.data(), buffer.size()-1);
        close(fd);
        if (bytesRead == -1) {return false;}
        if (bytesRead == 0) {
            info.command = "[" + info.name + "]";
            return true;
        }
        for (char& c : buffer) {
            if (c == '\0') {
                c = ' ';
            }
        }
        std::string_view content(buffer.data(),static_cast<size_t>(bytesRead));
        while (!content.empty() && content.back() == ' ') {
            content.remove_suffix(1);
        }
        if (!content.empty()) {
            info.command = std::string(content);
            return true;
        }
        return false;
    }

    uid_t ProcessReader::getUID(int dirfd) {
        struct stat st;
        if (fstat(dirfd,&st) == 0) {
            return st.st_uid;
        }
        return -1;

    }

    std::string ProcessReader::getUserName(uid_t uid) {
        auto it = userCache.find(uid);
        if (it != userCache.end()) {
            return it->second;
        }

        struct passwd* pw = getpwuid(uid);
        if (pw != nullptr) {
            return pw->pw_name;
        }
        else {
            return std::to_string(uid);
        }
    }

    bool ProcessReader::parseUserName(int dirfd, ProcessInfo &info) {
        uid_t uid = ProcessReader::getUID(dirfd);
        if (uid == -1) {return false;}
        info.user = getUserName(uid);
        return true;
    }
    bool ProcessReader::parseStatm(int dirfd,ProcessInfo& info) {
        if (dirfd == -1) return false;
        int fd = openat(dirfd,"statm",O_RDONLY);
        if (fd == -1) return false;
        std::array<char,128> buffer;
        ssize_t bytesRead = read(fd, buffer.data(), buffer.size()-1);
        close(fd);
        if (bytesRead <= 0) return false;
        buffer[bytesRead] = '\0';

        static const long pageSizeKb = sysconf(_SC_PAGESIZE) / 1024;
        char* endPtr = nullptr;

        long virtPages = std::strtol(buffer.data(), &endPtr, 10);
        long resPages = std::strtol(endPtr, &endPtr, 10);

        info.virtMemKb = virtPages * pageSizeKb;
        info.resMemKb = resPages * pageSizeKb;
        return true;
    }

    bool ProcessReader::parseStat(int dirfd, ProcessInfo &info) {
        if (dirfd == -1) return false;
        int fd = openat(dirfd,"stat",O_RDONLY);
        if (fd == -1) return false;
        std::array<char,1024> buffer;
        ssize_t bytesRead = read(fd, buffer.data(), buffer.size()-1);
        if (bytesRead <= 0) return false;
        buffer[bytesRead] = '\0';
        close(fd);
        std::string_view content(buffer.data());

        size_t rightParen = content.find(')');
        if (rightParen == std::string_view::npos) return false;
        char* ptr = buffer.data() + rightParen + 3;
        int field = 4;

        while (*ptr != '\0' && field <= 22) {
            if (field == 14 || field == 15 || field == 22) {
                uint64_t val = std::strtoull(ptr,&ptr,10);

                if (field == 14) info.totalCpuTicks = val;
                else if (field == 15) info.totalCpuTicks += val;
                else if (field == 22) info.startTime = val;
            }
            else if (field == 18 || field == 19) {
                long val = std::strtol(ptr,&ptr,10);
                if (field == 18) info.priority = static_cast<int>(val);
                else if (field == 19) info.nice = static_cast<int>(val);
            }
            else {
                std::strtol(ptr,&ptr,10);
            }
            field++;
        }
        return true;
    }

    bool ProcessReader::parseProcessInfo(pid_t pid, ProcessInfo& info) {
        std::string path = "/proc/" + std::to_string(pid);
        info.pid = pid;
        int dirfd = open(path.c_str(), O_RDONLY | O_DIRECTORY);
        if (dirfd == -1) { return false; }
        bool succes = ProcessReader::parseStatus(dirfd,info)
                    && ProcessReader::parseCmdline(dirfd,info)
                    && this->parseUserName(dirfd,info)
                    && ProcessReader::parseStatm(dirfd,info)
                    && ProcessReader::parseStat(dirfd,info);

        close(dirfd);
        return succes;
    }
    std::vector<ProcessInfo> ProcessReader::getAllProcesses(const SystemReader& sysReader) {
        std::vector<ProcessInfo> processes;
        std::unordered_set<pid_t> seenPids;
        std::error_code err;
        long long totalRamKb = sysReader.getMemCache().at("MemTotal");
        auto now = std::chrono::steady_clock::now();
        double timeDeltaSec = std::chrono::duration<double>(now - lastReadTime).count();
        lastReadTime = now;
        static long hertz = sysconf(_SC_CLK_TCK);

        for (const auto& entry : fs::directory_iterator("/proc/",err)) {
            err.clear();
            std::string filename = entry.path().filename().string();
            if (entry.is_directory() && utils::isNumber(filename)) {
                auto pid = static_cast<pid_t>(std::stoi(filename));
                seenPids.insert(pid);
                ProcessInfo info;
                info.pid = pid;

                std::string path = "/proc/" + std::to_string(pid);
                int dirfd = open(path.c_str(), O_RDONLY | O_DIRECTORY);
                if (dirfd == -1) continue;

                if (parseStat(dirfd,info) && parseStatm(dirfd,info)) {
                    info.memPercent = (static_cast<double>(info.resMemKb)/totalRamKb) * 100.0;


                    auto it = processCache.find(pid);
                    if (it != processCache.end() && it->second.startTime == info.startTime) {
                        info.name = it->second.name;
                        info.user = it->second.user;
                        info.command = it->second.command;

                        uint64_t processDelta = info.totalCpuTicks - it->second.totalCpuTicks;
                        if (timeDeltaSec > 0.0) {
                            info.cpuPercent = ((static_cast<double>(processDelta) / hertz) /timeDeltaSec) * 100.0;
                        }
                    }else {
                        parseStatus(dirfd,info);
                        parseCmdline(dirfd,info);
                        parseUserName(dirfd,info);

                        info.cpuPercent = 0.0;
                    }
                    processes.push_back(info);
                    processCache[pid] = info;
                }
                close(dirfd);
            }
            if (err) {
                std::cerr << "Error Code: " << err.value() << " Message: " << err.message() << std::endl;
            }
        }
        for (auto it = processCache.begin(); it != processCache.end();) {
            if (seenPids.find(it->first) == seenPids.end()) {
                it = processCache.erase(it);
            }
            else {
                ++it;
            }
        }
        return processes;

    }

}
