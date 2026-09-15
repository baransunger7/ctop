#pragma once
#include "sysmon/commands/command.hpp"


namespace sysmon::commands {
class ReniceCommand : public Command {
public:
    explicit ReniceCommand(const int pid,const int niceValue);
    [[nodiscard]] CommandResult execute() override;
    [[nodiscard]] std::string getName() const override;

private:
    int targetPid_;
    int niceValue_;
};
}
