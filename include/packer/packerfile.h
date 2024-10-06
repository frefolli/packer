#ifndef PACKER_PACKERFILE
#define PACKER_PACKERFILE
#include <packer/utility.h>

typedef struct {
  char* version;
  char* license;
  char* summary;
  Map_string_Vector_string makedepends;
  Map_string_Vector_string depends;
} Packerfile;

Packerfile* Packerfile__new();
void Packerfile__delete(Packerfile* packerfile);
void Packerfile__init(Packerfile* packerfile);
void Packerfile__clean(Packerfile* packerfile);
void Packerfile__print(Packerfile* packerfile);
Packerfile* Packerfile__parse(const char* filepath);

#endif//PACKER_PACKERFILE
