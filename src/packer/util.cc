#include <packer/util.hh>

bool packer::is_space_char(char c) {
  return c == ' ' || c == '\n' || c == '\t';
}

std::string packer::trim_string(std::string str) {
  int64_t first_meaningful_idx = 0;
  int64_t size = str.size();
  while (first_meaningful_idx < size) {
    if (!packer::is_space_char(str[first_meaningful_idx]))
      break;
    ++first_meaningful_idx;
  }

  if (first_meaningful_idx >= size) {
    return "";
  }

  int64_t last_meaningful_idx = size - 1;
  while (last_meaningful_idx >= 0) {
    if (!packer::is_space_char(str[last_meaningful_idx]))
      break;
    --last_meaningful_idx;
  }

  if (last_meaningful_idx < 0 || last_meaningful_idx < first_meaningful_idx) {
    return  "";
  }

  return str.substr(first_meaningful_idx, last_meaningful_idx - first_meaningful_idx + 1);
}
