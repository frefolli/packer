import utils

def install(packages: list[str]):
  utils.system("sudo yum install %s" % ' '.join(packages))
