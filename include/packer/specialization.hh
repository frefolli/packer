#ifndef PACKER_SPECIALIZATION
#define PACKER_SPECIALIZATION
#include <string>
#include <optional>

namespace packer {
  struct Host {
    std::string distro;
    std::string package_manager;
    std::string packaging;
  };

  std::optional<std::string> identify_distro();
  std::optional<std::string> identify_package_manager();
  std::optional<std::string> identify_packaging();
  bool probe_host(Host& out);
}
#endif//PACKER_SPECIALIZATION
