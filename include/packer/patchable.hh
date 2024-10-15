#ifndef PACKER_PATCHABLE
#define PACKER_PATCHABLE
#include <string>
#include <vector>

namespace packer {
  struct Patchable {
    std::vector<std::string> makedepends;
    std::vector<std::string> depends;
    std::string build_script;
    std::string install_script;
  };
}
#endif//PACKER_PATCHABLE
