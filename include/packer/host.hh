#ifndef PACKER_HOST
#define PACKER_HOST
#include <string>
#include <optional>
#include <packer/packerfile.hh>
#include <packer/package_manager.hh>
#include <packer/packaging.hh>

namespace packer {
  struct Host {
    std::string distro;
    package_manager_t package_manager;
    packaging_t packaging;
    std::string homedir;
  };

  std::optional<std::string> identify_distro();
  std::optional<package_manager_t> identify_package_manager();
  std::optional<packaging_t> identify_packaging();
  std::optional<std::string> identify_homedir();
  bool probe_host(Host& out);

  void patch(const Host& host, packer::Packerfile& packerfile);
}
#endif//PACKER_HOST
