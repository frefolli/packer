#ifndef PACKER_SCHEDULING
#define PACKER_SCHEDULING
#include <packer/packerfile.hh>
#include <packer/package.hh>
#include <packer/specialization.hh>

namespace packer {
  bool schedule_packages(const packer::Host& host,
                         std::unordered_map<std::string, Package*>& packages,
                         std::unordered_map<std::string, Packerfile>& packerfile_index);
}
#endif//PACKER_SCHEDULING
