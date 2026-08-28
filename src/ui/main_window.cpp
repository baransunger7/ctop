#include "sysmon/ui/main_window.hpp"
#include "sysmon/utils/string_helpers.hpp"
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/screen/terminal.hpp>

namespace sysmon {
    MainWindow::MainWindow(std::vector<ProcessInfo>& processData, std::mutex& dataMutex, ProcessView& viewEngine)
        : processes{processData},mt{dataMutex}, view{viewEngine} {}

    ftxui::Component MainWindow::getRenderer() {

        auto uiRenderer = ftxui::Renderer([this]{
            std::vector<ProcessInfo> currentPage;
            {
                std::scoped_lock lock{this->mt};
                int terminalHeight = ftxui::Terminal::Size().dimy;
                size_t visibleRows = (terminalHeight > 2) ? (terminalHeight - 2) : 10;
                this->view.setLimit(visibleRows);
                currentPage = this->view.applyView(this->processes);
            }

            using namespace ftxui;
            std::vector<Element> rows;

            rows.push_back(
                hbox({
                    text("PID")  | size(WIDTH,EQUAL,8) | bold,
                    text("USER") | size(WIDTH,EQUAL,12) | bold,
                    text("CPU%") | size(WIDTH,EQUAL,8) | bold,
                    text("RAM%") | size(WIDTH,EQUAL,8) | bold,
                    text("NAME") | flex                  | bold,
                }) | bgcolor(Color::Green) | color(Color::Black)
            );

            int activePid = this->view.getSelectedPid();
            size_t currentOffset = this->view.getOffset();
            size_t rowIndex = 0;

            for (const auto& p : currentPage) {

                bool isSelected = (p.pid == activePid);
                auto rowInfo = hbox({
                    text(std::to_string(p.pid))                             | size(WIDTH,EQUAL,8) | bold,
                    text(" "),
                    text(p.user)                                         | size(WIDTH,EQUAL,8) | bold,
                    text(" "),
                    text(sysmon::utils::formatPercent(p.cpuPercent))        | size(WIDTH,EQUAL,8) | bold,
                    text(" "),
                    text(sysmon::utils::formatPercent(p.memPercent))        | size(WIDTH,EQUAL,8) | bold,
                    text(" "),
                    text(p.name)                                      | flex
                });

                if (isSelected) {
                    rowInfo = rowInfo | inverted;
                }
                rows.push_back(rowInfo);
                rowIndex++;
            }

            return window(text(" CTOP "),vbox(std::move(rows)));
        });

        auto eventListener = ftxui::CatchEvent(uiRenderer, [this](ftxui::Event event) {


            size_t numberOfCurrentProcesses = 0;

            {
                std::scoped_lock lock{this->mt};
                numberOfCurrentProcesses = this->processes.size();
            }

            if (event == ftxui::Event::ArrowDown || (event.is_mouse() && event.mouse().button == ftxui::Mouse::WheelDown)) {
                this->view.scrollDown(numberOfCurrentProcesses);
                return true;
            }
            if (event == ftxui::Event::ArrowUp || (event.is_mouse() && event.mouse().button == ftxui::Mouse::WheelUp)) {
                this->view.scrollUp();
                return true;
            }
            return false;
        });

        return eventListener;
    }

}