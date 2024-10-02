import utils
import hints
import os

def craft_spec(info: dict[str, str]) -> hints.SpecFile:
  filepath = os.path.expanduser("~/rpmbuild/SPECS/%s.spec" % info["name"])
  if not os.path.exists(filepath) or info["refresh"]:
    template = utils.acquire_template('specfile')
    spec = template.render(**info, len=len)
    with open(filepath, mode="w") as file:
      file.write(spec)
  return filepath

def download_source(info: dict[str, str]) -> hints.SourceFile:
  target = "%s/archive/refs/heads/master.tar.gz" % info["url"]
  filepath = os.path.expanduser("~/rpmbuild/SOURCES/%s.tar.gz" % info["name"])
  if not os.path.exists(filepath) or info["refresh"]:
    utils.system("wget %s -O %s" % (target, filepath))
  return filepath

def find_pkg(info: dict[str, str]) -> hints.PackageFile:
  pkgid = "%s-%s" % (info["name"], info["version"])
  pkgdir = os.path.expanduser("~/rpmbuild/RPMS/x86_64")
  for file in os.listdir(pkgdir):
    if pkgid in file:
      return os.path.join(pkgdir, file)
  return None

def assemble_package(spec_file: hints.SpecFile, info: dict[str, str]) -> hints.PackageFile:
  package_file = find_pkg(info)
  if package_file is None or info["refresh"]:
    utils.system("rpmbuild -bb %s" % spec_file)
    package_file = find_pkg(info)
  assert package_file is not None
  return package_file

def sign_package(package_file: hints.PackageFile) -> hints.PackageFile:
  utils.system("rpmsign --addsign %s" % package_file)
  return package_file

def craft(info: dict[str, str]) -> hints.PackageFile:
  utils.ensure_fields(info, ["name", "group", "version", "url", "depends"])
  spec_file: hints.SpecFile = craft_spec(info)
  source_file: hints.SourceFile = download_source(info)
  package_file: hints.PackageFile = sign_package(assemble_package(spec_file, info))
  return package_file
