#ifndef PACKER_SYSTEM
#define PACKER_SYSTEM
#include <string>
#include <optional>

namespace packer {
  bool execute_shell_command(const std::string& command);
  std::optional<std::string> get_stdout_of_shell_command(const std::string& command);
  bool download_file(const std::string& url, const std::string& out);
}
#endif//PACKER_SYSTEM
