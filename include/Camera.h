#pragma once

#include "GameObject.h"
#include "CommonTypes.h"

#include "json.hpp"

struct CameraData {
  float camMatrix[12];
  float projectionMatrix[16];
  u32 mFlags;
  Vec3 position; // at 0x74
  Vec3 VEC3_0x80;
  Vec3 VEC3_0x8C;
  Vec3 VEC3_0x98;
  float FLOAT_0xA4;
  float FLOAT_0xA8;
  float FLOAT_0xAC;
  float FLOAT_0xB0;
  float FLOAT_0xB4;
  float FLOAT_0xB8;
  float FLOAT_0xBC;
  float FLOAT_0xC0;
  float FLOAT_0xC4;
  float FLOAT_0xC8;
  float FLOAT_0xCC;
  float FLOAT_0xD0;
  float FLOAT_0xD4;
  float FLOAT_0xD8;
  float FLOAT_0xDC;
  float FLOAT_0xE0;
  float FLOAT_0xE4;
  float FLOAT_0xE8;
  float FLOAT_0xEC;
  float FLOAT_0xF0;
  u32 INT_0xF4;
  u32 INT_0xF8;
  u32 INT_0xFC;
  u32 INT_0x100;
  u32 INT_0x104;
  u32 INT_0x108;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(CameraData, camMatrix, projectionMatrix,
                                 mFlags, position, VEC3_0x80, VEC3_0x8C,
                                 VEC3_0x98, FLOAT_0xA4, FLOAT_0xA8, FLOAT_0xAC,
                                 FLOAT_0xB0, FLOAT_0xB4, FLOAT_0xB8, FLOAT_0xBC,
                                 FLOAT_0xC0, FLOAT_0xC4, FLOAT_0xC8, FLOAT_0xCC,
                                 FLOAT_0xD0, FLOAT_0xD4, FLOAT_0xD8, FLOAT_0xDC,
                                 FLOAT_0xE0, FLOAT_0xE4, FLOAT_0xE8, FLOAT_0xEC,
                                 FLOAT_0xF0, INT_0xF4, INT_0xF8, INT_0xFC,
                                 INT_0x100, INT_0x104, INT_0x108)
};

class Camera : public GameObject {
public:
  Camera(){}
  Camera(Pointers pointers, std::shared_ptr<PointerCache> pointerCache)
      : GameObject(pointers, pointerCache) {}
  void update() override;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Camera, position, rotation, fov)
private:
  Vec3 position;
  Matrix33 rotation;
  float fov;
};
