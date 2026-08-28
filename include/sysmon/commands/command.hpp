#pragma once
#include <string>
namespace sysmon::commands {

struct CommandResult {
  bool success{false};
  std::string message;
};

class Command {
public:
  virtual ~Command() = default;
  [[nodiscard]] virtual CommandResult execute() = 0;
  [[nodiscard]] virtual std::string getName() const  = 0;
};
}
