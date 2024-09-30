import yaml
import os
import utils

def get_groupdir(group: str) -> str:
  return os.path.join("repository", group)

def get_packerfile(group: str, name: str) -> str:
  return os.path.join("repository", group, name)

def download_packerfile(group: str, name: str) -> str:
  packerfile = get_packerfile(group, name)
  groupdir = get_groupdir(group)
  url = "https://github.com/%s/%s/blob/master/Packerfile" % (group, name)
  utils.system("mkdir -p %s" % groupdir)
  if utils.system("wget %s -o %s" % (url, packerfile)) != 0:
    utils.system("rm %s" % packerfile)
    raise ValueError(packerfile)
  return packerfile

def get_package_info(group: str, name: str) -> dict[str, str]|None:
  packerfile = get_packerfile(group, name)
  if not os.path.exists(packerfile):
    packerfile = download_packerfile(group, name)
  with open(packerfile, "r") as file:
    info = yaml.load(file, Loader=yaml.BaseLoader)
    info['group'] = group
    info['name'] = name
    if 'depends' not in info:
      if 'makedepends' not in info:
        info['depends'] = {key:[] for key in info['makedepends']}
      else:
        info['depends'] = {}
    if 'makedepends' not in info:
      info['makedepends'] = {key:[] for key in info['depends']}

    for key in info['depends']:
      if isinstance(info['depends'][key], str):
        info['depends'][key] = info['depends'][key].strip().split(' ')
    for key in info['makedepends']:
      if isinstance(info['makedepends'][key], str):
        info['makedepends'][key] = info['makedepends'][key].strip().split(' ')
    info["url"] = "https://github.com/%s/%s" % (info["group"], info["name"])
    print(info)
    return info
