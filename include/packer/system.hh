#ifndef PACKER_SYSTEM
#define PACKER_SYSTEM
#include <string>
#include <optional>

namespace packer {
  bool execute_shell_command(const std::string& command);
  std::optional<std::string> get_stdout_of_shell_command(const std::string& command);
}
#endif//PACKER_SYSTEM
