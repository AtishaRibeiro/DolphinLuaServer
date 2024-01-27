#pragma once

#include "sol/sol.hpp"

#include "json.hpp"

#include <filesystem>
#include <string>

class LuaInstance {
public:
  LuaInstance(){};
  LuaInstance(std::filesystem::path scriptPath, std::string gameId);

  /// Runs the `main` function from the script. The table returned from that
  /// function is converted into a json object.
  nlohmann::json runScript();

  /// Reload the script if it was modified
  void reloadScript();

private:
  /// Attempt to load the script, if there is an error loading `mIsCrashing`
  /// will be set to true.
  void loadScript();
  void registerTypes();
  void registerStaticFuncs();
  nlohmann::json tableToJson(sol::table &&table);

  std::filesystem::path mScriptPath;
  std::filesystem::file_time_type mScriptLastModified;
  sol::state mState;
  std::function<sol::table()> mScriptMain;

  // True if the currently loaded script is crashing
  bool mIsCrashing = false;
};
