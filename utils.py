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
