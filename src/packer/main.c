#include <stdio.h>
#include <assert.h>
#include <packer/packerfile.h>
#include <packer/package.h>
#include <packer/repository.h>
#include <packer/analysis.h>
#include <packer/probe.h>

int main(int argc, char** args) {
  int32_t exit_code = 0;

  Host host;
  Host__init(&host);
  if (!Host__read_into(&host)) {
    fprintf(stderr, "unable to retrieve information about the host machine\n");
    return 1;
  }
  Host__fprintf(stdout, &host);

  Map_string_Package packages;
  Map_string_Package__init(&packages);

  const char* package_id = "";
  for (int32_t arg_index = 1; arg_index < argc; ++arg_index) {
    package_id = args[arg_index];
    fprintf(stdout, "reading %s\n", package_id);

    if (!is_package_id(package_id)) {
      fprintf(stderr, "input '%s' is not a package_id in the form <group>/<name>\n", package_id);
      exit_code = 1;
      continue;
    }

    Package package;
    Package__init(&package);
    if (!retrieve_package_into(&host, &package, package_id)) {
      fprintf(stderr, "unable to read info for package '%s'\n", package_id);
      fprintf(stderr, "Packerfile may not be present in remote repository or the file in the cache could be corrupted.\n");
      exit_code = 1;
      continue;
    }
    Map_string_Package__put(&packages, strdup(package_id), package);
  }

  if (!analyze_packages(&host, &packages)) {
    fprintf(stderr, "unable to analyze dependencies of packages\n");
    exit_code = 1;
  }

  if (exit_code == 0) {
    if (Map_string_Package__empty(&packages)) {
      fprintf(stderr, "error: no package specified\n");
      exit_code = 1;
    } else {
      for (__auto_type it = Map_string_Package__cbegin(&packages);
          it != Map_string_Package__cend(&packages);
          ++it) {
        Package__fprint(stdout, &it->second);
      }
    }
  }

  for (__auto_type it = Map_string_Package__begin(&packages);
      it != Map_string_Package__end(&packages);
      ++it) {
    Package__clean(&it->second);
    free_and_clean_charpp(&it->first);
  }
  Map_string_Package__clean(&packages);

  Host__clean(&host);
  return exit_code;
}
