#include <packer/serde.hh>
#include <ostream>

void packer::operator<<(YAML::Node& out, const packer::Packerfile& packerfile) {
  out["version"] = packerfile.version;
  out["license"] = packerfile.license;
  out["summary"] = packerfile.summary;
  out << packerfile.baseline;
}

bool packer::operator>>(const YAML::Node& in, packer::Packerfile& packerfile) {
  packerfile.version = in["version"].as<std::string>();
  packerfile.license = in["license"].as<std::string>();
  packerfile.summary = in["summary"].as<std::string>();
  in >> packerfile.baseline;

  return true;
}

void packer::operator<<(YAML::Node& out, const packer::Patchable& patchable) {
  out["build"] = patchable.build_script;
  out["install"] = patchable.install_script;
}

bool packer::operator>>(const YAML::Node& in, packer::Patchable& patchable) {
  patchable.build_script = in["build"].as<std::string>();
  patchable.install_script = in["install"].as<std::string>();
  
  return true;
}

std::ostream& packer::operator<<(std::ostream& out, const packer::Packerfile& packerfile) {
  out << "version: " << packerfile.version << std::endl;
  out << "license: " << packerfile.license << std::endl;
  out << "summary: " << packerfile.summary << std::endl;
  out << "baseline:" << std::endl;
  out << packerfile.baseline;
  return out;
}

std::ostream& packer::operator<<(std::ostream& out, const packer::Patchable& patchable) {
  out << "  build_script: " << patchable.build_script << std::endl;
  out << "  install_script: " << patchable.install_script;
  return out;
}
