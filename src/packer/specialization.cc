#include <packer/specialization.hh>
#include <packer/util.hh>
#include <packer/logging.hh>
#include <filesystem>

std::optional<std::string> packer::identify_distro() {
  std::string release_file = "";
  if (std::filesystem::exists("/etc/os-release")) {
    release_file = "/etc/os-release";
  }

  if (release_file.empty()) {
    packer::throw_warning(MSG("release file couldn't be located"));
    return std::nullopt;
  }

  FILE* fd = std::fopen(release_file.c_str(), "r");
  if (fd == nullptr) {
    packer::throw_warning(MSG("release file '" << release_file << "' couldn't be opened for reading"));
    return std::nullopt;
  }

  std::string result = "";
  static char buffer[64];
  char next_c;
  do {
    if (fscanf(fd, "ID=%s", buffer) == 1) {
      result = buffer;
      break;
    }
    next_c = fgetc(fd);
    while(next_c != '\n') {
      next_c = fgetc(fd);
    }
  } while(next_c != EOF);
  fclose(fd);

  if (result.empty()) {
    packer::throw_warning(MSG("release file '" << release_file << "' is ill-formed"));
    return std::nullopt;
  }
  return result;
}

std::optional<std::string> packer::identify_package_manager() {
  if (std::filesystem::exists("/usr/bin/dnf"))
    return "/usr/bin/dnf";
  if (std::filesystem::exists("/usr/bin/yum"))
    return "/usr/bin/yum";
  if (std::filesystem::exists("/usr/bin/yay"))
    return "/usr/bin/yay";
  if (std::filesystem::exists("/usr/bin/pacman"))
    return "/usr/bin/pacman";
  return std::nullopt;
}

std::optional<std::string> packer::identify_packaging() {
  if (std::filesystem::exists("/usr/bin/makepkg"))
    return "/usr/bin/makepkg";
  if (std::filesystem::exists("/usr/bin/rpmbuild"))
    return "/usr/bin/rpmbuild";
  return std::nullopt;
}

bool packer::probe_host(packer::Host& out) {
  const auto maybe_distro = identify_distro();
  const auto maybe_package_manager = identify_package_manager();
  const auto maybe_packaging = identify_packaging();
  if (!(maybe_distro.has_value() && maybe_package_manager.has_value() && maybe_packaging.has_value())) {
    return false;
  }
  out.distro = maybe_distro.value();
  out.package_manager = maybe_package_manager.value();
  out.packaging = maybe_packaging.value();
  return true;
}

void packer::patch(const packer::Host& host, packer::Packerfile& packerfile) {
  auto patch = packerfile.patches.find(host.distro);
  if (patch != packerfile.patches.end()) {
    if (!patch->second.build_script.empty())
      packerfile.baseline.build_script += "\n" + patch->second.build_script;
    if (!patch->second.install_script.empty())
      packerfile.baseline.install_script += "\n" + patch->second.install_script;
    packer::merge_vectors(packerfile.baseline.depends, patch->second.depends);
    packer::merge_vectors(packerfile.baseline.makedepends, patch->second.makedepends);
  } else {
    packer::throw_warning(MSG("missing patch of '" << packerfile.locator.id()  <<  "' for  '" << host.distro << "' is always interpreted as not-needed (add an empty clause to solve this warning)"));
  }
  packerfile.patches.clear();
}
