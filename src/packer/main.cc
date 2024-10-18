#include <packer/util.hh>
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
#include <filesystem>

struct CLI {
  bool refresh;
  bool install;
  bool pretend;
  bool noconfirm;
  bool verbose;
  std::string repository;
  std::string hub;
  std::vector<std::string> targets;
};

void print_help(char* progname) {
  std::cout << "Usage: " << progname << " [options] target..." << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -r/--refresh              Re-generate source/spec/package files even if they exist." << std::endl;
  std::cout << "  -i/--install              If not issued, packer packages which are not dependencies of other packages won't get automatically installed." << std::endl;
  std::cout << "  -p/--pretend              Don't actually do stuff, only pretend to do that." << std::endl;
  std::cout << "  -n/--noconfirm            Don't ask for confirmation." << std::endl;
  std::cout << "  -v/--verbose              Print extra stuff." << std::endl;
  std::cout << "  -R/--repository           Sets the local repository path (default is ~/.packer/repository)." << std::endl;
  std::cout << "  -H/--hub                  Sets the remote Hub (default is 'https://github.com')." << std::endl;
  std::cout << "  -h/--help                 Display this information." << std::endl;
  exit(0);
}

bool parse_cli(CLI& cli, const packer::Host& host, int argc, char** args) {
  cli.refresh = false;
  cli.install = false;
  cli.pretend = false;
  cli.noconfirm = false;
  cli.verbose = false;
  cli.repository = MSG(host.homedir << "/.packer/repository").str();
  cli.hub = "https://github.com";
  cli.targets.clear();

  for (int argi = 1; argi < argc; ++argi) {
    std::string arg = args[argi];
    if (arg[0] == '-') {
      if (arg.size() > 1) {
        if (arg[1] == '-') {
          std::string long_flag = arg.substr(2);
          if (long_flag == "help") {
            print_help(args[0]);
            packer::unreachable();
          } else if (long_flag == "refresh") {
            cli.refresh = true;
          } else if (long_flag == "install") {
            cli.install = true;
          } else if (long_flag == "pretend") {
            cli.pretend = true;
          } else if (long_flag == "noconfirm") {
            cli.noconfirm = true;
          } else if (long_flag == "verbose") {
            cli.verbose = true;
          } else if (long_flag == "repository") {
            if (argi + 1 >= argc) {
              packer::throw_warning(MSG("flag '" << arg << "' expects an argument"));
            }
            cli.repository = args[++argi];
          } else if (long_flag == "hub") {
            if (argi + 1 >= argc) {
              packer::throw_warning(MSG("flag '" << arg << "' expects an argument"));
            }
            cli.hub = args[++argi];
          } else {
            packer::throw_warning(MSG("unknown flag '" << arg << "'"));
            return false;
          }
        } else {
          for (char c : arg.substr(1)) {
            if (c == 'h') {
              print_help(args[0]);
              packer::unreachable();
            } else if (c == 'r') {
              cli.refresh = true;
            } else if (c == 'i') {
              cli.install = true;
            } else if (c == 'p') {
              cli.pretend = true;
            } else if (c == 'n') {
              cli.noconfirm = true;
            } else if (c == 'v') {
              cli.verbose = true;
            } else if (c == 'R') {
              if (argi + 1 >= argc) {
                packer::throw_warning(MSG("flag '-" << c << "' expects an argument"));
              }
              cli.repository = args[++argi];
            } else if (c == 'H') {
              if (argi + 1 >= argc) {
                packer::throw_warning(MSG("flag '-" << c << "' expects an argument"));
              }
              cli.hub = args[++argi];
            }  else {
              packer::throw_warning(MSG("unknown flag '-" << c << "'"));
              return false;
            }
          }
        }
      } else {
        packer::throw_warning(MSG("unknown flag '" << arg << "'"));
        return false;
      }
    } else {
      cli.targets.push_back(arg);
    }
  }
  return true;
}

std::string left_justify(const std::string& str, uint64_t width, char filling) {
  uint64_t length = str.size();
  assert(length < width);
  if (length < width) {
    return str + std::string(width - length, filling);
  }
  return str;
}

void preview_packages(const std::vector<packer::Package*>& schedule) {
  uint64_t max_id_length = 0;
  for (packer::Package* package : schedule) {
    max_id_length = std::max(package->locator.id().size(), max_id_length);
  }
  std::cout << left_justify(MSG("Package (" << schedule.size() << ")").str(), max_id_length + 2, ' ') << "New Version" << std::endl << std::endl;
  for (packer::Package* package : schedule) {
    std::cout << " " << left_justify(package->locator.id(), max_id_length + 2, ' ') << package->version << std::endl;
  }
}

bool ask_yn(const std::string& msg) {
  std::string reply;
  std::cout << std::endl << ":: " << msg << " [Y/n]: " << std::flush;
  std::cin >> reply;
  while (true) {
    for (uint64_t i = 0; i < reply.size(); ++i) {
      if (reply[i] >= 'A' && reply[i] <= 'Z') {
        reply[i] += ('a' - 'A');
      }
    }
    if (reply.empty()) {
      return true;
    } else if (reply == "yes" || reply == "y") {
      return true;
    } else if (reply == "no" || reply == "n") {
      return false;
    } else {
      std::cout << ":: '" << reply << "' isn't a valid reply" << std::endl;
      std::cout << ":: You should reply [Y/n]: " << std::flush;
      std::cin >> reply;
    }
  }
  packer::unreachable();
}

int main(int argc, char** args) {
  packer::Host host;
  if (!packer::probe_host(host)) {
    packer::raise_error(1, MSG("couldn't probe host for info"));
  }

  CLI cli;
  if (!parse_cli(cli, host, argc, args)) {
    packer::raise_error(1, MSG("error while parsing arguments"));
  }
  packer::REPOSITORY_DIRECTORY = cli.repository;
  packer::URL_BASE = cli.hub;
  if (!std::filesystem::exists(packer::REPOSITORY_DIRECTORY)) {
    if (!std::filesystem::create_directories(packer::REPOSITORY_DIRECTORY)) {
      packer::raise_error(1, MSG("unable to create packer repository directory '" << packer::REPOSITORY_DIRECTORY << "'"));
    }
  }

  if (cli.verbose) {
    std::cout << "# Host Info" << std::endl;
    packer::operator<<(std::cout, host) << std::endl;
  }
  
  if (cli.targets.empty()) {
    packer::raise_error(1, MSG("no input file supplied"));
  }

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

  preview_packages(schedule);
  if (cli.noconfirm || ask_yn("Proceed with build/installation?")) {
    if (!cli.pretend) {
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
    }
  }

  for (std::pair<std::string, packer::Package*> package_it :package_index) {
    delete package_it.second;
  }
}
