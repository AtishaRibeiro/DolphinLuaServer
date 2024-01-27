#pragma once

#include "CommonTypes.h"
#include "DolphinAddress.h"
#include "Error.h"
#include <sol/sol.hpp>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <expected>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

// Singleton class to access Dolphin memory
class DolphinInterface {
public:
  static DolphinInterface &Instance() {
    static DolphinInterface instance;
    return instance;
  }

  void printInfo();

  Result hook();

  Expected<std::string> readGameID();

  Result readFromRam(const DolphinAddress address, char *buffer,
                     const std::size_t size) const;

  DolphinAddress readPointer(const DolphinAddress address) const;

  Expected<std::string> readString(const DolphinAddress address,
                                   const std::size_t size) const;

  /// Read value of type T at the given address.
  /// When `reverseWords` is true, the read value will be reversed on a
  /// word-by-word basis to counteract memcpy reversing byte order.
  template <typename T>
  std::optional<T> readType(const DolphinAddress address,
                            const bool reverseWords = true) const {
    char buffer[sizeof(T)];
    if (auto res = readFromRam(address, buffer, sizeof(T)); res.isError()) {
      // std::cerr << res.getError().str() << std::endl;
      return std::nullopt;
    }

    if (reverseWords) {
      // The amount of bytes we reverse at a time is based on the type.
      // This doesn't handle all cases correctly e.g. a type consisting of two
      // u16's. In that case sizeof(T) will be 4 and the entire 4 bytes will be
      // reversed as one word, even. I don't see a good way of handling that
      // case though.
      int wordSize = sizeof(T) >= 4 ? 4 : sizeof(T);
      reverseBufferBytes(buffer, sizeof(T), wordSize);
    }

    T value;
    std::memcpy(&value, buffer, sizeof(T));
    return value;
  }

  template <typename T>
  std::optional<std::vector<T>>
  readVectorOfType(const DolphinAddress address, const std::size_t size,
                   const bool reverseWords = true) const {
    auto bufferSize = sizeof(T) * size;
    char buffer[bufferSize];
    if (auto res = readFromRam(address, buffer, bufferSize); res.isError()) {
      // TODO: log
      return std::nullopt;
    }

    if (reverseWords) {
      reverseBufferBytes(buffer, bufferSize, sizeof(T));
    }

    std::vector<T> vec(size);
    for (int i = 0; i < size; ++i) {
      memcpy(&vec[i], &buffer[i * sizeof(T)], sizeof(T));
    }
    return vec;
  }

  /// LUA bindings

  template <typename T>
  static std::optional<T> readTypeLua(const DolphinAddress address) {
    return Instance().readType<T>(address);
  }

  template <typename T>
  static std::optional<std::vector<T>>
  readVectorOfTypeLua(const DolphinAddress address, const size_t size) {
    return Instance().readVectorOfType<T>(address, size);
  }

  static std::optional<std::string> readStringLua(const DolphinAddress address,
                                                  const std::size_t size) {
    auto res = Instance().readString(address, size);
    if (res.isError()) {
      // TODO: log
      // std::cerr << res.getError().str() << std::endl;
      return std::nullopt;
    }
    return res.getValue();
  }

private:
  DolphinInterface() {}
  DolphinInterface(const DolphinInterface &) = delete;
  DolphinInterface &operator=(const DolphinInterface &) = delete;
  ~DolphinInterface() {}

  Result findRamAddress();
  void reverseBufferBytes(char *buffer, const size_t bufferSize,
                          const size_t wordSize) const;

  bool mHooked = false;
  int mPid;
  u64 mRamAddress;
};
