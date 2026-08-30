#pragma once
#include <ftxui/component/component.hpp>
#include <mutex>
#include <vector>
#include "sysmon/models/process_info.hpp"
#include "sysmon/core/process_view.hpp"
#include "sysmon/ui/command_prompt.hpp"
#include "sysmon/commands/command_manager.hpp"
#include "sysmon/models/system_info.hpp"

namespace sysmon {
class MainWindow {
public:
    MainWindow( std::vector<ProcessInfo>& processData,
                SystemInfo& sysInfoData,
                std::mutex& dataMutex,
                ProcessView& viewEngine,
                commands::CommandManager& cmdManager);


    ftxui::Component getRenderer();
private:
    std::vector<ProcessInfo>& processes;
    std::mutex& mt;
    ProcessView& view;
    CommandPrompt prompt_;
    SystemInfo& sysInfo;
    bool isCommandMode{false};
};
}