#include "sysmon/ui/main_window.hpp"
#include "sysmon/utils/string_helpers.hpp"
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/screen/terminal.hpp>

namespace sysmon {
    MainWindow::MainWindow(std::vector<ProcessInfo>& processData,SystemInfo& sysInfoData, std::mutex& dataMutex, ProcessView& viewEngine,commands::CommandManager& cmdManager)
        : processes{processData},sysInfo{sysInfoData},mt{dataMutex}, view{viewEngine},prompt_{cmdManager,[this] {this->isCommandMode = false;}}{}

    ftxui::Component MainWindow::getRenderer() {

        auto renderHeader = [this]() -> ftxui::Element {

            auto formatPct = [](float usage) {
                int pct = static_cast<int>(usage * 100.0f);
                if (pct < 10)  return "  " + std::to_string(pct) + "%";
                if (pct < 100) return " " + std::to_string(pct) + "%";
                return std::to_string(pct) + "%";
            };
            ftxui::Elements col1,col2,col3;
            size_t totalCores = sysInfo.coresUsages.size();
            size_t colSize = (totalCores + 2) / 3;
            for (size_t i = 0; i < totalCores; ++i) {
                std::string pad = (i < 10) ? " " :  "";
                auto cpuBar = ftxui::hbox({
                    ftxui::text("CPU" + pad + std::to_string(i) + " [") | ftxui::color(ftxui::Color::GreenLight),
                    ftxui::gauge(sysInfo.coresUsages[i]) | ftxui::color(ftxui::Color::Green),
                    ftxui::text("] " + formatPct(sysInfo.coresUsages[i])) | ftxui::color(ftxui::Color::GreenLight)
                });

                if (i < colSize) {
                    col1.push_back(cpuBar);
                } else if (i < colSize * 2) {
                    col2.push_back(cpuBar);
                } else {
                    col3.push_back(cpuBar);
                }
            }

            col1.push_back(
                ftxui::hbox({
                    ftxui::text("Mem   [") | ftxui::color(ftxui::Color::YellowLight),
                    ftxui::gauge(sysInfo.ramUsage) | ftxui::color(ftxui::Color::Yellow),
                    ftxui::text("] " + formatPct(sysInfo.ramUsage)) | ftxui::color(ftxui::Color::YellowLight)
                })
            );

            return ftxui::window(ftxui::text(" System Info "),
             ftxui::hbox({
                 ftxui::vbox(std::move(col1)) | ftxui::flex,
                 ftxui::text("   "), // 1. ve 2. sütun arası şeffaf boşluk
                 ftxui::vbox(std::move(col2)) | ftxui::flex,
                 ftxui::text("   "), // 2. ve 3. sütun arası şeffaf boşluk
                 ftxui::vbox(std::move(col3)) | ftxui::flex
             })
            );


        };

        auto uiRenderer = ftxui::Renderer([this]{
            std::vector<ProcessInfo> currentPage;
            {
                std::scoped_lock lock{this->mt};
                int terminalHeight = ftxui::Terminal::Size().dimy;
                size_t visibleRows = (terminalHeight > 13) ? (terminalHeight - 13) : 10;
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


        auto mainContainer = ftxui::Container::Vertical({
            uiRenderer,
            ftxui::Maybe(prompt_.getComponent(),&isCommandMode)
        });

        auto modeHandler = ftxui::CatchEvent(mainContainer, [this](ftxui::Event event) {


                    if (!isCommandMode) {
                        if (event == ftxui::Event::Character(':')) {
                            isCommandMode = true;
                            prompt_.getComponent()->TakeFocus();
                            return true;
                        }

                        if (event == ftxui::Event::ArrowDown || (event.is_mouse() && event.mouse().button == ftxui::Mouse::WheelDown)) {
                            size_t numberOfCurrentProcesses = 0;
                            {
                                std::scoped_lock lock{this->mt};
                                numberOfCurrentProcesses = this->processes.size();
                            }
                            this->view.scrollDown(numberOfCurrentProcesses);
                            return true;
                        }

                        if (event == ftxui::Event::ArrowUp || (event.is_mouse() && event.mouse().button == ftxui::Mouse::WheelUp)) {
                            this->view.scrollUp();
                            return true;
                        }
                    }
                    else {
                        if (event == ftxui::Event::Escape) {
                            isCommandMode = false;
                            prompt_.clearInput();
                            return true;
                        }
                        if (event == ftxui::Event::Return) {
                            prompt_.executeInput();
                            isCommandMode = false;
                            return true;
                        }
                        if (event.is_mouse() && event.mouse().button == ftxui::Mouse::WheelDown) {
                            size_t numberOfCurrentProcesses = 0;
                            {
                                std::scoped_lock lock{this->mt};
                                numberOfCurrentProcesses = this->processes.size();
                            }
                            this->view.scrollDown(numberOfCurrentProcesses);
                            return true;
                        }

                        if (event.is_mouse() && event.mouse().button == ftxui::Mouse::WheelUp) {
                            this->view.scrollUp();
                            return true;
                        }
                    }
                    return false;
                });

        return ftxui::Renderer(modeHandler, [this, uiRenderer,renderHeader] {
                return ftxui::vbox({
                    renderHeader(),
                    uiRenderer->Render() | ftxui::flex,
                    ftxui::separator(),
                    prompt_.Render(isCommandMode)
                });
        });

    }

}