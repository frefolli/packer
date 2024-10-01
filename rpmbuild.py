import utils
import os

SpecFile=str
SourceFile=str
PackageFile=str

def craft_spec(info: dict[str, str], refresh: bool) -> SpecFile:
  template = utils.acquire_template('specfile')
  spec = template.render(**info, len=len)
  filepath = os.path.expanduser("~/rpmbuild/SPECS/%s.spec" % info["name"])
  with open(filepath, mode="w") as file:
    file.write(spec)
  return filepath

def download_source(info: dict[str, str], refresh: bool) -> SourceFile:
  target = "%s/archive/refs/heads/master.tar.gz" % info["url"]
  filepath = os.path.expanduser("~/rpmbuild/SOURCES/%s.tar.gz" % info["name"])
  if not os.path.exists(filepath) or refresh:
    utils.system("wget %s -O %s" % (target, filepath))
  return filepath

def find_pkg(info: dict[str, str]) -> PackageFile:
  pkgid = "%s-%s" % (info["name"], info["version"])
  pkgdir = os.path.expanduser("~/rpmbuild/RPMS/x86_64")
  for file in os.listdir(pkgdir):
    if pkgid in file:
      return os.path.join(pkgdir, file)
  return None

def assemble_package(spec_file: SpecFile, info: dict[str, str], refresh: bool) -> PackageFile:
  package_file = find_pkg(info)
  if package_file is None or refresh:
    utils.system("rpmbuild -bb %s" % spec_file)
    package_file = find_pkg(info)
  assert package_file is not None
  return package_file

def sign_package(package_file: PackageFile) -> PackageFile:
  utils.system("rpmsign --addsign %s" % package_file)
  return package_file

def craft(info: dict[str, str], refresh: bool) -> PackageFile:
  utils.ensure_fields(info, ["name", "group", "version", "url", "depends"])
  spec_file: SpecFile = craft_spec(info, refresh)
  source_file: SourceFile = download_source(info, refresh)
  package_file: PackageFile = sign_package(assemble_package(spec_file, info, refresh))
  return package_file
