#include "CommonTypes.h"
#include "DolphinAddress.h"
#include "LuaInstance.h"
#include "json.hpp"

#include <filesystem>
#include <mutex>
#include <stop_token>
#include <vector>

class Game2 {
public:
  Game2();

  nlohmann::json getData();

  void startScriptLoop(std::stop_token stopToken,
                       std::filesystem::path scriptPath,
                       std::optional<std::filesystem::path> jsonDumpPath);

private:
  // Loop that waits until the next frame has begun
  int waitUntilNextFrame(std::stop_token stopToken);

  LuaInstance mLua;
  nlohmann::json mCurrentData;
  std::vector<nlohmann::json> mRecordedData;
  std::mutex mDataMutex;

  DolphinAddress mFrameAddress;
  u32 mCurrentFrame;
};
