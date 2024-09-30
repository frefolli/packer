import utils
import os

PkgbuildFile=str
SourceFile=str
PackageFile=str

def download_source(info: dict[str, str], refresh: bool) -> SourceFile:
  source_dir = os.path.expanduser('~/makepkg/sources')
  utils.ensure_dir(source_dir)
  target = "%s/archive/refs/heads/master.tar.gz" % info["url"]
  filepath = os.path.join(source_dir, "%s.tar.gz" % info["name"])
  if not os.path.exists(filepath) or refresh:
    utils.system("wget %s -O %s" % (target, filepath))
    assert os.path.exists(filepath)
  info["hash"] = utils.compute_hash(filepath)
  return filepath

def craft_pkgbuild(info: dict[str, str], source_file: SourceFile, refresh: bool) -> PkgbuildFile:
  pkgbuild_dir = os.path.expanduser('~/makepkg/pkgbuilds')
  utils.ensure_dir(pkgbuild_dir)
  template = utils.acquire_template('pkgbuild')
  pkgbuild = template.render(**info)
  filepath = os.path.join(pkgbuild_dir, "%s.pkgbuild" % info["name"])
  with open(filepath, mode="w") as file:
    file.write(pkgbuild)
  return filepath

def find_pkg(info: dict[str, str]) -> PackageFile:
  build_dir = os.path.expanduser('~/makepkg/builds')
  pkgid = "%s-" % info["name"]
  pkgdir = utils.ensure_dir(build_dir)
  for file in os.listdir(pkgdir):
    if pkgid in file:
      return os.path.join(pkgdir, file)
  return None

def assemble_package(pkgbuild_file: PkgbuildFile, source_file: SourceFile, info: dict[str, str], refresh: bool) -> PackageFile:
  build_dir = os.path.expanduser('~/makepkg/builds')
  package_file = find_pkg(info)
  if package_file is None or refresh:
    utils.system("ln -sf %s %s/PKGBUILD" % (pkgbuild_file, build_dir))
    utils.system("ln -sf %s %s/sources.tar.gz" % (source_file, build_dir))
    utils.system("makepkg --sign -D %s" % build_dir)
    package_file = find_pkg(info)
  assert package_file is not None
  return package_file

def craft(info: dict[str, str], refresh: bool) -> PackageFile:
  utils.ensure_fields(info, ["name", "group", "version", "url", "depends"])
  source_file: SourceFile = download_source(info, refresh)
  pkgbuild_file: PkgbuildFile = craft_pkgbuild(info, source_file, refresh)
  package_file: PackageFile = assemble_package(pkgbuild_file, source_file, info, refresh)
  return package_file
