#pragma once
#include "sysmon/core/system_reader.hpp"
#include "sysmon/core/process_view.hpp"
#include "sysmon/core/process_reader.hpp"
#include "sysmon/models/system_info.hpp"
#include <mutex>
#include <thread>
#include <atomic>
#include <vector>
#include <ftxui/component/screen_interactive.hpp>


class Application {
public:
    void run();

private:
    sysmon::SystemReader sysReader;
    sysmon::ProcessReader procReader;
    sysmon::ProcessView view;

    ftxui::ScreenInteractive* screenPtr{nullptr};
    std::vector<sysmon::ProcessInfo> currProcesses;
    std::vector<sysmon::ProcessInfo> viewList;
    sysmon::SystemInfo currentSysInfo;
    std::mutex mt;

    std::jthread workerThread;

    void workerLoop(std::stop_token stopToken);
};