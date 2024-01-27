#include "Game.h"

#include "Camera.h"
#include "DolphinAddress.h"
#include "DolphinInterface.h"

GameId getGameId(std::string idString) {
  if (idString == "RMCP01") {
    return GameId::PAL;
    // TODO: other game id's
  } else {
    throw std::exception();
  }
}

std::tuple<DolphinAddress, bool>
PointerCache::getPointer(const DolphinAddress address) {
  auto res = address[0];
  if (!res.isValid()) {
    // If the address is invalid, we don't bother updating the cache.
    return {res, true};
  }

  if (!mCache.contains(address)) {
    mCache[address] = res;
    return {res, true};
  } else if (mCache[address] == address) {
    return {res, false};
  } else {
    mCache[address] = res;
    return {res, true};
  }
}

Game::Game(GameId gameId) : mGameId(gameId) {
  switch (gameId) {
  case GameId::PAL:
    mPointers.raceInfo = 0x9BD730;
    mPointers.playerHolder = 0x9C18F8;
    mPointers.cameraManager = 0x9C19B8;
    mPointers.cameraManager2 = 0x9C19A8;
    mPointers.kartObjectManager = 0x9C18F8;
    break;
  // These need to be filled in still
  case GameId::NTSC_U:
  case GameId::NTSC_J:
  case GameId::NTSC_K:
  default:
    throw std::exception();
  }

  mCache = std::make_shared<PointerCache>();
  player = Player(mPointers, mCache);
  camera = Camera(mPointers, mCache);
}

void Game::update() {
  // player update is causing core dump?
  player.update();
  camera.update();
}

nlohmann::json Game::getJson() { return *this; }
