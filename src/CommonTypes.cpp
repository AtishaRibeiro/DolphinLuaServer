#include "CommonTypes.h"

#include <fmt/core.h>

std::string Vec3::str() const {
  return fmt::format("{: 12.4f} | {: 12.4f} | {: 12.4f}", x, y, z);
}

std::string Vec4::str() const {
  // clang-format off
  return fmt::format("{: 12.4f} | {: 12.4f} | {: 12.4f} | {: 12.4f}", w, x, y, z);
  // clang-format on
}

std::string Matrix33::str() const {
  std::string str = "-----------------\n";
  str += fmt::format("{: 12.4f} | {: 12.4f} | {: 12.4f}\n", x.x, y.x, z.x);
  str += fmt::format("{: 12.4f} | {: 12.4f} | {: 12.4f}\n", x.y, y.y, z.y);
  str += fmt::format("{: 12.4f} | {: 12.4f} | {: 12.4f}\n", x.z, y.z, z.z);
  str += "-----------------\n";
  return str;
}

std::string Matrix34::str() const {
  // clang-format off
  std::string str = "-----------------\n";
  str += fmt::format("{: 12.4f} | {: 12.4f} | {: 12.4f} | {: 12.4f}\n", w.x, x.x, y.x, z.x);
  str += fmt::format("{: 12.4f} | {: 12.4f} | {: 12.4f} | {: 12.4f}\n", w.y, x.y, y.y, z.y);
  str += fmt::format("{: 12.4f} | {: 12.4f} | {: 12.4f} | {: 12.4f}\n", w.z, x.z, y.z, z.z);
  str += "-----------------\n";
  // clang-format on
  return str;
}

std::string Matrix44::str() const {
  // clang-format off
  std::string str = "-----------------\n";
  str += fmt::format("{: 12.4f} | {: 12.4f} | {: 12.4f} | {: 12.4f}\n", w.w, x.w, y.w, w.w);
  str += fmt::format("{: 12.4f} | {: 12.4f} | {: 12.4f} | {: 12.4f}\n", w.x, x.x, y.x, z.x);
  str += fmt::format("{: 12.4f} | {: 12.4f} | {: 12.4f} | {: 12.4f}\n", w.y, x.y, y.y, z.y);
  str += fmt::format("{: 12.4f} | {: 12.4f} | {: 12.4f} | {: 12.4f}\n", w.z, x.z, y.z, z.z);
  str += "-----------------\n";
  // clang-format on
  return str;
}
