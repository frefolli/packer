#ifndef PACKER_PACKAGING
#define PACKER_PACKAGING
#include <packer/package.hh>
#include <optional>

namespace packer {
  enum packaging_t {
    RPMBUILD, MAKEPKG, XBPS_SRC
  };

  constexpr const char* RPMBUILD_PATH = "/usr/bin/rpmbuild";
  constexpr const char* MAKEPKG_PATH = "/usr/bin/makepkg";
  constexpr const char* XBPS_SRC_PATH = "/usr/sbin/xbps-src";

  std::optional<std::string> craft_package(const std::string& homedir, packaging_t packaging, Package* package);
}
#endif//PACKER_PACKAGING
