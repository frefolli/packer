#include <iostream>
#include <packer/packerfile.hh>
#include <packer/serde.hh>
#include <yaml-cpp/yaml.h>
#include <cassert>

int main(int argc, char** args) {
  std::string filepath = "Packerfile";
  if (argc > 1) {
    filepath = args[1];
  }
  
  packer::Packerfile packerfile;
  YAML::Node yaml_file = YAML::LoadFile(filepath);
  assert(packer::operator>>(yaml_file, packerfile));
  packer::operator<<(std::cout, packerfile);
}
