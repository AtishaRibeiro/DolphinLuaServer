#include "Player.h"

#include "DolphinAddress.h"
#include "DolphinInterface.h"

#include <fmt/core.h>

void KartPart::update() {
  auto mdlPointer = address[0x7c];
  auto transMatrixAddr = mdlPointer[0x14][0x0] + 0xc;
  auto maybeTransMatrix =
      DolphinInterface::Instance().readType<Matrix34>(transMatrixAddr);
  if (!maybeTransMatrix.has_value()) {
    return;
  }
  // +----+----+----+
  // | X0 | Y0 | Z0 |
  // +----+----+----+
  // | PX | X1 | Y1 |
  // +----+----+----+
  // | Z1 | PY | X2 |
  // +----+----+----+
  // | Y2 | Z2 | PZ |
  // +----+----+----+
  auto transMatrix = maybeTransMatrix.value();
  rotation.x = Vec3{transMatrix.w.x, transMatrix.x.y, transMatrix.y.z};
  rotation.y = Vec3{transMatrix.w.y, transMatrix.x.z, transMatrix.z.x};
  rotation.z = Vec3{transMatrix.w.z, transMatrix.y.x, transMatrix.z.y};
  position = Vec3{transMatrix.x.x, transMatrix.y.y, transMatrix.z.z};
}

// RaceScene_calcSubsystemsUnpaused
// kartObjectManager: 809c18f8[0]

// calc 8058ffe8
// r3: kartObjectManager
// objectPtr: r3[20][0]

// calcSub
// r3: objectPtr
// Kartsub: r3[10]

// calcPass0 80596480
// bl 805967d0
// r29: KartSub
// Kartmove = r29[10]

// getPhysics 805903cc
// KartMove: 80e50df8

// physics = KartMove[0][8][90]

// TOTAL:
// 809c18f8[0][20][0][10][10][0][8][90]

void Player::update() {
  DolphinAddress playerAddress;
  bool addressIsNew;
  std::tie(playerAddress, addressIsNew) =
      mPointerCache->getPointer(mPointers.playerHolder);

  DolphinAddress kartPointerAddr;
  DolphinAddress kartSettings;
  DolphinAddress kartSub;
  if (addressIsNew) {
    mPositionAddress = playerAddress[0x20][0x0][0x0][0x8][0x90][0x4] + 0x68;
    mPhysicsAddress = playerAddress[0x20][0x0][0x10][0x10][0x0][0x8][0x90];
    kartSub = playerAddress[0x20][0x0][0x10];
    kartPointerAddr = kartSub[0x0];
    kartSettings = kartPointerAddr[0x0];
    updateKartPointers(playerAddress);
    setKartParts(mKartPointers.KartBody);
  }

  if (!mPositionAddress.isValid() || !mPhysicsAddress.isValid()) {
    return;
  }

  for (auto &part : kartParts) {
    part.update();
  }

  // // Tire count
  // auto susRes = DolphinInterface::Instance().readType<u16>(
  //     mKartPointers.KartSettings + 0xc);
  // auto tireRes = DolphinInterface::Instance().readType<u16>(
  //     mKartPointers.KartSettings + 0xe);
  // if (!susRes.has_value() || !tireRes.has_value()) {
  //   return;
  // }

  // // // Iterate over sus
  // // for (int i = 0; i < susRes.value(); ++i) {
  // //   auto susAddr = mKartPointers.KartSuspensions[i * 0x4];
  // //   auto mdlPointer = susAddr[0x7c];
  // //   std::cout << susAddr.getStr() << std::endl;
  // //   std::cout << getModelName(mdlPointer) << std::endl;
  // // }

  // auto tireNr = tireRes.value();
  // if (tireNr != tires.size()) {
  //   tires.resize(tireNr);
  // }

  // // Iterate over tires
  // for (int i = 0; i < tireNr; ++i) {
  //   auto tireAddr = mKartPointers.KartTires[i * 0x4];
  //   // std::cout << tireAddr.getStr() << std::endl;
  //   updateTireRotation(i, tireAddr);
  // }

  // Position
  auto res = DolphinInterface::Instance().readType<Vec3>(mPositionAddress);
  if (!res.has_value()) {
    return;
  }
  position = res.value();

  updatePlayerRotation();
}

void Player::updateKartPointers(const DolphinAddress playerAddress) {
  auto kartPtrsAddr = playerAddress[0x20][0x0][0x10][0x0];

  // clang-format off
  mKartPointers = {
      kartPtrsAddr[0x0],
      kartPtrsAddr[0x4],
      kartPtrsAddr[0x8],
      kartPtrsAddr[0xC],
      kartPtrsAddr[0x10],
      kartPtrsAddr[0x14],
      kartPtrsAddr[0x18],
      kartPtrsAddr[0x1C],
      kartPtrsAddr[0x20],
      kartPtrsAddr[0x24],
      kartPtrsAddr[0x28],
      kartPtrsAddr[0x2C],
      kartPtrsAddr[0x30],
  };
  // clang-format on

  std::cout << mKartPointers.PlayerModel.getStr() << std::endl;
}

void Player::updatePlayerRotation() {
  auto vec1 =
      DolphinInterface::Instance().readType<Vec3>(mPhysicsAddress + 0x9C);
  auto vec2 =
      DolphinInterface::Instance().readType<Vec3>(mPhysicsAddress + 0xa8);
  auto vec3 =
      DolphinInterface::Instance().readType<Vec3>(mPhysicsAddress + 0xb4);
  auto vec4 =
      DolphinInterface::Instance().readType<Vec3>(mPhysicsAddress + 0xc0);
  if (!vec1.has_value() || !vec2.has_value() || !vec3.has_value() ||
      !vec4.has_value()) {
    return;
  }

  // The @ denotes player position, maybe we should read that in as well?
  // +----+----+----+
  // | X0 | Y0 | Z0 |
  // +----+----+----+
  // | @  | X1 | Y1 |
  // +----+----+----+
  // | Z1 | @  | X2 |
  // +----+----+----+
  // | Y2 | Z2 | @  |
  // +----+----+----+
  rotation.x = Vec3{vec1.value().x, vec2.value().y, vec3.value().z};
  rotation.y = Vec3{vec1.value().y, vec2.value().z, vec4.value().x};
  rotation.z = Vec3{vec1.value().z, vec3.value().x, vec4.value().y};
}

void Player::updateTireRotation(const int index, const DolphinAddress address) {
  // auto kartPartAddress = address + 0x1c;
  auto mdlPointer = address[0x7c];
  // std::cout << getModelName(mdlPointer) << std::endl;
  // std::cout << mdlPointer.getStr() << std::endl;
  auto kartPartAddress = mdlPointer[0x14][0x0] + 0xc;
  auto vec1 = DolphinInterface::Instance().readType<Vec3>(kartPartAddress);
  auto vec2 =
      DolphinInterface::Instance().readType<Vec3>(kartPartAddress + 0xC);
  auto vec3 =
      DolphinInterface::Instance().readType<Vec3>(kartPartAddress + 0x18);
  auto vec4 =
      DolphinInterface::Instance().readType<Vec3>(kartPartAddress + 0x24);
  if (!vec1.has_value() || !vec2.has_value() || !vec3.has_value() ||
      !vec4.has_value()) {
    return;
  }

  // The @ denotes player position, maybe we should read that in as well?
  // +----+----+----+
  // | X0 | Y0 | Z0 |
  // +----+----+----+
  // | @  | X1 | Y1 |
  // +----+----+----+
  // | Z1 | @  | X2 |
  // +----+----+----+
  // | Y2 | Z2 | @  |
  // +----+----+----+
  tires[index].rotation.x =
      Vec3{vec1.value().x, vec2.value().y, vec3.value().z};
  tires[index].rotation.y =
      Vec3{vec1.value().y, vec2.value().z, vec4.value().x};
  tires[index].rotation.z =
      Vec3{vec1.value().z, vec3.value().x, vec4.value().y};

  // // access realpos in wheelphysics
  auto res = DolphinInterface::Instance().readType<Vec3>(address[0x98] +
  0x20); if (!res.has_value()) {

    std::cout << "tire position invalid" << std::endl;
    return;
  }
  tires[index].position = res.value();
  // tires[index].position = Vec3{vec2.value().x, vec3.value().y, vec4.value().z};
}

/// Based on https://wiki.tockdom.com/wiki/MDL0_(File_Format)
std::string Player::getModelName(const DolphinAddress mdlPointer) {
  auto header = mdlPointer[0xC];

  // Verify header magic
  auto magicRes = DolphinInterface::Instance().readString(header, 4);
  if (magicRes.isError() || magicRes.getValue() != "MDL0") {
    return "";
  }

  auto brresVersion = DolphinInterface::Instance().readType<u32>(header + 0x8);
  if (!brresVersion.has_value()) {
    return "";
  }
  auto nrSections = brresVersion.value() == 11 ? 14 : 11;

  auto nameOffset = DolphinInterface::Instance().readType<u32>(
      header + 0x10 + nrSections * 0x4);
  if (!nameOffset.has_value()) {
    return "";
  }
  auto nameAddr = header + nameOffset.value();
  // The size of the string is one word ahead of the string
  auto nameSize = DolphinInterface::Instance().readType<u32>(nameAddr - 0x4);
  if (!nameSize.has_value()) {
    return "";
  }

  // std::cout << mdlPointer.getStr() << std::endl;
  // std::cout << nameAddr.getStr() << std::endl;
  // std::cout << nameOffset.value() << std::endl;
  auto nameRes =
      DolphinInterface::Instance().readString(nameAddr, nameSize.value());
  if (nameRes.isError()) {
    return "";
  }
  return nameRes.getValue();
}

void Player::setKartParts(const DolphinAddress startPart) {
  kartParts.clear();

  auto currentAddress = startPart;
  KartPart currentPart;

  // Go forwards
  while (currentAddress != 0) {
    currentPart.modelName = getModelName(currentAddress[0x7C]);
    if (!currentPart.modelName.empty()) {
      currentPart.address = currentAddress;
      kartParts.push_back(currentPart);
    }
    // Get the next from Link
    currentAddress = currentAddress[0x8];
  }

  // Go backwards
  currentAddress = startPart[0x4];
  while (currentAddress != 0) {
    currentPart.modelName = getModelName(currentAddress[0x7C]);
    if (!currentPart.modelName.empty()) {
      currentPart.address = currentAddress;
      kartParts.push_back(currentPart);
    }
    // Get the prev from Link
    currentAddress = currentAddress[0x4];
  }
}
