#include "sysmon/ui/command_prompt.hpp"
#include <functional>
namespace sysmon {

CommandPrompt::CommandPrompt(commands::CommandManager &cmdManager, std::function<void()> onEnterCallback)
    : cmdManager_{cmdManager},statusMsg_{"Ready."} {
    ftxui::InputOption option;
    option.on_enter = [this,onEnterCallback] {
        if (inputStr_.empty()) {
            onEnterCallback();
            return;
        }
        auto result = cmdManager_.executeFromString(inputStr_);
        statusMsg_ = result.message;
        clearInput();
        onEnterCallback();
    };

    inputComponent_ = ftxui::Input(&inputStr_,"Enter Command",option);
}

void CommandPrompt::clearInput() { inputStr_.clear(); }


ftxui::Component CommandPrompt::getComponent() {
    return inputComponent_;
}

ftxui::Element CommandPrompt::Render(bool isActive) {
    if (isActive) {
        return ftxui::hbox({
            ftxui::text(":") | ftxui::bold | ftxui::color(ftxui::Color::GreenLight),
            inputComponent_->Render() | ftxui::flex
        });
    }
    return ftxui::text(statusMsg_) | ftxui::color(ftxui::Color::GrayDark);
}

void CommandPrompt::executeInput() {
    if (inputStr_.empty()) return;
    auto result = cmdManager_.executeFromString(inputStr_);
    statusMsg_ = result.message;
    clearInput();
}
}
