#include <packer/serde.hh>
#include <ostream>
#include <cstring>

void packer::operator<<(YAML::Node& out, const packer::Packerfile& packerfile) {
  out["version"] = packerfile.version;
  out["license"] = packerfile.license;
  out["summary"] = packerfile.summary;
  out << packerfile.baseline;
}

bool packer::operator>>(const YAML::Node& in, packer::Packerfile& packerfile) {
  bool ok = true;
  if (!in.IsMap()) {
    ok = false;
  } else {
    if (in["version"].IsScalar() && in["license"].IsScalar() && in["summary"].IsScalar()) {
      packerfile.version = in["version"].as<std::string>();
      packerfile.license = in["license"].as<std::string>();
      packerfile.summary = in["summary"].as<std::string>();
    } else {
      ok = false;
    }
    ok &= (in >> packerfile.baseline);
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
    } else if (in["patches"].IsDefined()) {
      ok = false;
    }
  }

  return ok;
}

void packer::operator<<(YAML::Node& out, const packer::Patchable& patchable) {
  out["build"] = patchable.build_script;
  out["install"] = patchable.install_script;
}

inline bool parse_vector_of_dependencies(const YAML::Node& in, std::vector<std::string>& out, const std::string& keyword) {
  bool ok = true;
  if (in[keyword].IsScalar()) {
    out.clear();
    std::string buffer = in[keyword].as<std::string>();
    const char* token = std::strtok(buffer.data(), " ,");
    while (token != nullptr) {
      if (strlen(token) > 0) {
        out.push_back(token);
      }
      token = std::strtok(nullptr, " ,");
    }
  }
  else if (in[keyword].IsSequence()) {
    for (const YAML::Node& token : in[keyword]) {
      out.push_back(token.as<std::string>());
    }
    out.clear();
  } else if (in[keyword].IsDefined()) {
    ok = false;
  }
  return ok;
}

bool packer::operator>>(const YAML::Node& in, packer::Patchable& patchable) {
  bool ok = true;

  if (!in.IsMap()) {
    ok = false;
  } else {
    if (in["build"].IsScalar()) {
      patchable.build_script = in["build"].as<std::string>();
    } else if (in["build"].IsDefined()) {
      ok = false;
    }
    if (in["install"].IsScalar()) {
      patchable.install_script = in["install"].as<std::string>();
    } else if (in["install"].IsDefined()) {
      ok = false;
    }
    ok &= parse_vector_of_dependencies(in, patchable.depends, "depends"); 
    ok &= parse_vector_of_dependencies(in, patchable.makedepends, "makedepends"); 
  }
  
  return ok;
}

std::ostream& packer::operator<<(std::ostream& out, const packer::Packerfile& packerfile) {
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
  out << "  build_script: " << patchable.build_script << std::endl;
  out << "  install_script: " << patchable.install_script;
  if (patchable.depends.size() > 0) {
    out << std::endl << "depends:";
    for (const std::string& item : patchable.depends) {
      out << " " << item;
    }
  }
  if (patchable.makedepends.size() > 0) {
    out << std::endl << "makedepends:";
    for (const std::string& item : patchable.makedepends) {
      out << " " << item;
    }
  }
  return out;
}
