#include <stdio.h>
#include <sys/stat.h>
#include <assert.h>
#include <packer/packerfile.h>

int main(int argc, char** args) {
  const char* filepath = "repository/frefolli/lartc";
  if (argc > 1) {
    filepath = args[1];
  }

  struct stat statbuf;
  if (stat(filepath, &statbuf) != 0) {
    fprintf(stderr, "%s: ", filepath);
    perror(NULL);
    exit(1);
  }

  Packerfile* packerfile = Packerfile__parse(filepath);
  assert(packerfile);
  Packerfile__print(packerfile);
  Packerfile__delete(packerfile);
}
