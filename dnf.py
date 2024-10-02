import utils

def install(packages: list[str]):
  if utils.system("sudo dnf install %s" % ' '.join(packages)) != 0:
    raise ValueError("unable to install required packages: %s" % ' '.join(packages))

def check_which_packages_need_to_be_installed(packages: list[str]) -> list[str]:
  packages = [package_id for package_id in packages if not utils.in_packer_format(package_id)]
  out = utils.getoutput("dnf info %s --installed | grep \"Name\\s\\+: \"" % ' '.join(packages))
  packages = [line.strip().split(':')[1].strip() for line in out.strip().split('\n')]
  return packages
