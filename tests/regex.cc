#include <cassert>
#include <regex>
int main() {
  std::regex regex ("Name\\s+:\\s+ ([a-zA-Z_0-9+-]+)");
  std::string input = "Name        :   meson\nName        :   gcc-c++\n";
  auto matches_begin = std::sregex_iterator(input.begin(), input.end(), regex);
  auto matches_end = std::sregex_iterator();
  bool found_meson = false;
  bool found_gcc_cpp = false;
  auto matches_it = matches_begin;
  while (matches_it != matches_end) {
    std::smatch match = *matches_it;
    if (match[1] == "meson") {
      assert(!found_meson);
      found_meson = true;
    } else if (match[1] == "gcc-c++") {
      assert(!found_gcc_cpp);
      found_gcc_cpp = true;
    }
    ++matches_it;
  }
}
