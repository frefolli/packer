#include <packer/package.h>
#include <packer/repository.h>

void Package__init(Package* package) {
  package->name = NULL;
  package->group = NULL;
  Packerfile__init(&package->packerfile);
  Vector_Packagep__init(&package->dependencies);
}

void Package__clean(Package* package) {
  if (package->group != NULL) {
    free(package->group);
    package->group = NULL;
  }
  if (package->name != NULL) {
    free(package->name);
    package->name = NULL;
  }
  Packerfile__clean(&package->packerfile);
  Vector_Packagep__clean(&package->dependencies);
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
  fprintf(fd, "dependencies:\n");
  for (__auto_type it = Vector_Packagep__cbegin(&package->dependencies);
      it != Vector_Packagep__cend(&package->dependencies);
      ++it) {
    fprintf(fd, "- %s/%s\n", (*it)->group, (*it)->name);
  }
}
