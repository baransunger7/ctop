#include "sysmon/commands/renice_command.hpp"
#include <sys/resource.h>
#include <cerrno>
#include <cstring>

namespace sysmon::commands {
ReniceCommand::ReniceCommand(const int pid, const int niceValue)
    : targetPid_{pid},niceValue_{niceValue} {}


std::string ReniceCommand::getName() const {
    return "Renice (PID: " + std::to_string(targetPid_) + " , NICE: " + std::to_string(niceValue_) + ")";
}

CommandResult ReniceCommand::execute() {
    if (niceValue_ < -20) niceValue_ = -20;
    if (niceValue_ > 19) niceValue_ = 19;

    if (setpriority(PRIO_PROCESS, targetPid_,niceValue_) == -1) {
        return CommandResult{
            false,
            "Failed to  renice process" + std::to_string(targetPid_) + " to nice " + std::to_string(niceValue_) +  ": " + std::string(std::strerror(errno))
        };
    }else {
        return CommandResult{
            true,
            "Successfully reniced process" + std::to_string(targetPid_) + " to nice " + std::to_string(niceValue_)
        };
    }
}
}
