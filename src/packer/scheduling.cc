#include <packer/scheduling.hh>
#include <packer/serde.hh>
#include <packer/logging.hh>
#include <packer/io.hh>

bool schedule_package(std::unordered_map<std::string, packer::Package*>& packages, std::unordered_map<std::string, packer::Packerfile>& packerfile_index, std::vector<std::string>& scheduling_queue, packer::Packerfile& packerfile) {
  packer::Package* package = new packer::Package();
  for (std::string dependency : packerfile.baseline.makedepends) {
    if (packer::is_package_id(dependency)) {
      if (packerfile_index.find(dependency) == packerfile_index.end()) {
        packer::Packerfile dependency_packerfile;
        if (!packer::load_from_package_id(packerfile, dependency)) {
          packer::throw_warning(MSG("Packerfile of dependency'" << dependency << "' of '" << packerfile.locator.group << "/" << packerfile.locator.name << "' couldn't be loaded"));
          return false;
        }
        packerfile_index[dependency] = packerfile;
        scheduling_queue.push_back(dependency);
      }
    } else {
      package->build_system_requirements.push_back(dependency);
    }
  }
  return false;
}

bool packer::schedule_packages(std::unordered_map<std::string, Package*>& packages, std::unordered_map<std::string, Packerfile>& packerfile_index) {
  bool ok = true;
  packages.clear();
  std::vector<std::string> scheduling_queue = {};
  for (std::pair<std::string, packer::Packerfile> packerfile_it : packerfile_index) {
    scheduling_queue.push_back(packerfile_it.first);
  }
  while (!scheduling_queue.empty()) {
    std::string package_id = scheduling_queue.back();
    scheduling_queue.pop_back();
    Packerfile packerfile = packerfile_index.at(package_id);
    if (!schedule_package(packages, packerfile_index, scheduling_queue, packerfile)) {
      packer::throw_warning(MSG("packerfile '" << package_id <<  "' couldn't be scheduled"));
      ok = false;
      break;
    }
  }
  return ok;
}
