#ifndef PACKER_IO
#define PACKER_IO
#include <optional>
#include <packer/packerfile.hh>
#include <packer/locator.hh>
#include <packer/host.hh>

namespace packer {
  extern std::string REPOSITORY_DIRECTORY;
  extern std::string URL_BASE;
  bool load_from_file(Packerfile& packerfile, const std::string& filepath);
  bool dump_into_file(const Packerfile& packerfile, const std::string& filepath);
  std::optional<Locator> parse_locator(const std::string& package_id);
  bool load_from_package_id(const Host& host, Packerfile& packerfile, const std::string& package_id);
};
#endif//PACKER_IO
