#pragma once
#include <string>
#include <vector>
#include "sysmon/models/process_info.hpp"

namespace sysmon {
enum class SortKey {
    CPU,
    RAM,
    PID,
    NAME
};

class ProcessView {
public:
    ProcessView& setSortKey(SortKey key) noexcept;
    ProcessView& setFilter(std::string filter);
    ProcessView& toggleSortOrder() noexcept;
    ProcessView& scrollUp() noexcept;
    ProcessView& scrollDown(size_t maxItems) noexcept;
    ProcessView& setLimit(size_t newLimit) noexcept;
    [[nodiscard]] std::vector<ProcessInfo> applyView(const std::vector<ProcessInfo>& processes) noexcept;
    size_t getSelectedIndex() const noexcept;
    size_t getOffset() const noexcept;
    int getSelectedPid() const noexcept;

private:
    SortKey currentSortKey{SortKey::CPU};
    std::string currentFilter;
    size_t offset{0};
    size_t limit{15};
    size_t selectedIndex{0};
    int selectedPid{-1};
    bool reverseOrder{false};

};
}
