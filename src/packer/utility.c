#include <packer/utility.h>
#include <sys/stat.h>

bool file_exists(const char* filepath) {
  struct stat statbuf;
    return (stat(filepath, &statbuf) == 0);
}
