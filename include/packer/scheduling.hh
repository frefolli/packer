#ifndef PACKER_SCHEDULING
#define PACKER_SCHEDULING
#include <packer/packerfile.hh>
#include <packer/package.hh>
#include <packer/specialization.hh>

namespace packer {
  bool analyze_packages(const packer::Host& host,
                        std::unordered_map<std::string, Package*>& packages,
                        std::unordered_map<std::string, Packerfile>& packerfile_index);
  bool schedule_packages(std::vector<packer::Package*>& schedule,
                         const std::unordered_map<std::string, packer::Package*>& packages);
}
#endif//PACKER_SCHEDULING
