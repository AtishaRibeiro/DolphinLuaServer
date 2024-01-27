#include "Camera.h"

#include "CommonTypes.h"
#include "DolphinInterface.h"

void Camera::update() {
  auto cameraAddress = mPointers.cameraManager[0x0] + 0x6C;
  auto cameraRes =
      DolphinInterface::Instance().readType<CameraData>(cameraAddress);
  if (!cameraRes.has_value()) {
    return;
  }
  auto camera = cameraRes.value();
  position = Vec3{camera.FLOAT_0xA4, camera.FLOAT_0xA8, camera.FLOAT_0xAC};

  // We read in the rotation matrix from the projection matrix as follows.
  // I don't know what Nintendo was smoking when making this but it makes
  // absolutely no sense.
  // +----+---+----+----+
  // | Z2 | 0 | X0 | X1 |
  // +----+---+----+----+
  // | X2 | 0 | Y0 | Y1 |
  // +----+---+----+----+
  // | Y2 | 0 | Z0 | Z1 |
  // +----+---+----+----+
  // | 0  | 0 | 0  | 0  |
  // +----+---+----+----+
  rotation.x = Vec3{camera.projectionMatrix[2], camera.projectionMatrix[3],
                    camera.projectionMatrix[4]};
  rotation.y = Vec3{camera.projectionMatrix[6], camera.projectionMatrix[7],
                    camera.projectionMatrix[8]};
  rotation.z = Vec3{camera.projectionMatrix[10], camera.projectionMatrix[11],
                    camera.projectionMatrix[0]};

  // This address is right above the camera address (hence the -0x10)
  // It might make more sense to have 2 base addresses in this class then.
  // Also, the game does some weird stuff to get the address here. It first
  // subtracts 0x88 and then adds 0x11c. I decide to keep it like this for now,
  // because I don't fully understand why it does this calculation. It's
  // possible that the -0x88 offset is different for each player?
  auto fovAddress = mPointers.cameraManager2[0x0] + (-0x88 + 0x11c);
  auto fovRes = DolphinInterface::Instance().readType<float>(fovAddress);
  if (!fovRes.has_value()) {
    return;
  }

  fov = fovRes.value();
}
