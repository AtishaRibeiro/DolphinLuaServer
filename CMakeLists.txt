cmake_minimum_required(VERSION 3.27)

project(DolphinLuaServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
message(STATUS "Using build type ${CMAKE_BUILD_TYPE}")

# Find LuaJit
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(ENV{LUA_DIR} "/usr/local/lib")
find_package(LuaJIT)

include_directories("src")
include_directories(${LUA_INCLUDE_DIR})

file(GLOB CPP_FILES CONFIGURE_DEPENDS
  "src/*.cpp"
)
set(SOURCE ${CPP_FILES})

add_executable(out src/main.cpp ${SOURCE})

add_subdirectory(fmt EXCLUDE_FROM_ALL)
target_link_libraries(out PRIVATE fmt::fmt-header-only ${LUA_LIBRARY})

