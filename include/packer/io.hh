#ifndef PACKER_IO
#define PACKER_IO
#include <optional>
#include <packer/packerfile.hh>
#include <packer/locator.hh>

namespace packer {
  bool load_from_file(Packerfile& packerfile, const std::string& filepath);
  bool dump_into_file(const Packerfile& packerfile, const std::string& filepath);
  std::optional<Locator> parse_locator(const std::string& package_id);
  bool load_from_package_id(Packerfile& packerfile, const std::string& package_id);
};
#endif//PACKER_IO
