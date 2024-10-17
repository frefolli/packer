#include <packer/analysis.hh>
#include <packer/serde.hh>
#include <packer/logging.hh>
#include <packer/io.hh>

bool analyze_package(const packer::Host& host,
    std::unordered_map<std::string, packer::Package*>& packages,
    std::unordered_map<std::string, packer::Packerfile>& packerfile_index,
    const std::string& package_id);

bool analyze_dependencies(const packer::Host& host,
    std::unordered_map<std::string, packer::Package*>& packages,
    std::unordered_map<std::string, packer::Packerfile>& packerfile_index,
    const std::string& package_id,
    const std::vector<std::string>& packerfile_dependencies,
    std::vector<std::string>& system_requirements,
    std::vector<packer::Package*>& packer_requirements) {
  for (std::string dependency : packerfile_dependencies) {
    if (packer::is_package_id(dependency)) {
      if (packerfile_index.find(dependency) == packerfile_index.end()) {
        packer::Packerfile dependency_packerfile;
        if (!packer::load_from_package_id(host, dependency_packerfile, dependency)) {
          packer::throw_warning(MSG("Packerfile of dependency'" << dependency << "' of '" << package_id << "' couldn't be loaded"));
          return false;
        }
        packerfile_index[dependency] = dependency_packerfile;
        if (!analyze_package(host, packages, packerfile_index, dependency)) {
          packer::throw_warning(MSG("Dependency '" << dependency << "' of '" << package_id << "' couldn't be analyze"));
          return false;
        }
      } else {
        auto dependency_it = packages.find(dependency);
        if (dependency_it == packages.end()) {
          packer::throw_warning(MSG("Dependency '" << dependency << "' of '" << package_id << "' may be part of a cycle"));
          return false;
        }
      }
      packer_requirements.push_back(packages.at(dependency));
    } else {
      system_requirements.push_back(dependency);
    }
  }
  return true;
}

bool analyze_package(const packer::Host& host,
    std::unordered_map<std::string, packer::Package*>& packages,
    std::unordered_map<std::string, packer::Packerfile>& packerfile_index,
    const std::string& package_id) {
  auto package_it = packages.find(package_id);
  if (package_it != packages.end()) {
    return true;
  }

  packer::Packerfile packerfile = packerfile_index.at(package_id);
  packer::Package* package = new packer::Package();
  package->locator = packerfile.locator;
  package->version = packerfile.version;
  package->license = packerfile.license;
  package->summary = packerfile.summary;

  if (!analyze_dependencies(host,
        packages,
        packerfile_index,
        package_id,
        packerfile.baseline.makedepends,
        package->build_system_requirements,
        package->build_packer_requirements)) {
    packer::throw_warning(MSG("couldn't analyze build dependencies '" << package_id << "'"));
    return false;
  }
  if (!analyze_dependencies(host,
        packages,
        packerfile_index,
        package_id,
        packerfile.baseline.depends,
        package->install_system_requirements,
        package->install_packer_requirements)) {
    packer::throw_warning(MSG("couldn't analyze install dependencies '" << package_id << "'"));
    return false;
  }

  packages[package_id] = package;
  return true;
}

bool packer::analyze_packages(const packer::Host& host,
    std::unordered_map<std::string, Package*>& packages,
    std::unordered_map<std::string, Packerfile>& packerfile_index) {
  bool ok = true;
  packages.clear();
  std::vector<std::string> scheduling_queue = {};
  for (std::pair<std::string, packer::Packerfile> packerfile_it : packerfile_index) {
    scheduling_queue.push_back(packerfile_it.first);
  }
  while (!scheduling_queue.empty()) {
    std::string package_id = scheduling_queue.back();
    scheduling_queue.pop_back();
    if (!analyze_package(host, packages, packerfile_index, package_id)) {
      packer::throw_warning(MSG("packerfile '" << package_id <<  "' couldn't be analyzed"));
      ok = false;
      break;
    }
  }
  return ok;
}
