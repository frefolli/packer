#ifndef PACKER_SPECIALIZATION
#define PACKER_SPECIALIZATION
#include <string>
#include <optional>
#include <packer/packerfile.hh>

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

  void patch(const Host& host, packer::Packerfile& packerfile);
}
#endif//PACKER_SPECIALIZATION
