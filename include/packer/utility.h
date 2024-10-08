#ifndef PACKER_UTILITY
#define PACKER_UTILITY
#include <string.h>

typedef char* string;
#define T string
#include <c-templates/vector.h>
#undef T

#define K string
#define const_K const char*
#define K_Equals(Ka, Kb) (strcmp(Ka, Kb) == 0)
#define V Vector_string
#include <c-templates/map.h>
#undef V
#undef K_Equals
#undef const_K 
#undef K

bool file_exists(const char* filepath);
static inline void free_and_clean_voidpp(void** voidpp) {
  if (voidpp != NULL) {
    if (*voidpp != NULL) {
      free(*voidpp);
      *voidpp = NULL;
    }
  }
}
static inline void free_and_clean_charpp(char** charpp) {
  free_and_clean_voidpp((void**) charpp);
}
#endif//PACKER_UTILITY
