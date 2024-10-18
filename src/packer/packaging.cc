#include <packer/packaging.hh>
#include <packer/logging.hh>
#include <packer/system.hh>
#include <filesystem>
#include <fstream>

namespace packer::rpmbuild {
  std::optional<std::string> craft_spec(const std::string& homedir, packer::Package* package) {
    std::string spec_dir = std::filesystem::path(MSG(homedir << "/rpmbuild/SPECS/").str());
    if (!std::filesystem::exists(spec_dir)) {
      std::filesystem::create_directories(spec_dir);
    }
    std::string filepath = std::filesystem::path(MSG(spec_dir << "/" << package->locator.name << ".spec").str());
    if (std::filesystem::exists(filepath) && !package->refresh_before_build) {
      return filepath;
    }

    auto buffer = MSG("");
    buffer << "AutoReq:        no" << std::endl;
    buffer << "Name:           " << package->locator.name << std::endl;
    buffer << "Version:        " << package->version << std::endl;
    buffer << "Release:        1" << std::endl;
    buffer << "BuildArch:      x86_64" << std::endl;
    buffer << "Summary:        " << package->summary << std::endl;
    buffer << "License:        " << package->license << std::endl;
    buffer << "Source:         " << package->locator.name << ".tar.gz" << std::endl;

    if (!(package->build_packer_requirements.empty() && package->build_system_requirements.empty())) {
      buffer << "BuildRequires:     ";
      for (packer::Package* dependency : package->build_packer_requirements) {
        buffer << " " << dependency->locator.name;
      }
      for (std::string dependency : package->build_system_requirements) {
        buffer << " " << dependency;
      }
      buffer << std::endl;
    }

    if (!(package->install_packer_requirements.empty() && package->install_system_requirements.empty())) {
      buffer << "Requires:     ";
      for (packer::Package* dependency : package->install_packer_requirements) {
        buffer << " " << dependency->locator.name;
      }
      for (std::string dependency : package->install_system_requirements) {
        buffer << " " << dependency;
      }
      buffer << std::endl;
    }
    
    buffer << "" << std::endl;
    buffer << "Url: " << package->locator.url << std::endl;
    buffer << "Prefix: /usr" << std::endl;
    buffer << "" << std::endl;
    buffer << "%description" << std::endl;
    buffer << package->summary << std::endl;
    buffer << "" << std::endl;
    buffer << "%prep" << std::endl;
    buffer << "%setup -n " << package->locator.name << "-master" << std::endl;
    buffer << "" << std::endl;
    buffer << "%install" << std::endl;
    buffer << "# reset buildroot" << std::endl;
    buffer << "rm -rf $RPM_BUILD_ROOT" << std::endl;
    buffer << "mkdir -p $RPM_BUILD_ROOT" << std::endl;
    buffer << "export DESTDIR=$RPM_BUILD_ROOT" << std::endl;
    buffer << "export PREFIX=/usr" << std::endl;
    buffer << package->build_script << std::endl;
    buffer << package->install_script << std::endl;
    buffer << "find $RPM_BUILD_ROOT -type f,l | sed \"s|^$RPM_BUILD_ROOT|/|\" > .extra_files" << std::endl;
    buffer << "" << std::endl;
    buffer << "%clean" << std::endl;
    buffer << "rm -rf $RPM_BUILD_ROOT" << std::endl;
    buffer << "" << std::endl;
    buffer << "%files -f .extra_files" << std::endl;
    buffer << "" << std::endl;
    buffer << "%changelog" << std::endl;
    buffer << "* Sat Aug 03 2024 Refolli Francesco <frefolli@campus.unimib.it> - " << package->version << std::endl;
    buffer << "- Created a RPM Package for this Library" << std::endl;

    std::ofstream file (filepath);
    if (!file.good()) {
      throw_warning(MSG("unable to open '" << filepath << "' spec-file for writing"));
      return std::nullopt;
    }
    file << buffer.str();
    file.close();
    return filepath;
  }

  std::optional<std::string> download_source(const std::string& homedir, packer::Package* package) {
    std::string source_dir = std::filesystem::path(MSG(homedir << "/rpmbuild/SOURCES").str());
    if (!std::filesystem::exists(source_dir)) {
      std::filesystem::create_directories(source_dir);
    }
    std::string url = MSG(package->locator.url << "/archive/refs/heads/master.tar.gz").str();
    std::string filepath = std::filesystem::path(MSG(source_dir << "/" << package->locator.name <<".tar.gz").str());
    if (std::filesystem::exists(filepath) && !package->refresh_before_build) {
      return filepath;
    }
    if (!packer::download_file(url, filepath)) {
      return std::nullopt;
    }
    return filepath;
  }
  
  std::optional<std::string> assemble_package(const std::string& homedir, const std::string& spec_file, packer::Package* package) {
    std::string build_dir = std::filesystem::path(MSG(homedir << "/rpmbuild/RPMS/x86_64").str());
    if (!std::filesystem::exists(build_dir)) {
      std::filesystem::create_directories(build_dir);
    }
    std::string package_file = std::filesystem::path(MSG(build_dir << "/" << package->locator.name << "-" << package->version << "-1.x86_64.rpm").str());
    if (std::filesystem::exists(package_file) && !package->refresh_before_build) {
      return package_file;
    }
    if (!packer::execute_shell_command(MSG("rpmbuild -bb " << spec_file).str())) {
      return std::nullopt;
    }
    return package_file;
  }
  
  std::optional<std::string> sign_package(const std::string& rpm_package) {
    if (!packer::execute_shell_command(MSG("rpmsign --addsign " << rpm_package).str())) {
      return std::nullopt;
    }
    return rpm_package;
  }
}

namespace packer::makepkg {
  std::optional<std::string> download_source(const std::string& homedir, packer::Package* package) {
    std::string source_dir = std::filesystem::path(MSG(homedir << "/makepkg/SOURCES").str());
    if (!std::filesystem::exists(source_dir)) {
      std::filesystem::create_directories(source_dir);
    }
    std::string url = MSG(package->locator.url << "/archive/refs/heads/master.tar.gz").str();
    std::string filepath = std::filesystem::path(MSG(source_dir << "/" << package->locator.name <<".tar.gz").str());
    if (std::filesystem::exists(filepath) && !package->refresh_before_build) {
      return filepath;
    }
    if (!packer::download_file(url, filepath)) {
      return std::nullopt;
    }
    return filepath;
  }
  
  std::optional<std::string> craft_pkgbuild(const std::string& homedir, packer::Package* package) {
    std::string pkgbuild_dir = std::filesystem::path(MSG(homedir << "/makepkg/PKGBUILDS").str());
    if (!std::filesystem::exists(pkgbuild_dir)) {
      std::filesystem::create_directories(pkgbuild_dir);
    }
    std::string filepath = std::filesystem::path(MSG(pkgbuild_dir << "/" << package->locator.name << ".pkgbuild").str());
    if (std::filesystem::exists(filepath) && !package->refresh_before_build) {
      return filepath;
    }

    auto buffer = MSG("");
    buffer << "# Contributor & Maintainer: Refolli Francesco <francesco.refolli@gmail.com>" << std::endl;
    buffer << "pkgname=" << package->locator.name << "" << std::endl;
    buffer << "pkgver=" << package->version << "" << std::endl;
    buffer << "pkgrel=1" << std::endl;
    buffer << "pkgdesc='" << package->summary << "'" << std::endl;
    buffer << "license=('" << package->license << "')" << std::endl;
    buffer << "arch=('x86_64')" << std::endl;

    if (!(package->build_packer_requirements.empty() && package->build_system_requirements.empty())) {
      buffer << "makedepends=(";
      for (packer::Package* dependency : package->build_packer_requirements) {
        buffer << " '" << dependency->locator.name << "'";
      }
      for (std::string dependency : package->build_system_requirements) {
        buffer << " '" << dependency << "'";
      }
      buffer << ")" << std::endl;
    }

    if (!(package->install_packer_requirements.empty() && package->install_system_requirements.empty())) {
      buffer << "depends=(";
      for (packer::Package* dependency : package->install_packer_requirements) {
        buffer << " '" << dependency->locator.name << "'";
      }
      for (std::string dependency : package->install_system_requirements) {
        buffer << " '" << dependency << "'";
      }
      buffer << ")" << std::endl;
    }

    buffer << "url=" << package->locator.url << "" << std::endl;
    buffer << "source=('sources.tar.gz')" << std::endl;
    buffer << "sha256sums=('SKIP')" << std::endl;
    buffer << "build() {" << std::endl;
    buffer << "cd $pkgname-master" << std::endl;
    buffer << "export PREFIX=/usr" << std::endl;
    buffer << "export DESTDIR=$pkgdir" << std::endl;
    buffer << package->build_script << std::endl;
    buffer << "}" << std::endl;
    buffer << "package() {" << std::endl;
    buffer << "cd $pkgname-master" << std::endl;
    buffer << "export PREFIX=/usr" << std::endl;
    buffer << "export DESTDIR=$pkgdir" << std::endl;
    buffer << package->install_script << std::endl;
    buffer << "}" << std::endl;
    buffer << "options=(!debug)" << std::endl;

    std::ofstream file (filepath);
    if (!file.good()) {
      throw_warning(MSG("unable to open '" << filepath << "' spec-file for writing"));
      return std::nullopt;
    }
    file << buffer.str();
    file.close();
    return filepath;
  }

  std::optional<std::string> assemble_package(const std::string& homedir, const std::string& pkgbuild_file, const std::string& source_file, packer::Package* package) {
    std::string build_dir = std::filesystem::path(MSG(homedir << "/makepkg/BUILDS").str());
    if (!std::filesystem::exists(build_dir)) {
      std::filesystem::create_directories(build_dir);
    }
    std::string package_file = std::filesystem::path(MSG(build_dir << "/" << package->locator.name << "-" << package->version << "-1-x86_64.pkg.tar.zst").str());
    if (std::filesystem::exists(package_file) && !package->refresh_before_build) {
      return package_file;
    }
    if (!packer::execute_shell_command(MSG("ln -sf " << pkgbuild_file << " " << build_dir << "/PKGBUILD").str())) {
      return std::nullopt;
    }
    if (!packer::execute_shell_command(MSG("ln -sf " << source_file << " " << build_dir <<  "/sources.tar.gz").str())) {
      return std::nullopt;
    }
    if (!packer::execute_shell_command(MSG("makepkg -f --sign -D " << build_dir).str())) {
      return std::nullopt;
    }
    return package_file;
  }
}

std::optional<std::string> packer::craft_package(const std::string& homedir, packaging_t packaging, Package* package) {
  switch (packaging) {
    case packer::RPMBUILD:
      {
        std::optional<std::string> spec_file = packer::rpmbuild::craft_spec(homedir, package);
        if (!spec_file.has_value()) {
          throw_warning(MSG("unable to craft spec file for '" << package->locator.id() << "' package"));
          return std::nullopt;
        }
        std::optional<std::string> source_file = packer::rpmbuild::download_source(homedir, package);
        if (!source_file.has_value()) {
          throw_warning(MSG("unable to download source for '" << package->locator.id() << "' package"));
          return std::nullopt;
        }
        std::optional<std::string> rpm_package = packer::rpmbuild::assemble_package(homedir, spec_file.value(), package);
        if (!rpm_package.has_value()) {
          throw_warning(MSG("unable to assemble rpm package for '" << package->locator.id() << "' package"));
          return std::nullopt;
        }
        rpm_package = packer::rpmbuild::sign_package(rpm_package.value());
        if (!rpm_package.has_value()) {
          throw_warning(MSG("unable to sign rpm package for '" << package->locator.id() << "' package"));
          return std::nullopt;
        }
        return rpm_package;
      }; break;
    case packer::MAKEPKG:
      {
        std::optional<std::string> pkgbuild_file = packer::makepkg::craft_pkgbuild(homedir, package);
        if (!pkgbuild_file.has_value()) {
          throw_warning(MSG("unable to craft pkgbuild file for '" << package->locator.id() << "' package"));
          return std::nullopt;
        }
        std::optional<std::string> source_file = packer::makepkg::download_source(homedir, package);
        if (!source_file.has_value()) {
          throw_warning(MSG("unable to download source for '" << package->locator.id() << "' package"));
          return std::nullopt;
        }
        std::optional<std::string> abs_package = packer::makepkg::assemble_package(homedir, pkgbuild_file.value(), source_file.value(), package);
        if (!abs_package.has_value()) {
          throw_warning(MSG("unable to assemble and sign abs package for '" << package->locator.id() << "' package"));
          return std::nullopt;
        }
        return abs_package;
      }; break;
    default: raise_error(1, MSG("packaging := " << packaging));
  }
  packer::unreachable();
}
