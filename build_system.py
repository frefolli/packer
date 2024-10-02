import rpmbuild
import makepkg
import utils

def craft_package(info: dict[str, str], refresh_mode: str) -> str:
  builder = utils.get_build_system()
  if builder == "rpmbuild":
    return rpmbuild.craft(info, refresh_mode)
  elif builder == "makepkg":
    return makepkg.craft(info, refresh_mode)
  else:
    ValueError(builder)
