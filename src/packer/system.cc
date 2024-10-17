#include <packer/system.hh>
#include <packer/logging.hh>
#include <memory>
#include <array>

bool packer::execute_shell_command(const std::string& command) {
  throw_debug(MSG("|> " << command));
  if (system(command.c_str()) != 0) {
    packer::throw_warning(MSG("command '" << command << "' failed"));
    return false;
  }
  return true;
}

std::optional<std::string> packer::get_stdout_of_shell_command(const std::string& command) {
  throw_debug(MSG("|> " << command));
  std::array<char, 1024> buffer;
  std::string result;
  std::unique_ptr<FILE, int(*)(FILE*)> pipe(popen(command.c_str(), "r"), pclose);
  if (!pipe) {
    packer::throw_warning(MSG("command '" << command << "' failed"));
    return std::nullopt;
  }
  while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return result;
}

bool packer::download_file(const std::string& url, const std::string& out) {
  return packer::execute_shell_command(MSG("wget " << url << " -O " << out).str());
}
