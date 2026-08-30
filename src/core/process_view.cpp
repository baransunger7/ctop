#include "sysmon/core/process_view.hpp"
#include <algorithm>
#include <ranges>

namespace sysmon {

    ProcessView& ProcessView::setFilter(std::string filter) {
        currentFilter = std::move(filter);
        return *this;
    }

    ProcessView& ProcessView::setSortKey(SortKey key) noexcept {
        currentSortKey = key;
        return *this;
    }

    ProcessView& ProcessView::toggleSortOrder() noexcept{
        reverseOrder = !reverseOrder;
        return *this;
    }

    ProcessView& ProcessView::scrollDown(size_t maxItems) noexcept {
        if (maxItems > 0 && selectedIndex < maxItems - 1) {
            selectedIndex++;
        }
        return *this;
    }

    ProcessView& ProcessView::scrollUp() noexcept {
        if (selectedIndex > 0) {
            selectedIndex--;
        }
        return *this;
    }

    ProcessView& ProcessView::setLimit(size_t newLimit) noexcept {
        limit = newLimit;
        return *this;
    }

    size_t ProcessView::getOffset() const noexcept { return offset; }
    size_t ProcessView::getSelectedIndex() const noexcept { return selectedIndex; }

    std::vector<ProcessInfo> ProcessView::applyView(const std::vector<ProcessInfo>& processes) noexcept {
        if (processes.empty()) {
            selectedIndex = 0;
            offset = 0;
            selectedPid = -1;
            return {};
        }

        std::vector<ProcessInfo> filteredProcesses;
        std::ranges::copy(processes | std::views::filter([this](const ProcessInfo& p) {
            return p.name.find(currentFilter) != std::string::npos;
        }), std::back_inserter(filteredProcesses));

        std::ranges::sort(filteredProcesses, [this](const ProcessInfo& a, const ProcessInfo& b) {
            if (!reverseOrder) {
                switch (this->currentSortKey) {
                    case SortKey::CPU:
                        if (a.cpuPercent == b.cpuPercent) return a.pid > b.pid;
                        return a.cpuPercent > b.cpuPercent;
                    case SortKey::RAM:
                        if (a.memPercent == b.memPercent) return a.pid > b.pid;
                        return a.memPercent > b.memPercent;
                    case SortKey::PID: return a.pid > b.pid;
                    case SortKey::NAME: return a.name > b.name;
                }
            } else {
                switch (this->currentSortKey) {
                    case SortKey::CPU:
                        if (a.cpuPercent == b.cpuPercent) return a.pid < b.pid;
                        return a.cpuPercent < b.cpuPercent;
                    case SortKey::RAM:
                        if (a.memPercent == b.memPercent) return a.pid < b.pid;
                        return a.memPercent < b.memPercent;
                    case SortKey::PID: return a.pid < b.pid;
                    case SortKey::NAME: return a.name < b.name;
                }
            }
            return false;
        });

        if (filteredProcesses.empty()) {
            selectedIndex = 0;
            offset = 0;
            selectedPid = -1;
            return {};
        }

        if (selectedIndex >= filteredProcesses.size()) {
            selectedIndex = filteredProcesses.size() - 1;
        }

        if (selectedIndex < offset) {
            offset = selectedIndex;
        } else if (selectedIndex >= offset + limit) {
            offset = selectedIndex - limit + 1;
        }

        if (offset >= filteredProcesses.size()) {
            offset = filteredProcesses.empty() ? 0 : filteredProcesses.size() - 1;
        }

        selectedPid = filteredProcesses[selectedIndex].pid;


        std::vector<ProcessInfo> pagedProcesses;
        std::ranges::copy(
            filteredProcesses | std::views::drop(offset) | std::views::take(limit),
            std::back_inserter(pagedProcesses)
        );

        return pagedProcesses;
    }

    int ProcessView::getSelectedPid() const noexcept { return selectedPid; }
}