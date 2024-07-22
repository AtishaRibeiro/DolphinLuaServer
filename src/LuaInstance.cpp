#include "LuaInstance.h"

#include "CommonTypes.h"
#include "DolphinAddress.h"
#include "DolphinInterface.h"

LuaInstance::LuaInstance(std::filesystem::path scriptPath, std::string gameId) {
  if (!std::filesystem::exists(scriptPath)) {
    std::cerr << "Script does not exist: " << scriptPath << std::endl;
    throw std::exception();
  }
  mScriptPath = scriptPath;
  mScriptLastModified = std::filesystem::last_write_time(mScriptPath);

  mState.open_libraries(sol::lib::base, sol::lib::table);
  mState["GameId"] = gameId;
  registerTypes();
  registerStaticFuncs();

  loadScript();
}

nlohmann::json LuaInstance::runScript() {
  if (mIsCrashing) {
    return {};
  }

  try {
    sol::table res = mScriptMain();
    mIsCrashing = false;
    return tableToJson(std::move(res));
  } catch (const sol::error &e) {
    std::cerr << e.what() << std::endl;
    mIsCrashing = true;
  }
  return {};
}

void LuaInstance::loadScript() {
  try {
    mState.script_file(mScriptPath);
    sol::function solFunction = mState["Main"];
    mScriptMain = solFunction;
    mIsCrashing = false;
  } catch (const sol::error &e) {
    std::cerr << e.what() << std::endl;
    mIsCrashing = true;
  }
}

void LuaInstance::reloadScript() {
  auto writeTime = std::filesystem::last_write_time(mScriptPath);
  if (writeTime != mScriptLastModified) {
    mScriptLastModified = writeTime;
    loadScript();
  }
}

void LuaInstance::registerTypes() {
  // Vec3
  sol::usertype<Vec3> vec3Type =
      mState.new_usertype<Vec3>("Vec3", sol::constructors<Vec3()>());
  vec3Type["x"] = &Vec3::x;
  vec3Type["y"] = &Vec3::y;
  vec3Type["z"] = &Vec3::z;
  vec3Type["__tostring"] = &Vec3::str;

  // Vec4
  sol::usertype<Vec4> vec4Type =
      mState.new_usertype<Vec4>("Vec4", sol::constructors<Vec4()>());
  vec4Type["w"] = &Vec4::w;
  vec4Type["x"] = &Vec4::x;
  vec4Type["y"] = &Vec4::y;
  vec4Type["z"] = &Vec4::z;
  vec4Type["__tostring"] = &Vec4::str;

  // Matrix34
  sol::usertype<Matrix34> matrix34Type = mState.new_usertype<Matrix34>(
      "Matrix34", sol::constructors<Matrix34()>());
  matrix34Type["w"] = &Matrix34::w;
  matrix34Type["x"] = &Matrix34::x;
  matrix34Type["y"] = &Matrix34::y;
  matrix34Type["z"] = &Matrix34::z;
  matrix34Type["__tostring"] = &Matrix34::str;

  // Matrix44
  sol::usertype<Matrix44> matrix44Type = mState.new_usertype<Matrix44>(
      "Matrix44", sol::constructors<Matrix44()>());
  matrix44Type["w"] = &Matrix44::w;
  matrix44Type["x"] = &Matrix44::x;
  matrix44Type["y"] = &Matrix44::y;
  matrix44Type["z"] = &Matrix44::z;
  matrix44Type["__tostring"] = &Matrix44::str;

  // DolphinAddress
  sol::usertype<DolphinAddress> addressType =
      mState.new_usertype<DolphinAddress>(
          "Address",
          sol::constructors<DolphinAddress(), DolphinAddress(u32)>());
  addressType["is_valid"] = &DolphinAddress::isValid;
  addressType["pointer_chain"] = &DolphinAddress::readPointerChain;
  addressType["__tostring"] = &DolphinAddress::getStr;
  addressType["add"] = &DolphinAddress::add;
  addressType["sub"] = &DolphinAddress::sub;
  addressType["eq"] = &DolphinAddress::eq;
  addressType["__add"] = &DolphinAddress::add;
  addressType["__sub"] = &DolphinAddress::sub;
}

void LuaInstance::registerStaticFuncs() {
  mState.set_function("read_u8", DolphinInterface::readTypeLua<u8>);
  mState.set_function("read_u16", DolphinInterface::readTypeLua<u16>);
  mState.set_function("read_u32", DolphinInterface::readTypeLua<u32>);
  mState.set_function("read_u64", DolphinInterface::readTypeLua<u64>);
  mState.set_function("read_float", DolphinInterface::readTypeLua<float>);
  mState.set_function("read_vec3", DolphinInterface::readTypeLua<Vec3>);
  mState.set_function("read_vec4", DolphinInterface::readTypeLua<Vec4>);
  mState.set_function("read_matrix34", DolphinInterface::readTypeLua<Matrix34>);
  mState.set_function("read_matrix44", DolphinInterface::readTypeLua<Matrix44>);
  mState.set_function("read_string", DolphinInterface::readStringLua);

  // Vectors are stored as sol::userdata which doesn't exactly behave like a
  // normal table. This is not obvious from the scripting side, so this function
  // just converts the userdata vector to a regular table.
  mState.script(R"(
      function __convert_vec(vec)
        local ret = {}
        if not vec then
          return ret
        end
        for i = 1, #vec do
          table.insert(ret, vec[i])
        end
        return ret
      end
    )");

  mState.set_function("__read_float_vec",
                      DolphinInterface::readVectorOfTypeLua<float>);
  mState.script(R"(
      function read_float_vec(addr, size)
        return __convert_vec(__read_float_vec(addr, size))
      end
    )");

  mState.set_function("__read_u32_vec",
                      DolphinInterface::readVectorOfTypeLua<u32>);
  mState.script(R"(
      function read_u32_vec(addr, size)
        return __convert_vec(__read_u32_vec(addr, size))
      end
    )");
}

nlohmann::json LuaInstance::tableToJson(sol::table &&table) {
  nlohmann::json json;

  for (auto &some : table) {
    auto key = some.first.as<std::string>();
    auto value = some.second.as<std::string>();
    if (value.empty()) {
      json[key] = tableToJson(some.second.as<sol::table>());
    } else {
      if (some.second.is<double>()) {
        json[key] = some.second.as<double>();
      } else if (some.second.is<int>()) {
        json[key] = some.second.as<int>();
      } else {
        json[key] = value;
      }
    }
  }
  return json;
}
