#include <packer/scheduling.hh>
#include <packer/serde.hh>
#include <packer/logging.hh>
#include <packer/io.hh>

bool schedule_package(std::unordered_map<const packer::Package*, int64_t>& weights, packer::Package* package);

bool schedule_dependencies(std::unordered_map<const packer::Package*, int64_t>& weights, packer::Package* package, int64_t& weight, const std::vector<packer::Package*>& dependencies) {
  for (packer::Package* dependency : dependencies) {
    if (!schedule_package(weights, dependency)) {
      packer::throw_warning(MSG("couldn't compute weight of dependency '" << dependency->locator.id() << "' of '" << package->locator.id() << "'"));
      return false;
    }
    int64_t dependency_weight = weights.at(dependency);
    if (dependency_weight < 0) {
      packer::throw_warning(MSG("probable conflict with dependency '" << dependency->locator.id() << "' of '" << package->locator.id() << "' since schedule weight computation leads to recursive behavior"));
      return false;
    }
    weight += dependency_weight;
  }
  return true;
}

bool schedule_package(std::unordered_map<const packer::Package*, int64_t>& weights, packer::Package* package) {
  auto weight_it = weights.find(package);
  if (weight_it != weights.end()) {
    return true;
  }

  weights[package] = -1;
  int64_t weight = 0;
  if (!schedule_dependencies(weights, package, weight, package->build_packer_requirements)) {
    packer::throw_warning(MSG("couldn't process build dependencies of '" << package->locator.id() << "'"));
    return false;
  }
  if (!schedule_dependencies(weights, package, weight, package->install_packer_requirements)) {
    packer::throw_warning(MSG("couldn't process install dependencies of '" << package->locator.id() << "'"));
    return false;
  }
  weights[package] = weight;
  return true;
}

bool packer::schedule_packages(std::vector<packer::Package*>& schedule, const std::unordered_map<std::string, packer::Package*>& packages) {
  std::unordered_map<const packer::Package*, int64_t> weights;
  schedule.clear();
  for (std::pair<std::string, packer::Package*> package_it : packages) {
    if (!schedule_package(weights, package_it.second)) {
      packer::throw_warning(MSG("couldn't schedule '" << package_it.second << "'"));
      return false;
    }
    schedule.push_back(package_it.second);
  }
  std::sort(schedule.begin(), schedule.end(), [&weights](const packer::Package* A, const packer::Package* B){
    return weights[A] >= weights[B];
  });
  return true;
}
