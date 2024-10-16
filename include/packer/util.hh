#ifndef PACKER_UTIL
#define PACKER_UTIL
#include <string>
#include <vector>
#include <algorithm>

namespace packer {
  bool is_space_char(char c);
  std::string trim_string(std::string str);

  template <typename T>
  inline void add_if_not_present(std::vector<T>& out, T value) {
    auto it = std::find(out.begin(), out.end(), value);
    if (it == out.end()) {
      out.push_back(value);
    }
  }

  template <typename T>
  inline void merge_vectors(std::vector<T>& out, const std::vector<T>& in) {
    for (const T& obj : in) {
      add_if_not_present(out, obj);
    }
  }

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
