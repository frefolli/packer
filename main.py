import repository
import package_manager
import build_system
import argparse
import sys
import os

if __name__ == "__main__":
  parser = argparse.ArgumentParser(description="packer for github projects")
  parser.add_argument("packages", type=str, nargs="+", help="form: <user>/<repository>")
  parser.add_argument("-r", "--refresh", default=(os.getenv('PACKER_REFRESH') or False), action="store_true", help="always refresh files and sources")
  parser.add_argument("-s", "--strict", default=(os.getenv('PACKER_STRICT') or False), action="store_true", help="wheter to fail if some info isn't present")
  args = parser.parse_args(sys.argv[1:])

  packages = []
  for package in args.packages:
    info = repository.acquire_package_info(package, args.strict)
    packages.append(info)

  passes = package_manager.compute_passes(packages, args.strict)
  exit(0)
  make_dependencies = package_manager.extract_make_dependencies_to_install(packages)
  package_manager.install_dependencies(make_dependencies)

  for package in packages:
    package_file = build_system.craft_package(package)
    package_manager.install_dependencies(package_file)
