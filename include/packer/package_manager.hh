#ifndef PACKER_PACKAGE_MANAGER
#define PACKER_PACKAGE_MANAGER
#include <string>
#include <vector>
namespace packer {
  enum package_manager_t {
    DNF, YUM, YAY, PACMAN, XBPS
  };

  constexpr const char* DNF_PATH = "/usr/bin/dnf";
  constexpr const char* YUM_PATH = "/usr/bin/yum";
  constexpr const char* YAY_PATH = "/usr/bin/yay";
  constexpr const char* PACMAN_PATH = "/usr/bin/pacman";
  constexpr const char* XBPS_INSTALL_PATH = "/usr/sbin/xbps-install";
  constexpr const char* XBPS_QUERY_PATH = "/usr/sbin/xbps-query";

  bool install_system_dependencies(package_manager_t package_manager, const std::vector<std::string>& dependencies);
  bool install_packer_package(package_manager_t package_manager, std::string package_path);
  bool filter_installed_packages(package_manager_t package_manager, std::vector<std::string>& maybe_installed_packages);
}
#endif//PACKER_PACKAGE_MANAGER
