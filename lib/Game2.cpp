#include "Game2.h"

#include "DolphinInterface.h"

#include <chrono>
#include <cstdlib>
#include <fstream>

using namespace std::chrono;

Game2::Game2() {
  if (auto hookRes = DolphinInterface::Instance().hook(); hookRes.isError()) {
    throw std::runtime_error(
        "Failed to hook. Make sure that Dolphin is running.");
  }

  mFrameAddress = DolphinAddress(0x80386be4);
}

nlohmann::json Game2::getData() {
  auto guard = std::lock_guard<std::mutex>(mDataMutex);
  return mCurrentData;
}

void Game2::startScriptLoop(std::stop_token stopToken,
                            std::filesystem::path scriptPath,
                            std::optional<std::filesystem::path> jsonDumpPath) {
  mLua = LuaInstance(scriptPath,
                     DolphinInterface::Instance().readGameID().getValue());

  while (true) {
    // Wait until the next frame so all our reads happen araound the same
    // time
    auto curFrame = waitUntilNextFrame(stopToken);

    // Reload the script if modified
    mLua.reloadScript();
    auto watchStart = high_resolution_clock::now();
    auto data = mLua.runScript();
    auto diff = high_resolution_clock::now() - watchStart;
    auto castDiff = duration_cast<microseconds>(diff).count();
    std::cout << castDiff << "μs" << std::endl;

    auto guard = std::lock_guard<std::mutex>(mDataMutex);
    mCurrentData = {};
    mCurrentData["data"] = data;
    mCurrentData["misc"]["time"] = castDiff;
    mCurrentData["misc"]["frame"] = curFrame;

    if (jsonDumpPath.has_value()) {
      mRecordedData.push_back(mCurrentData);

      // Write collected data to json when an abort signal is detected
      if (stopToken.stop_requested()) {
        nlohmann::json dumpJson = mRecordedData;

        std::ofstream jsonFile;
        jsonFile.open(jsonDumpPath.value());
        jsonFile << dumpJson.dump();
        jsonFile.close();

        std::cout << "Written to " << jsonDumpPath.value() << std::endl;
        break;
      }
    } else if (stopToken.stop_requested()) {
      break;
    }
  }
}

int Game2::waitUntilNextFrame(std::stop_token stopToken) {
  std::optional<u32> maybeFrameNr;
  do {
    if (stopToken.stop_requested()) {
      return 0;
    }

    maybeFrameNr = DolphinInterface::Instance().readType<u32>(mFrameAddress);
    if (!maybeFrameNr.has_value()) {
      throw std::runtime_error("Could not read frame address.");
    }
  } while (maybeFrameNr.value() == mCurrentFrame);
  mCurrentFrame = maybeFrameNr.value();
  return 0;
}
