// term.h - ansi terminal routines
// 2013.02.09

#include "term.h"
#include <sstream>

namespace chessy {
namespace term {

std::string i2s(int x) {
  std::stringstream ss;
  ss << x;
  return ss.str();
}

std::string x2s(int x) {
  std::stringstream ss;
  ss << std::hex << x;
  return ss.str();
}

}  // namespace term
}  // namespace chessy
