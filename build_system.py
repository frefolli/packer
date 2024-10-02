import rpmbuild
import makepkg

def craft_package(info: dict[str, str], builder: str, refresh_mode: str) -> str:
  if builder == "rpmbuild":
    return rpmbuild.craft(info, refresh_mode)
  elif builder == "makepkg":
    return makepkg.craft(info, refresh_mode)
  else:
    ValueError(builder)
