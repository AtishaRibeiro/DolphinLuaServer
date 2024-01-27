#pragma once

#include "json.hpp"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

struct Vec3 {
  float x, y, z;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Vec3, x, y, z)

  std::string str() const;
};

struct Vec4 {
  float w, x, y, z;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Vec4, x, y, z)

  std::string str() const;
};

struct Matrix33 {
  Vec3 x, y, z;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Matrix33, x, y, z)

  std::string str() const;
};

struct Matrix34 {
  Vec3 w, x, y, z;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Matrix34, w, x, y, z)

  std::string str() const;
};

struct Matrix44 {
  Vec4 w, x, y, z;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Matrix44, w, x, y, z)

  std::string str() const;
};
