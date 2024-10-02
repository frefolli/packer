import rpmbuild
import makepkg
import utils

def craft_package(info: dict[str, str]) -> str:
  info["depends"] = [dependency_id.strip('/').split('/')[-1] for dependency_id in info["depends"]]
  info["makedepends"] = [dependency_id.strip('/').split('/')[-1] for dependency_id in info["makedepends"]]
  builder = utils.get_build_system()
  if builder == "rpmbuild":
    return rpmbuild.craft(info)
  elif builder == "makepkg":
    return makepkg.craft(info)
  else:
    ValueError(builder)
