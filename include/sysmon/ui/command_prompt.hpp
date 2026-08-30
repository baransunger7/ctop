#pragma once
#include <ftxui/component/component.hpp>
#include <string>
#include "sysmon/commands/command_manager.hpp"
#include <functional>

namespace sysmon {
class CommandPrompt {
public:
    explicit CommandPrompt(commands::CommandManager& cmdManager,std::function<void()> onEnterCallback);
    ftxui::Component getComponent();
    ftxui::Element Render(bool isActive);
    void clearInput();
    void executeInput();
private:
    commands::CommandManager& cmdManager_;
    std::string inputStr_;
    std::string statusMsg_;
    ftxui::Component inputComponent_;
};
}