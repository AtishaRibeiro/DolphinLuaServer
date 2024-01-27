#pragma once

#include "DolphinAddress.h"
#include "Camera.h"
#include "Player.h"

#include "json.hpp"

#include <string>

enum GameId { PAL, NTSC_U, NTSC_J, NTSC_K, INVALID };

GameId getGameId(std::string idString);

class Game {
public:
  Game(GameId gameId);

  void update();

  nlohmann::json getJson();

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Game, player, camera)
private:
  GameId mGameId;
  Pointers mPointers;
  std::shared_ptr<PointerCache> mCache;

  Player player;
  Camera camera;
};