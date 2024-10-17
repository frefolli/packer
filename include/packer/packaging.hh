#ifndef PACKER_PACKAGING
#define PACKER_PACKAGING
namespace packer {
  enum packaging_t {
    RPMBUILD, MAKEPKG
  };

  constexpr const char* RPMBUILD_PATH = "/usr/bin/rpmbuild";
  constexpr const char* MAKEPKG_PATH = "/usr/bin/makepkg";
}
#endif//PACKER_PACKAGING
