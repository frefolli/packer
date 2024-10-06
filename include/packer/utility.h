#ifndef PACKER_UTILITY
#define PACKER_UTILITY
#include <string.h>

typedef char* string;
#define T string
#include <c-templates/vector.h>
#undef T

#define K string
#define V Vector_string
#define K_Equals(Ka, Kb) (strcmp(Ka, Kb) == 0)
#include <c-templates/map.h>
#undef K_Equals
#undef V
#undef K
#endif//PACKER_UTILITY
