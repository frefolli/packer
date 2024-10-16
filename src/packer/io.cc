#include <packer/io.hh>
#include <packer/serde.hh>
#include <packer/logging.hh>
#include <filesystem>
#include <fstream>

bool packer::load_from_file(Packerfile& packerfile, const std::string& filepath) {
  if (!std::filesystem::exists(filepath)) {
    packer::throw_warning(MSG("file '" << filepath << "' doesn't exist"));
    return false;
  }
  YAML::Node yaml_file = YAML::LoadFile(filepath);
  if (!packer::operator>>(yaml_file, packerfile)) {
    packer::throw_warning(MSG("file '" << filepath << "' has invalid format"));
    return false;
  }
  return true;
}

bool packer::dump_into_file(const Packerfile& packerfile, const std::string& filepath) {
  YAML::Node node;
  packer::operator<<(node, packerfile);
  std::ofstream file (filepath);
  if (file.bad()) {
    packer::throw_warning(MSG("file '" << filepath << "' cannot be opened for writing"));
    return false;
  }
  file << YAML::Dump(node);
  file.close();
  return true;
}
