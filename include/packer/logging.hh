#ifndef PACKER_LOGGING
#define PACKER_LOGGING
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <packer/util.hh>

namespace packer {
  constexpr const char* RED_TEXT = "\x1b[1;31m";
  constexpr const char* PURPLE_TEXT = "\x1b[1;35m";
  constexpr const char* AZURE_TEXT = "\x1b[1;36m";
  constexpr const char* NORMAL_TEXT = "\x1b[0;39m";

  #define MSG(...) \
    (std::ostringstream() << __VA_ARGS__)

  inline void throw_warning(std::ostringstream msg) {
    std::cerr << PURPLE_TEXT << "warning" << NORMAL_TEXT << ": " << msg.str() << std::endl;
  }

  inline void throw_debug(std::ostringstream msg) {
    std::cerr << AZURE_TEXT << "debug" << NORMAL_TEXT << ": " << msg.str() << std::endl;
  }

  inline void raise_error(int status, std::ostringstream msg) {
    std::cerr << RED_TEXT << "error" << NORMAL_TEXT << ": " << msg.str() << std::endl;
    exit(status);
    packer::unreachable();
  }
}

#endif//PACKER_LOGGING
