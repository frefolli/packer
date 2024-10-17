#include <packer/packerfile.hh>
#include <packer/serde.hh>
#include <packer/io.hh>
#include <packer/logging.hh>
#include <packer/host.hh>
#include <packer/scheduling.hh>
#include <packer/analysis.hh>
#include <yaml-cpp/yaml.h>
#include <cassert>
#include <iostream>

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
    std::clog << "#################################################" << std::endl;
    packer::operator<<(std::clog, *package) << std::endl;
  }

  for (std::pair<std::string, packer::Package*> package_it :package_index) {
    delete package_it.second;
  }
}
