#include "DolphinAddress.h"

#include "DolphinInterface.h"

#include <fmt/core.h>

u32 DolphinAddress::getNormalised() const {
  if (mAddress.value() >= 0x80000000) {
    return mAddress.value() - 0x80000000;
  }
  return mAddress.value();
}

std::string DolphinAddress::getStr() const {
  if (!isValid()) {
    return "INVALID ADDRESS";
  }
  return fmt::format("{:#x}", getValue());
}

DolphinAddress DolphinAddress::operator[](const DolphinAddress other) const {
  return DolphinInterface::Instance().readPointer(*this + other);
}

DolphinAddress DolphinAddress::operator+(const DolphinAddress other) const {
  if (!isValid() || !other.isValid()) {
    return DolphinAddress();
  }

  return getValue() + other.getValue();
}

DolphinAddress DolphinAddress::operator-(const DolphinAddress other) const {
  if (!isValid() || !other.isValid()) {
    return DolphinAddress();
  }

  return getValue() - other.getValue();
}

bool DolphinAddress::operator==(const DolphinAddress other) const {
  if (!isValid() && !other.isValid()) {
    return true;
  }
  if (!isValid() || !other.isValid()) {
    return false;
  }
  return getValue() == other.getValue();
}

bool DolphinAddress::operator<(const DolphinAddress other) const {
  if (!isValid() && !other.isValid()) {
    return false;
  }
  if (!isValid()) {
    return true;
  }
  if (other.isValid()) {
    return false;
  }
  return getValue() < other.getValue();
}

DolphinAddress DolphinAddress::readPointerChain(sol::variadic_args va) {
  DolphinAddress result = *this;
  for (const u32 v : va) {
    result = result[v];
  }
  return result;
}

DolphinAddress DolphinAddress::add(const u32 val) const {
  return *this + val;
}

DolphinAddress DolphinAddress::sub(const u32 val) const {
  return *this - val;
}

bool DolphinAddress::eq(const u32 val) const {
  return *this == val;
}
