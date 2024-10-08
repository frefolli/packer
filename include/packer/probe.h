#ifndef PACKER_PROBE
#define PACKER_PROBE
#include <stdbool.h>
#include <stdio.h>
typedef struct Host {
  char* distro;
  char* package_manager;
  char* packaging;
} Host;

char* identify_distro();
char* identify_package_manager();
char* identify_packaging();

void Host__init(Host* host);
void Host__clean(Host* host);
void Host__fprintf(FILE* fd, Host* host);
bool Host__read_into(Host* host);
#endif//PACKER_PROBE
