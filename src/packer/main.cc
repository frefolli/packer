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
#include <cstring>

struct CLI {
  bool refresh = false;
  bool install = false;
  std::string repository = "./repository";
  std::vector<std::string> targets;
};

void print_help() {
  std::cout << "Usage: packer [options] target..." << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -r/--refresh              " << std::endl;
  std::cout << "  -i/--install              " << std::endl;
  std::cout << "  -R/--repository           " << std::endl;
  std::cout << "  -h/--help                 Display this information." << std::endl;
  exit(0);
}

bool parse_cli(CLI& cli, int argc, char** args) {
  for (int argi = 1; argi < argc; ++argi) {
    std::string arg = args[argi];
    if (arg[0] == '-') {
      if (arg.size() > 1) {
        if (arg[1] == '-') {
          // long flag
          std::string long_flag = arg.substr(2);
          if (long_flag == "help") {
            print_help();
            packer::unreachable();
          } else if (long_flag == "refresh") {
            cli.refresh = true;
          } else if (long_flag == "install") {
            cli.install = true;
          } else if (long_flag == "repository") {
            if (argi + 1 >= argc) {
              packer::throw_warning(MSG("flag '" << arg << "' expects an argument"));
            }
            cli.repository = args[++argi];
          } else {
            packer::throw_warning(MSG("unknown flag '" << arg << "'"));
            return false;
          }
        } else {
          // stacked short flags
          for (char c : arg.substr(1)) {
            if (c == 'h') {
              print_help();
              packer::unreachable();
            } else if (c == 'r') {
              cli.refresh = true;
            } else if (c == 'i') {
              cli.install = true;
            } else if (c == 'R') {
              if (argi + 1 >= argc) {
                packer::throw_warning(MSG("flag '-" << c << "' expects an argument"));
              }
              cli.repository = args[++argi];
            }  else {
              packer::throw_warning(MSG("unknown flag '-" << c << "'"));
              return false;
            }
          }
        }
      } else {
        return true;
      }
    } else {
      cli.targets.push_back(arg);
    }
  }
  return true;
}

int main(int argc, char** args) {
  CLI cli;
  if (!parse_cli(cli, argc, args)) {
    packer::raise_error(1, MSG("error while parsing arguments"));
  }

  packer::Host host;
  if (!packer::probe_host(host)) {
    packer::raise_error(1, MSG("couldn't probe host for info"));
  }
  std::cout << "# Host Info" << std::endl;
  packer::operator<<(std::cout, host) << std::endl;

  std::unordered_map<std::string, packer::Packerfile> packerfiles;
  for (std::string package_id : cli.targets) {
    packer::Packerfile packerfile;
    if (!packer::load_from_package_id(host, packerfile, package_id)) {
      packer::raise_error(1, MSG("unable to load Packerfile for '" << package_id << "' package_id"));
    }
    packerfiles[package_id] = packerfile;
  }

  std::unordered_map<std::string, packer::Package*> package_index;
  if (!packer::analyze_packages(host, package_index, packerfiles, cli.refresh, cli.install)) {
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
    if (package->install_after_build) {
      if (!packer::install_packer_package(host.package_manager, package_path.value())) {
        packer::raise_error(1, MSG("unable to install crafted package file for '" << package->locator.id() << "' package"));
      }
    }
  }

  for (std::pair<std::string, packer::Package*> package_it :package_index) {
    delete package_it.second;
  }
}
