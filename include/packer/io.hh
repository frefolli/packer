#ifndef PACKER_IO
#define PACKER_IO
#include <packer/packerfile.hh>

namespace packer {
  bool load_from_file(Packerfile& packerfile, const std::string& filepath);
  bool dump_into_file(const Packerfile& packerfile, const std::string& filepath);
};

#endif//PACKER_IO
