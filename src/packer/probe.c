#include <packer/probe.h>
#include <packer/utility.h>
#include <stdio.h>

char* identify_distro() {
  const char* release_file = NULL;
  if (file_exists("/etc/os-release")) {
    release_file = "/etc/os-release";
  }

  if (release_file == NULL)
    return NULL;

  FILE* fd = fopen(release_file, "r");
  if (fd == NULL)
    return NULL;

  char* result = NULL;
  static char buffer[64];
  char next_c;
  do {
    if (fscanf(fd, "ID=%s", buffer) == 1) {
      result = strdup(buffer);
      break;
    }
    next_c = fgetc(fd);
    while(next_c != '\n') {
      next_c = fgetc(fd);
    }
  } while(next_c != EOF);

  fclose(fd);
  return result;
}

char* identify_package_manager() {
  if (file_exists("/usr/bin/dnf"))
    return strdup("/usr/bin/dnf");
  if (file_exists("/usr/bin/yum"))
    return strdup("/usr/bin/yum");
  if (file_exists("/usr/bin/yay"))
    return strdup("/usr/bin/yay");
  if (file_exists("/usr/bin/pacman"))
    return strdup("/usr/bin/pacman");
  return NULL;
}

char* identify_packaging() {
  if (file_exists("/usr/bin/makepkg"))
    return strdup("/usr/bin/makepkg");
  if (file_exists("/usr/bin/rpmbuild"))
    return strdup("/usr/bin/rpmbuild");
  return NULL;
}

void Host__init(Host* host) {
  host->distro = NULL;
  host->package_manager = NULL;
  host->packaging = NULL;
}

void Host__clean(Host* host) {
  free_and_clean_charpp(&host->distro);
  free_and_clean_charpp(&host->package_manager);
  free_and_clean_charpp(&host->packaging);
}

void Host__fprintf(FILE* fd, Host* host) {
  fprintf(fd, "distro: %s\n", host->distro);
  fprintf(fd, "package_manager: %s\n", host->package_manager);
  fprintf(fd, "packaging: %s\n", host->packaging);
}

bool Host__read_into(Host* host) {
  host->distro = identify_distro();
  host->package_manager = identify_package_manager();
  host->packaging = identify_packaging();

  if (host->distro == NULL || host->package_manager == NULL || host->packaging == NULL) {
    Host__clean(host);
    return false;
  }
  return true;
}
