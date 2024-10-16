#include <iostream>
#include <packer/packerfile.hh>
#include <packer/serde.hh>
#include <packer/io.hh>
#include <packer/logging.hh>
#include <yaml-cpp/yaml.h>
#include <cassert>

int main(int argc, char** args) {
  for (int argi = 1; argi < argc; ++argi) {
    std::string filepath = args[argi];
    packer::Packerfile packerfile;
    if (!packer::load_from_file(packerfile, filepath)) {
      packer::raise_error(1, MSG("file '" << filepath << "' cannot be read as Packerfile"));
    }
    packer::operator<<(std::cout, packerfile);
  }
}
