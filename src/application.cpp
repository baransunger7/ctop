#include "sysmon/application.hpp"
#include "sysmon/core/system_reader.hpp"
#include "sysmon/models/system_info.hpp"
#include "sysmon/core/process_view.hpp"
#include "sysmon/core/process_reader.hpp"
#include <ftxui/component/screen_interactive.hpp>
#include <thread>
#include <chrono>
#include "sysmon/ui/main_window.hpp"
#include "sysmon/commands/command_manager.hpp"
#include "sysmon/commands/kill_command.hpp"
void Application::workerLoop(std::stop_token stopToken){
    while (!stopToken.stop_requested()) {
        auto freshData = procReader.getAllProcesses(sysReader);
        std::vector<double> cpuUsages = sysReader.getCpuUsages();


        if (!cpuUsages.empty()) {
            currentSysInfo.coresUsages = std::vector<float>(cpuUsages.begin() + 1, cpuUsages.end());
        }
        sysReader.refreshMemCache();
        auto memTotal = sysReader.getMemCache().at("MemTotal");
        auto usedMemory = memTotal - sysReader.getMemCache().at("MemAvailable");
        currentSysInfo.ramUsage = usedMemory/memTotal;


        {
            std::scoped_lock lock(mt);
            currProcesses = std::move(freshData);
        }
        if (screenPtr != nullptr) {
            screenPtr->PostEvent(ftxui::Event::Custom);
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}



void Application::run() {
    auto screen = ftxui::ScreenInteractive::Fullscreen();
    this->screenPtr = &screen;
    sysmon::commands::CommandManager cmdManager;
    cmdManager.registerCommand("kill",[](const std::vector<std::string>& args) -> std::unique_ptr<sysmon::commands::Command> {
        if (args.size() <2 ) return nullptr;
        try {
            if (args.size() == 2 ) {
                int pid = std::stoi(args[1]);
                return std::make_unique<sysmon::commands::KillCommand>(pid);
            }else if (args.size() >= 3) {
                int signal = std::abs(std::stoi(args[1]));
                int pid = std::stoi(args[2]);
                return std::make_unique<sysmon::commands::KillCommand>(pid,signal);
            }
        }catch (...) {
            return nullptr;
        }
        return nullptr;
    });
    workerThread = std::jthread(&Application::workerLoop, this);
    sysmon::MainWindow mainWindow(currProcesses,currentSysInfo,mt,view,cmdManager);
    screen.Loop(mainWindow.getRenderer());
    this->screenPtr = nullptr;
}
