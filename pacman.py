import utils

def install(packages: list[str]):
  if utils.system("sudo pacman -Syu %s" % ' '.join(packages)) != 0:
    raise ValueError("unable to install required packages: %s" % ' '.join(packages))
