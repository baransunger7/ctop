#pragma once

#include <ftxui/component/component.hpp>
#include <mutex>
#include <vector>
#include "sysmon/models/process_info.hpp"
#include "sysmon/core/process_view.hpp"

namespace sysmon {
class MainWindow {
public:
    MainWindow( std::vector<ProcessInfo>& processData,
                std::mutex& dataMutex,
                ProcessView& viewEngine);


    ftxui::Component getRenderer();
private:
    std::vector<ProcessInfo>& processes;
    std::mutex& mt;
    ProcessView& view;
};
}