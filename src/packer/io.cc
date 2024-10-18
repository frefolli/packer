#include <cstring>
#include <packer/io.hh>
#include <packer/serde.hh>
#include <packer/logging.hh>
#include <filesystem>
#include <fstream>

std::string packer::REPOSITORY_DIRECTORY = "./repository";
std::string packer::URL_BASE = "https://github.com";

bool packer::load_from_file(Packerfile& packerfile, const std::string& filepath) {
  if (!std::filesystem::exists(filepath)) {
    packer::throw_warning(MSG("file '" << filepath << "' doesn't exist"));
    return false;
  }
  YAML::Node yaml_file = YAML::LoadFile(filepath);
  if (!packer::operator>>(yaml_file, packerfile)) {
    packer::throw_warning(MSG("file '" << filepath << "' is ill-formed"));
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

constexpr const char* PACKAGE_ID_DELIMITERS = "/";
std::optional<packer::Locator> packer::parse_locator(const std::string& package_id) {
  packer::Locator locator;
  std::string buffer = package_id;
  
  const char* token = std::strtok(buffer.data(), PACKAGE_ID_DELIMITERS);
  if (token == nullptr)
    return std::nullopt;
  locator.group = token;
  
  token = std::strtok(nullptr, PACKAGE_ID_DELIMITERS);
  if (token == nullptr)
    return std::nullopt;
  locator.name = token;

  token = std::strtok(nullptr, PACKAGE_ID_DELIMITERS);
  if (token != nullptr)
    return std::nullopt;

  locator.url = MSG(URL_BASE << locator.id()).str();
  std::string dir = MSG(REPOSITORY_DIRECTORY  << "/" << locator.group).str();
  std::filesystem::create_directories(dir);
  locator.path = MSG(dir << "/" << locator.name).str();
  return locator;
}

bool packer::load_from_package_id(const Host& host, Packerfile& packerfile, const std::string& package_id) {
  std::optional<packer::Locator> maybe_locator = packer::parse_locator(package_id);
  if (!maybe_locator.has_value()) {
    packer::throw_warning(MSG("input '" << package_id << "' isn't a package_id"));
    return false;
  }
  packer::Locator locator = maybe_locator.value();
  if (!packer::load_from_file(packerfile, locator.path)) {
    packer::throw_warning(MSG("file '" << locator.path << "' cannot be read as Packerfile"));
    return false;
  }
  packerfile.locator = locator;
  packer::patch(host, packerfile);
  return true;
}
