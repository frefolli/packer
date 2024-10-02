import utils

def install(packages: list[str]):
  utils.system("yay -Syu %s" % ' '.join(packages))
