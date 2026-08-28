#pragma once
#include <csignal>
#include "sysmon/commands/command.hpp"

namespace sysmon::commands {
class KillCommand : public Command {
public:
    explicit KillCommand(int pid,int signal = SIGTERM);
    [[nodiscard]] CommandResult execute() override;
    [[nodiscard]] std::string getName() const override;
private:
    int targetPid_;
    int signal_;
};
}
