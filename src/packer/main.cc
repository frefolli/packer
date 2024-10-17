#include <packer/packerfile.hh>
#include <packer/serde.hh>
#include <packer/io.hh>
#include <packer/logging.hh>
#include <packer/specialization.hh>
#include <packer/scheduling.hh>
#include <yaml-cpp/yaml.h>
#include <cassert>
#include <iostream>

int main(int argc, char** args) {
  packer::Host host;
  if (!packer::probe_host(host)) {
    packer::raise_error(1, MSG("couldn't probe host for info"));
  }
  std::cout << "# Host Info" << std::endl;
  packer::operator<<(std::cout, host) << std::endl;

  std::unordered_map<std::string, packer::Packerfile> packerfiles;
  for (int argi = 1; argi < argc; ++argi) {
    std::string package_id = args[argi];
    packer::Packerfile packerfile;
    if (!packer::load_from_package_id(packerfile, package_id)) {
      packer::raise_error(1, MSG("unable to load Packerfile for '" << package_id << "' package_id"));
    }
    packer::patch(host, packerfile);
    packerfiles[package_id] = packerfile;
  }

  std::unordered_map<std::string, packer::Package*> packages;
  if (!packer::schedule_packages(packages, packerfiles)) {
    packer::raise_error(1, MSG("unable to schedule packages"));
  }
}
