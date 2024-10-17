#ifndef PACKER_SERDE
#define PACKER_SERDE
#include <packer/patchable.hh>
#include <packer/packerfile.hh>
#include <packer/specialization.hh>
#include <packer/package.hh>
#include <yaml-cpp/yaml.h>

namespace packer {
  void operator<<(YAML::Node& out, const Packerfile& packerfile);
  bool operator>>(const YAML::Node& in, Packerfile& packerfile);

  void operator<<(YAML::Node& out, const Patchable& patchable);
  bool operator>>(const YAML::Node& in, Patchable& patchable);

  std::ostream& operator<<(std::ostream& out, const Packerfile& packerfile);
  std::ostream& operator<<(std::ostream& out, const Patchable& patchable);

  std::ostream& operator<<(std::ostream& out, const Host& host);

  std::ostream& operator<<(std::ostream& out, const Locator& locator);
  std::ostream& operator<<(std::ostream& out, const Package& package);
}
#endif//PACKER_SERDE
