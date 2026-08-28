#include "sysmon/commands/kill_command.hpp"
#include <csignal>
#include <cerrno>
#include <cstring>

namespace sysmon::commands {
KillCommand::KillCommand(int pid,int signal)
    : targetPid_{pid},signal_(signal) {}

std::string KillCommand::getName() const {
    return "Kill (PID: " + std::to_string(targetPid_) + " , SIGNAL: " + std::to_string(signal_) + ")";
}

CommandResult KillCommand::execute() {
    if (::kill(targetPid_,signal_) == 0) {
        return CommandResult{
            true,
            "Successfully sent signal " + std::to_string(signal_) + " to process " + std::to_string(targetPid_)
        };
    }else {
        return CommandResult{
            false,
            "Failed to send signal to process " + std::to_string(targetPid_) + ": " + std::string(std::strerror(errno))
        };
    }
}
}
