#ifndef PACKER_PACKERFILE
#define PACKER_PACKERFILE
#include <packer/patchable.hh>
#include <packer/locator.hh>
#include <unordered_map>
#include <string>

namespace packer {
  struct Packerfile {
    packer::Locator locator;
    std::string version;
    std::string license;
    std::string summary;
    Patchable baseline;
    std::unordered_map<std::string, Patchable> patches;
  };
}
#endif//PACKER_PACKERFILE
