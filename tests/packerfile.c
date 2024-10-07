#include <packer/packerfile.h>
#include <assert.h>

int main() {
  Packerfile packerfile;
  Packerfile__init(&packerfile);

  Packerfile__parse_into(&packerfile, "../examples/correct.packerfile");
  Packerfile__clean(&packerfile);

  Packerfile__init(&packerfile);
  Packerfile__parse_into(&packerfile, "../examples/wrong.packerfile");

  Packerfile__clean(&packerfile);
}
