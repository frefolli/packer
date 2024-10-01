import repository
import argparse
import rpmbuild
import makepkg
import sys

import os

def default_or_fail(rec: dict[str, str], access: str, key: str, value, strict_mode: bool) -> dict[str, str]:
  rec[access] = rec[access].get(key)
  if rec[access] is None:
    if strict_mode:
      raise ValueError("missing key '%s' inside '%s'" % (key, access))
    else:
      rec[access] = value
  return rec

if __name__ == "__main__":
  parser = argparse.ArgumentParser(description="packer for github projects")
  parser.add_argument("packages", type=str, nargs="+", help="form: <user>/<repository>")
  parser.add_argument("-b", "--builder", type=str, default=os.getenv('PACKER_BUILDER'), choices=["rpmbuild", "makepkg"], help="build system to use")
  parser.add_argument("-d", "--distro", type=str, default=os.getenv('PACKER_DISTRO'), help="distro which specializes some fields for Packerfile")
  parser.add_argument("-r", "--refresh", default=(os.getenv('PACKER_REFRESH') or False), action="store_true", help="always refresh files and sources")
  parser.add_argument("-s", "--strict", default=(os.getenv('PACKER_STRICT') or False), action="store_true", help="wheter to fail if some info isn't present")
  args = parser.parse_args(sys.argv[1:])

  assert args.builder is not None
  assert args.distro is not None

  for package in args.packages:
    group, name = package.split('/')
    info = repository.get_package_info(group, name)
    info = default_or_fail(info, "depends", args.distro, [], args.strict)
    info = default_or_fail(info, "makedepends", args.distro, [], args.strict)
    print(info)

    if args.builder == "rpmbuild":
      rpmbuild.craft(info, args.refresh)
    elif args.builder == "makepkg":
      makepkg.craft(info, args.refresh)
    else:
      ValueError(args.builder)
