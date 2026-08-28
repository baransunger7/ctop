#include "sysmon/application.hpp"
#include "sysmon/core/system_reader.hpp"
#include "sysmon/models/system_info.hpp"
#include "sysmon/core/process_view.hpp"
#include "sysmon/core/process_reader.hpp"
#include <ftxui/component/screen_interactive.hpp>
#include <thread>
#include <chrono>

#include "sysmon/ui/main_window.hpp"

void Application::workerLoop(std::stop_token stopToken){
    while (!stopToken.stop_requested()) {
        auto freshData = procReader.getAllProcesses(sysReader);
        std::vector<double> cpuUsages = sysReader.getCpuUsages();

        sysmon::SystemInfo newSysInfo;

        if (!cpuUsages.empty()) {
            newSysInfo.globalCpuPercent = cpuUsages[0]; // Genel CPU

            for (size_t i = 1; i < cpuUsages.size(); ++i) {
                newSysInfo.coresPercent.push_back(cpuUsages[i]);
            }
        }

        newSysInfo.ramPercent = sysReader.getRamUsagePercent();
        newSysInfo.totalTasks = newProcesses.size();
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

    workerThread = std::jthread(&Application::workerLoop, this);

    sysmon::MainWindow mainWindow(currProcesses,mt,view);
    screen.Loop(mainWindow.getRenderer());
    this->screenPtr = nullptr;
}
