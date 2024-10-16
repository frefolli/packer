#include <iostream>
#include <packer/packerfile.hh>
#include <packer/serde.hh>
#include <packer/io.hh>
#include <packer/logging.hh>
#include <packer/specialization.hh>
#include <yaml-cpp/yaml.h>
#include <cassert>

int main(int argc, char** args) {
  packer::Host host;
  if (!packer::probe_host(host)) {
    packer::raise_error(1, MSG("couldn't probe host for info"));
  }
  std::cout << "# Host Info" << std::endl;
  packer::operator<<(std::cout, host) << std::endl;

  for (int argi = 1; argi < argc; ++argi) {
    std::string filepath = args[argi];
    packer::Packerfile packerfile;
    if (!packer::load_from_file(packerfile, filepath)) {
      packer::raise_error(1, MSG("file '" << filepath << "' cannot be read as Packerfile"));
    }
    std::cout << "# Packerfile '" << filepath << "'" << std::endl;
    packer::operator<<(std::cout, packerfile) << std::endl;
  }
}
