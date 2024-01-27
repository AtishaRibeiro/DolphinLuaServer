#pragma once

#include "CommonTypes.h"
#include "DolphinAddress.h"

#include "json.hpp"

#include <cstdint>
#include <memory>


class GameObject {
public:
  GameObject(){}
  GameObject(Pointers pointers, std::shared_ptr<PointerCache> pointerCache)
      : mPointers(pointers), mPointerCache(pointerCache) {}

  virtual void update() = 0;

protected:
  Pointers mPointers;
  std::shared_ptr<PointerCache> mPointerCache;
};
