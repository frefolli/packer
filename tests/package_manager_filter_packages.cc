#include <packer/package_manager.hh>
#include <packer/host.hh>
#include <cassert>
#include <packer/serde.hh>
#include <iostream>

int main() {
  packer::Host host;
  assert(packer::probe_host(host));
  packer::operator<<(std::clog, host) << std::endl;

  std::vector<std::string> system_packages = {"Villanterio", "meson"};
  packer::filter_installed_packages(host.package_manager, system_packages);
  std::vector<std::string> expected_packages = {"Villanterio"};
  assert(system_packages == expected_packages);
}
