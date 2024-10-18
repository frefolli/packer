#ifndef PACKER_ANALYSIS
#define PACKER_ANALYSIS
#include <packer/packerfile.hh>
#include <packer/package.hh>
#include <packer/host.hh>

namespace packer {
  bool analyze_packages(const packer::Host& host,
                        std::unordered_map<std::string, Package*>& packages,
                        std::unordered_map<std::string, Packerfile>& packerfile_index,
                        bool refresh, bool install);
}
#endif//PACKER_ANALYSIS
