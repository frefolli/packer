#ifndef PACKER_SCHEDULING
#define PACKER_SCHEDULING
#include <packer/packerfile.hh>
#include <packer/package.hh>

namespace packer {
  bool schedule_packages(std::vector<Package*>& packages, const std::unordered_map<std::string, Packerfile>& packerfile_index);
}
#endif//PACKER_SCHEDULING
