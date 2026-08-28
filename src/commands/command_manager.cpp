#include "sysmon/commands/command_manager.hpp"
#include "sysmon/commands/kill_command.hpp"
#include <sstream>
#include <cmath>

namespace sysmon::commands {
void CommandManager::registerCommand(const std::string& commandName, CommandFactory factory) {
    commandRegistry_[commandName] = std::move(factory);
}



std::vector<std::string> CommandManager::parseInput(const std::string& input) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream{input};

    while (tokenStream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

CommandResult CommandManager::executeCommand(std::unique_ptr<Command> command) {
    if (!command) {
        return {false,"Invalid command object"};
    }
    return command->execute();
}

CommandResult CommandManager::executeFromString(const std::string& input) {
    if (input.empty()) {
        return {false, "Empty Command"};
    }
    auto args = parseInput(input);
    const std::string& commandName = args[0];

    auto it = commandRegistry_.find(commandName);
    if (it == commandRegistry_.end()) {
        return {false, "Unknown command " + commandName};
    }
    auto cmd = it->second(args);
    if (cmd) {
        return executeCommand(std::move(cmd));
    }
    return {false, "Unknown command " + commandName};
}



}



