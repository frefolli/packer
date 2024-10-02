import utils

def install(packages: list[str]):
  utils.system("sudo dnf install %s" % ' '.join(packages))
