import utils
import repository
import dnf
import yum
import yay
import pacman

def index_packages(packages: list[dict[str, str]], extra_info = None) -> dict[str, list[dict[str, str]]]:
  return {'%s/%s'%(package['group'], package['name']):[package, extra_info] for package in packages}

def length_dependency_chain(index_of_packages: dict[str, list[dict[str, str]]], package_id: str, strict_mode: bool) -> int:
  if package_id not in index_of_packages:
    index_of_packages[package_id] = [repository.acquire_package_info(package_id, strict_mode), 0]

  if index_of_packages[package_id][1] is not None:
    return index_of_packages[package_id][1]
  else:
    dependencies = [length_dependency_chain(index_of_packages, dependency_id, strict_mode) for dependency_id in index_of_packages[package_id][0]['makedepends'] if utils.in_packer_format(dependency_id)]
    length: int
    if len(dependencies) > 0:
      length = max(dependencies) + 1
    else:
      length = 0
    index_of_packages[package_id][1] = length
    return length

def identify_cross_dependencies(packages: list[dict[str, str]], strict_mode: bool) -> dict[str, list[dict[str, str]]]:
  index_of_packages = index_packages(packages, None)
  for package_id in list(index_of_packages.keys()):
    length_dependency_chain(index_of_packages, package_id, strict_mode)
  return index_of_packages

def compute_passes(packages: list[dict[str, str]], strict_mode: bool) -> list[list[dict[str, str]]]:
  index_of_packages = identify_cross_dependencies(packages, strict_mode)
  # This function should recognize dependencies in the form of <group>/<name> to other Packer's packages
  # Thhis packages should be installed before the requirent because otherwise the user may be using old versions of such packages or worse he hasn't them
  pass

def extract_make_dependencies_to_install(packages: list[dict[str, str]]) -> list[str]:
  dependencies: dict[str, bool] = {}
  for package in packages:
    dependencies.update({dependency:True for dependency in package['makedepends']})
  return list(dependencies.keys())

def install_dependencies(dependencies: list[str]):
  match utils.get_package_manager():
    case 'dnf':
      dnf.install(dependencies)
    case 'yum':
      yum.install(dependencies)
    case 'yay':
      yay.install(dependencies)
    case 'pacman':
      pacman.install(dependencies)
