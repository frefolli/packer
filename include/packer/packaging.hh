#ifndef PACKER_PACKAGING
#define PACKER_PACKAGING
#include <packer/package.hh>
#include <optional>

namespace packer {
  enum packaging_t {
    RPMBUILD, MAKEPKG
  };

  constexpr const char* RPMBUILD_PATH = "/usr/bin/rpmbuild";
  constexpr const char* MAKEPKG_PATH = "/usr/bin/makepkg";

  std::optional<std::string> craft_package(const std::string& homedir, packaging_t packaging, Package* package);
}
#endif//PACKER_PACKAGING
