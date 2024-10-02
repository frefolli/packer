import utils

def install(packages: list[str]):
  if utils.system("yay -Syu %s" % ' '.join(packages)) != 0:
    raise ValueError("unable to install required packages: %s" % ' '.join(packages))
