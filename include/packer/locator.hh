#ifndef PACKER_LOCATOR
#define PACKER_LOCATOR
#include <string>

namespace packer {
  struct Locator {
    std::string group;
    std::string name;
    std::string url;
    std::string path;

    std::string id();
  };

  bool is_package_id(const std::string& maybe_package_id);
}
#endif//PACKER_LOCATOR
