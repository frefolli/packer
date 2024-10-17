#include "packer/util.hh"
#include <packer/packerfile.hh>
#include <packer/serde.hh>
#include <packer/io.hh>
#include <packer/logging.hh>
#include <packer/host.hh>
#include <packer/scheduling.hh>
#include <packer/analysis.hh>
#include <packer/package_manager.hh>
#include <packer/packaging.hh>
#include <yaml-cpp/yaml.h>
#include <cassert>
#include <iostream>

struct CLI {
  bool refresh = false;
  bool install = false;
  bool help = false;
  std::string repository = "./repository";
};

int main(int argc, char** args) {
  packer::Host host;
  if (!packer::probe_host(host)) {
    packer::raise_error(1, MSG("couldn't probe host for info"));
  }
  std::cout << "# Host Info" << std::endl;
  packer::operator<<(std::cout, host) << std::endl;

  std::unordered_map<std::string, packer::Packerfile> packerfiles;
  for (int argi = 1; argi < argc; ++argi) {
    std::string package_id = args[argi];
    packer::Packerfile packerfile;
    if (!packer::load_from_package_id(host, packerfile, package_id)) {
      packer::raise_error(1, MSG("unable to load Packerfile for '" << package_id << "' package_id"));
    }
    packerfiles[package_id] = packerfile;
  }

  std::unordered_map<std::string, packer::Package*> package_index;
  if (!packer::analyze_packages(host, package_index, packerfiles)) {
    packer::raise_error(1, MSG("unable to analyze packages"));
  }

  std::vector<packer::Package*> schedule;
  if (!packer::schedule_packages(schedule,package_index)) {
    packer::raise_error(1, MSG("unable to schedule packages"));
  }

  for (packer::Package* package : schedule) {
    std::vector<std::string> requirements = package->build_system_requirements;
    packer::merge_vectors(requirements, package->install_system_requirements);
    if (!packer::filter_installed_packages(host.package_manager, requirements)) {
      packer::raise_error(1, MSG("unable to compute dependencies to install prior to build of '" << package->locator.id() << "' package"));
    }
    if (!packer::install_system_dependencies(host.package_manager, requirements)) {
      packer::raise_error(1, MSG("unable to install dependencies prior to build of '" << package->locator.id() << "' package"));
    }
    std::optional<std::string> package_path = packer::craft_package(host.homedir, host.packaging, package);
    if (!package_path.has_value()) {
      packer::raise_error(1, MSG("unable to craft package file for '" << package->locator.id() << "' package"));
    }
    if (!packer::install_packer_package(host.package_manager, package_path.value())) {
      packer::raise_error(1, MSG("unable to install crafted package file for '" << package->locator.id() << "' package"));
    }
  }

  for (std::pair<std::string, packer::Package*> package_it :package_index) {
    delete package_it.second;
  }
}
