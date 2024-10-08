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
  Vector_string* makedepends;
  Vector_string* depends;
  size_t depends_on;
  size_t depended_on;
} Package;

#define T Package
#include <c-templates/vector.h>
#undef T

#define K string
#define const_K const char*
#define K_Equals(Ka, Kb) (strcmp(Ka, Kb) == 0)
#define V Package
#include <c-templates/map.h>
#undef V
#undef K_Equals
#undef const_K
#undef K

void Package__init(Package* package);
void Package__clean(Package* package);

Package* Package__new();
void Package__delete(Package* package);

void Package__fprint(FILE* fd, const Package* package);
#endif//PACKER_PACKAGE
