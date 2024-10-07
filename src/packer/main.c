#include <stdio.h>
#include <assert.h>
#include <packer/packerfile.h>
#include <packer/package.h>
#include <packer/repository.h>

int main(int argc, char** args) {
  int32_t exit_code = 0;

  Vector_Package packages;
  Vector_Package__init(&packages);

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
    if (!retrieve_package_into(&package, package_id)) {
      fprintf(stderr, "unable to read info for package '%s'\n", package_id);
      fprintf(stderr, "Packerfile may not be present in remote repository or the file in the cache could be corrupted.\n");
      exit_code = 1;
      continue;
    }
    Vector_Package__push_back(&packages, package);
  }

  if (exit_code == 0) {
    if (Vector_Package__empty(&packages)) {
      fprintf(stderr, "error: no package specified\n");
      exit_code = 1;
    } else {
      for (__auto_type it = Vector_Package__begin(&packages);
          it != Vector_Package__end(&packages);
          ++it) {
        Package__fprint(stdout, it);
      }
    }
  }

  for (__auto_type it = Vector_Package__begin(&packages);
      it != Vector_Package__end(&packages);
      ++it) {
    Package__clean(it);
  }
  Vector_Package__clean(&packages);

  return exit_code;
}
