#include "util.h"

#include <iomanip>

namespace util {
std::string getHexString(char *buffer, const size_t size) {
  std::stringstream stream;
  for (int i = 0; i < size; ++i) {
    stream << std::hex << (int)buffer[i];
  }
  return std::string(stream.str());
}
} // namespace util
