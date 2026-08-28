#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include "sysmon/commands/command.hpp"

namespace sysmon::commands {
class CommandManager {
using CommandFactory = std::function<std::unique_ptr<Command>(const std::vector<std::string>&)>;
public:
    CommandManager() = default;
    CommandResult executeFromString(const std::string& input);
    CommandResult executeCommand(std::unique_ptr<Command> command);
    void registerCommand(const std::string& commandName, CommandFactory factory);
private:
    std::vector<std::string> parseInput(const std::string& input);
    std::map<std::string, CommandFactory> commandRegistry_;
};
}