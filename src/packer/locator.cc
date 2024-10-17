#include <packer/locator.hh>

bool packer::is_package_id(const std::string& maybe_package_id) {
  bool parsed_slash = false;
  for (char c : maybe_package_id) {
    if (c == '/') {
      if (parsed_slash)
        return false;
      parsed_slash = true;
    }
  }
  return parsed_slash;
}

std::string packer::Locator::id() {
  return group + "/" + name;
}
