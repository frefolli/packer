#include <packer/scheduling.hh>
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

bool schedule_package(std::unordered_map<packer::Package*, int64_t> weights, packer::Package* package);

bool schedule_dependencies(std::unordered_map<packer::Package*, int64_t> weights, packer::Package* package, int64_t& weight, const std::vector<packer::Package*>& dependencies) {
  for (packer::Package* dependency : dependencies) {
    if (!schedule_package(weights, dependency)) {
      packer::throw_warning(MSG("couldn't compute weight of dependency '" << dependency << "' of '" << package << "'"));
      return false;
    }
    int64_t dependency_weight = weights.at(dependency);
    if (dependency_weight < 0) {
      packer::throw_warning(MSG("probable conflict with dependency '" << dependency << "' of '" << package << "' since schedule weight computation leads to recursive behavior"));
      return false;
    }
    weight += dependency_weight;
  }
  return true;
}

bool schedule_package(std::unordered_map<packer::Package*, int64_t> weights, packer::Package* package) {
  auto weight_it = weights.find(package);
  if (weight_it != weights.end()) {
    return true;
  }

  weights[package] = -1;
  int64_t weight = 0;
  if (!schedule_dependencies(weights, package, weight, package->build_packer_requirements)) {
    packer::throw_warning(MSG("couldn't process build dependencies of '" << package << "'"));
    packer::throw_warning(MSG(""));
    return false;
  }
  if (!schedule_dependencies(weights, package, weight, package->install_packer_requirements)) {
    packer::throw_warning(MSG("couldn't process install dependencies of '" << package << "'"));
    return false;
  }
  weights[package] = weight;
  return true;
}

bool packer::schedule_packages(std::vector<packer::Package*>& schedule,
    const std::unordered_map<std::string, packer::Package*>& packages) {
  std::unordered_map<packer::Package*, int64_t> weights;
  schedule.clear();
  for (std::pair<std::string, packer::Package*> package_it : packages) {
    if (!schedule_package(weights, package_it.second)) {
      packer::throw_warning(MSG("couldn't schedule '" << package_it.second << "'"));
    }
  }
  return true;
}
