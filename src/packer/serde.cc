#include <packer/serde.hh>
#include <packer/util.hh>
#include <ostream>
#include <cstring>
#include <cstdio>

void packer::operator<<(YAML::Node& out, const packer::Packerfile& packerfile) {
  out["version"] = packerfile.version;
  out["license"] = packerfile.license;
  out["summary"] = packerfile.summary;
  out << packerfile.baseline;
}

inline bool parse_string(const YAML::Node& in, std::string& out, const std::string& keyword, bool or_null = false) {
  bool ok = true;
  out.clear();
  if (in[keyword].IsDefined()) {
    if (in[keyword].IsScalar()) {
      out = packer::trim_string(in[keyword].as<std::string>());
    } else {
      ok = false;
    }
  } else {
    ok = or_null;
  }
  return ok;
}

bool packer::operator>>(const YAML::Node& in, packer::Packerfile& packerfile) {
  bool ok = true;
  if (!in.IsMap()) {
    ok = false;
  } else {
    ok &= parse_string(in, packerfile.version, "version");
    ok &= parse_string(in, packerfile.license, "license");
    ok &= parse_string(in, packerfile.summary, "summary");
    ok &= (in >> packerfile.baseline);
    if (in["patches"].IsDefined()) {
      if (in["patches"].IsMap()) {
        packerfile.patches.clear();
        for (YAML::const_iterator it = in["patches"].begin(); it != in["patches"].end(); ++it) {
          if (!it->first.IsScalar()) {
            ok = false;
            continue;
          }
          std::string key = it->first.as<std::string>();
          Patchable patch;
          ok &= (it->second >> patch);
          if (ok)
            packerfile.patches[key] = std::move(patch);
        }
      } else {
        ok = false;
      }
    }
  }

  return ok;
}

void packer::operator<<(YAML::Node& out, const packer::Patchable& patchable) {
  out["build"] = patchable.build_script;
  out["install"] = patchable.install_script;
}

constexpr const char* PACKERFILE_DEPENDENCIES_DELIMITERS = ", ";
inline bool parse_vector_of_dependencies(const YAML::Node& in, std::vector<std::string>& out, const std::string& keyword, bool or_null = false) {
  bool ok = true;
  out.clear();
  if (in[keyword].IsDefined()) {
    if (in[keyword].IsScalar()) {
      std::string buffer = in[keyword].as<std::string>();
      const char* token = std::strtok(buffer.data(), PACKERFILE_DEPENDENCIES_DELIMITERS);
      while (token != nullptr) {
        if (strlen(token) > 0) {
          packer::add_if_not_present(out, std::string(token));
        }
        token = std::strtok(nullptr, PACKERFILE_DEPENDENCIES_DELIMITERS);
      }
    } else if (in[keyword].IsSequence()) {
      for (const YAML::Node& token : in[keyword]) {
        packer::add_if_not_present(out, token.as<std::string>());
      }
    } else {
      ok = false;
    }
  } else {
    ok = or_null;
  }
  return ok;
}

bool packer::operator>>(const YAML::Node& in, packer::Patchable& patchable) {
  bool ok = true;

  if (!in.IsMap()) {
    ok = false;
  } else {
    ok &= parse_string(in, patchable.build_script, "build", true);
    ok &= parse_string(in, patchable.install_script, "install", true);
    ok &= parse_vector_of_dependencies(in, patchable.depends, "depends", true); 
    ok &= parse_vector_of_dependencies(in, patchable.makedepends, "makedepends", true); 
  }
  
  return ok;
}

std::ostream& packer::operator<<(std::ostream& out, const packer::Packerfile& packerfile) {
  out << packerfile.locator << std::endl;
  out << "version: " << packerfile.version << std::endl;
  out << "license: " << packerfile.license << std::endl;
  out << "summary: " << packerfile.summary << std::endl;
  out << "baseline:" << std::endl;
  out << packerfile.baseline;
  for (const std::pair<std::string, Patchable> patch : packerfile.patches) {
    out << std::endl << patch.first << ":" << std::endl;
    out << patch.second;
  }
  return out;
}

std::ostream& packer::operator<<(std::ostream& out, const packer::Patchable& patchable) {
  out << "  build_script: | " << patchable.build_script << std::endl;
  out << "  install_script: | " << patchable.install_script;
  if (patchable.depends.size() > 0) {
    out << std::endl << "  depends: |";
    for (const std::string& item : patchable.depends) {
      out << " " << item;
    }
  }
  if (patchable.makedepends.size() > 0) {
    out << std::endl << "  makedepends: |";
    for (const std::string& item : patchable.makedepends) {
      out << " " << item;
    }
  }
  return out;
}

std::ostream& packer::operator<<(std::ostream& out, const packer::Host& host) {
  out << "distro: " << host.distro << std::endl;
  out << "package_manager: " << std::endl;
  switch (host.package_manager) {
    case packer::DNF: out << DNF_PATH << std::endl; break;
    case packer::YUM: out << YUM_PATH << std::endl; break;
    case packer::YAY: out << YAY_PATH << std::endl; break;
    case packer::PACMAN: out << PACMAN_PATH << std::endl; break;
  }
  out << "packaging: ";
  switch (host.packaging) {
    case packer::RPMBUILD: out << RPMBUILD_PATH << std::endl; break;
    case packer::MAKEPKG: out << MAKEPKG_PATH << std::endl; break;
  }
  out << "homedir: " << host.homedir;
  return out;
}

std::ostream& packer::operator<<(std::ostream& out, const Locator& locator) {
  out << "group: " << locator.group << std::endl;
  out << "name: " << locator.name << std::endl;
  out << "url: " << locator.url << std::endl;
  out << "path: " << locator.path << std::endl;
  return out;
}

std::ostream& packer::operator<<(std::ostream& out, const Package& package) {
  out << package.locator << std::endl;
  out << "version: " << package.version << std::endl;
  out << "license: " << package.license << std::endl;
  out << "summary: " << package.summary << std::endl;

  out << "build_system_deps:";
  for (std::string requirement : package.build_system_requirements) {
    out << " " << requirement;
  }
  out << std::endl;
  
  out << "install_system_deps:";
  for (std::string requirement : package.install_system_requirements) {
    out << " " << requirement;
  }
  out << std::endl;

  out << "build_packer_deps:";
  for (packer::Package* requirement : package.build_packer_requirements) {
    out << " " << requirement;
  }
  out << std::endl;

  out << "install_packer_deps:";
  for (packer::Package* requirement : package.install_packer_requirements) {
    out << " " << requirement;
  }
  out << std::endl;

  out << "build: | " << package.build_script << std::endl;
  out << "install: | " << package.install_script;
  return out;
}
