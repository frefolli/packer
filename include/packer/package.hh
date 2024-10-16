#ifndef PACKER_PACKAGE
#define PACKER_PACKAGE
#include <string>
#include <vector>

namespace packer {
  struct Package {
    std::string version;
    std::string license;
    std::string summary;

    std::vector<std::string> build_system_requirements;
    std::vector<std::string> install_system_requirements;

    std::vector<Package*> build_packer_requirements;
    std::vector<Package*> install_packer_requirements;

    std::string build_script;
    std::string install_script;
  };
}
#endif//PACKER_PACKAGE
