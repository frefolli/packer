import os
import jinja2
import hashlib

def system(cmd: str) -> int:
  print("|>", cmd)
  return os.system(cmd)

def ensure_dir(dirpath: str) -> str:
  dirpath = os.path.expanduser(dirpath)
  if not os.path.exists(dirpath):
    os.makedirs(dirpath)
  return dirpath

def ensure_fields(info: dict[str, str], fields: list):
  for field in fields:
    if isinstance(field, list):
      if field[0] not in info:
        raise ValueError("missing '%s' field in '%s'" % (info, field[0]))
      else:
        ensure_fields(info[field[0]], field[1])
    else:
      if field not in info:
        raise ValueError("missing '%s' field in '%s'" % (info, field))

def acquire_template(name: str):
  environment = jinja2.Environment(loader=jinja2.FileSystemLoader("templates/"))
  template = environment.get_template(name)
  return template

def compute_hash(filepath: str):
  with open(filepath, 'rb') as file:
    return hashlib.sha256(file.read()).hexdigest()

def get_distro() -> str:
  if os.path.exists('/etc/os-release'):
    with open('/etc/os-release', 'r') as file:
      for line in file:
        line: str = line.strip()
        if line.find('ID=') == 0:
          return line[3:]
  raise ValueError("unknown distro")

def first_which_exists(file_list: list[str], prefix: str = '') -> str|None:
  for file in file_list:
    if os.path.exists(os.path.join(prefix, file)):
      return file
  return None

def get_package_manager() -> str:
  package_manager: str|None = first_which_exists(file_list=[
    'dnf', 'yum', 'yay', 'pacman'
  ], prefix='/usr/bin')
  if package_manager is None:
    raise ValueError("unknown package manager")
  return package_manager

def in_packer_format(package_id: str) -> bool:
  ss = package_id.strip("/").split("/")
  return len(ss) == 2
