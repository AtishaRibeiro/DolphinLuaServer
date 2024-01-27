#pragma once

#include "CommonTypes.h"

#include "json.hpp"
#include <sol/sol.hpp>

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <tuple>

class DolphinAddress {
public:
  DolphinAddress(){};
  DolphinAddress(const u32 address) : mAddress(address) {}

  bool isValid() const { return mAddress.has_value(); }

  u32 getValue() const { return mAddress.value(); }

  u32 getNormalised() const;

  std::string getStr() const;

  DolphinAddress operator[](const DolphinAddress other) const;
  DolphinAddress operator+(const DolphinAddress other) const;
  DolphinAddress operator-(const DolphinAddress other) const;
  bool operator==(const DolphinAddress other) const;
  bool operator<(const DolphinAddress other) const;

  // LUA binding
  DolphinAddress readPointerChain(sol::variadic_args va);
  DolphinAddress add(const u32 val) const;
  DolphinAddress sub(const u32 val) const;
  bool eq(const u32 val) const;

private:
  std::optional<u32> mAddress;
};

// JSON serialization
namespace nlohmann {
template <> struct adl_serializer<DolphinAddress> {
  static void to_json(json &j, DolphinAddress dA) { j = dA.getStr(); }
  static void from_json(json &j, DolphinAddress dA) {dA = 0;}
};
} // namespace nlohmann

struct Pointers {
  DolphinAddress raceInfo;
  DolphinAddress playerHolder;
  DolphinAddress cameraManager;
  // I don't know what this is but it has camera related info
  DolphinAddress cameraManager2;
  DolphinAddress kartObjectManager;
};

class PointerCache {
public:
  /// Returns the pointer located at `address` and also a bool that is true when
  /// the pointer is different from the one stored in the cache. False if it is
  /// the same. The cache is updated in case it is different.
  std::tuple<DolphinAddress, bool> getPointer(const DolphinAddress address);

private:
  std::map<DolphinAddress, DolphinAddress> mCache;
};
