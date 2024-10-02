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
  parser.add_argument("-i", "--install", default=(os.getenv('PACKER_INSTALL') or False), action="store_true", help="wheter to install immediately crafted packages (mandatory for passes which are required in order to build other packages)")
  args = parser.parse_args(sys.argv[1:])

  packages = []
  for package in args.packages:
    info = repository.acquire_package_info(package, args.strict)
    packages.append(info)
  passes = package_manager.compute_passes(packages, args.strict)
  last_stage = len(passes) - 1
  for (stage_index, stage) in enumerate(passes):
    make_dependencies = package_manager.extract_make_dependencies_to_install(stage)
    make_dependencies = package_manager.get_list_of_uninstalled_packages(make_dependencies)
    package_manager.install_dependencies(make_dependencies)
    for package in stage:
      package_file = build_system.craft_package(package, args.refresh)
      if stage_index < last_stage or args.install:
        package_manager.install_dependencies([package_file])
