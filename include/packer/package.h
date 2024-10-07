#ifndef PACKER_PACKAGE
#define PACKER_PACKAGE
#include <packer/packerfile.h>
#include <stdio.h>

struct Package;
typedef struct Package* Packagep;
#define T Packagep
#include <c-templates/vector.h>
#undef T

typedef struct Package {
  char* group;
  char* name;
  Packerfile packerfile;
  Vector_Packagep dependencies;
} Package;

#define T Package
#include <c-templates/vector.h>
#undef T

#define K string
#define V Package
#define K_Equals(Ka, Kb) (strcmp(Ka, Kb) == 0)
#include <c-templates/map.h>
#undef K_Equals
#undef V
#undef K

void Package__init(Package* package);
void Package__clean(Package* package);

Package* Package__new();
void Package__delete(Package* package);

void Package__fprint(FILE* fd, const Package* package);
#endif//PACKER_PACKAGE
