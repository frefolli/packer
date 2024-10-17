#include <cstdlib>
#include <packer/package_manager.hh>
#include <packer/logging.hh>
#include <packer/system.hh>
#include <regex>

bool packer::install_system_dependencies(packer::package_manager_t package_manager, const std::vector<std::string>& dependencies) {
  if (dependencies.empty())
    return true;

  auto buffer = MSG("");
  switch (package_manager) {
    case packer::DNF: buffer << "sudo " << DNF_PATH << " install"; break;
    case packer::YUM: buffer << "sudo " << YUM_PATH << " install"; break;
    case packer::YAY: buffer << YAY_PATH << " -Syu"; break;
    case packer::PACMAN: buffer << "sudo " << PACMAN_PATH << " -Syu"; break;
    default: raise_error(1, MSG("package_manager := " << package_manager));
  }
  for (const std::string& dependency : dependencies) {
    buffer << " " << dependency;
  }
  return packer::execute_shell_command(buffer.str());
}

bool packer::install_packer_package(packer::package_manager_t package_manager, std::string package_path) {
  auto buffer = MSG("");
  switch (package_manager) {
    case packer::DNF: buffer << "sudo " << DNF_PATH << " install"; break;
    case packer::YUM: buffer << "sudo " << YUM_PATH << " install"; break;
    case packer::YAY: buffer << YAY_PATH << " -U"; break;
    case packer::PACMAN: buffer << "sudo " << PACMAN_PATH << " -U"; break;
    default: raise_error(1, MSG("package_manager := " << package_manager));
  }
  buffer << " " << package_path;
  return packer::execute_shell_command(buffer.str());
}

bool packer::filter_installed_packages(packer::package_manager_t package_manager, std::vector<std::string>& maybe_installed_packages) {
  if (maybe_installed_packages.empty())
    return true;

  auto buffer = MSG("");
  switch (package_manager) {
    case packer::DNF: buffer << "dnf info "; break;
    case packer::YUM: buffer << "dnf info "; break;
    case packer::YAY: buffer << "yay -Qi "; break;
    case packer::PACMAN: buffer << "yay -Qi "; break;
    default: raise_error(1, MSG("package_manager := " << package_manager));
  }
  for (const std::string& package : maybe_installed_packages) {
    buffer << " " << package;
  }
  switch (package_manager) {
    case packer::DNF: buffer << " --installed | grep \"Name\\s\\+: \""; break;
    case packer::YUM: buffer << " --installed | grep \"Name\\s\\+: \""; break;
    case packer::YAY: buffer << " | grep \"Name\\s\\+: \""; break;
    case packer::PACMAN: buffer << " | grep \"Name\\s\\+: \""; break;
    default: raise_error(1, MSG("package_manager := " << package_manager));
  }
  std::optional<std::string> maybe_stdout = packer::get_stdout_of_shell_command(buffer.str());
  if (!maybe_stdout.has_value()) {
    return false;
  }
  std::regex regex ("Name\\s+:\\s+([a-zA-Z_0-9-]+)");
  auto matches_begin = std::sregex_iterator(maybe_stdout.value().begin(), maybe_stdout.value().end(), regex);
  auto matches_end = std::sregex_iterator();
  auto matches_it = matches_begin;
  while (matches_it != matches_end) {
    std::smatch match = *matches_it;
    auto package_it = std::find(maybe_installed_packages.begin(), maybe_installed_packages.end(), match[1]);
    if (package_it != maybe_installed_packages.end()) {
      maybe_installed_packages.erase(package_it);
    }
    ++matches_it;
  }
  return true;
}
