#ifndef PACKER_UTIL
#define PACKER_UTIL
#include <string>

namespace packer {
  bool is_space_char(char c);
  std::string trim_string(std::string str);

  // taken from https://en.cppreference.com/w/cpp/utility/unreachable
  [[noreturn]] inline void unreachable() {
    #if defined(_MSC_VER) && !defined(__clang__) // MSVC
      __assume(false);
    #else // GCC, Clang
      __builtin_unreachable();
    #endif
  }
};

#endif//PACKER_UTIL
