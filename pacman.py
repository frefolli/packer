import utils

def install(packages: list[str]):
  utils.system("sudo pacman -Syu %s" % ' '.join(packages))
