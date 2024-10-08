#include <packer/package.h>
#include <packer/repository.h>

void Package__init(Package* package) {
  package->name = NULL;
  package->group = NULL;
  Packerfile__init(&package->packerfile);
  package->makedepends = NULL;
  package->depends = NULL;
  package->depends_on = 0;
  package->depended_on = 0;
}

void Package__clean(Package* package) {
  free_and_clean_charpp(&package->group);
  free_and_clean_charpp(&package->name);
  Packerfile__clean(&package->packerfile);
  package->makedepends = NULL;
  package->depends = NULL;
  package->depends_on = 0;
  package->depended_on = 0;
}

Package* Package__new() {
  Package* package = (Package*) malloc(sizeof(Package));
  Package__init(package);
  return package;
}

void Package__delete(Package* package) {
  if (package != NULL) {
    Package__clean(package);
    free(package);
  }
}

void Package__fprint(FILE* fd, const Package* package) {
  fprintf(fd, "group: %s\n", package->group);
  fprintf(fd, "name: %s\n", package->name);
  Packerfile__fprint(fd, &package->packerfile);
  fprintf(fd, "depends_on: %lu\n", package->depends_on);
  fprintf(fd, "depended_on: %lu\n", package->depended_on);
}
