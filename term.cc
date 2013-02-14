// term.h - ansi terminal routines
// 2013.02.09

#include "term.h"
#include <sstream>

using std::string;

namespace chessy {
namespace term {

// TODO: Make this better.
string i2s(int x) {
  std::stringstream ss;
  ss << x;
  return ss.str();
}

string x2s(int x) {
  std::stringstream ss;
  ss << std::hex << x;
  return ss.str();
}

}  // namespace term
}  // namespace chessy
