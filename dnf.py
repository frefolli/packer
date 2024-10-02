import utils

def install(packages: list[str]):
  if utils.system("sudo dnf install %s" % ' '.join(packages)) != 0:
    raise ValueError("unable to install required packages: %s" % ' '.join(packages))

def get_list_of_installed_packages(packages: list[str]) -> list[str]:
  out = utils.getoutput("dnf info %s --installed | grep \"Name\\s\\+: \"" % ' '.join(packages))
  packages = [line.strip().split(':')[1].strip() for line in out.strip().split('\n')]
  return packages
