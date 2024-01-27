#include "DolphinInterface.h"

#include "DolphinAddress.h"
#include "util.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sys/uio.h>

namespace fs = std::filesystem;

void DolphinInterface::printInfo() {
  std::cout << "pid: " << mPid << std::endl;
  std::cout << "base_address: " << mRamAddress << std::endl;
}

Result DolphinInterface::hook() {
  auto proc = fs::path("/proc");
  for (auto const &dirEntry : fs::directory_iterator{proc}) {
    auto comm = dirEntry / fs::path("comm");
    if (fs::exists(comm)) {
      auto commFile = std::ifstream(comm);
      std::string processName;
      std::getline(commFile, processName);

      if (processName == "dolphin-emu") {
        mPid = std::stoi(dirEntry.path().filename());

        if (auto res = findRamAddress(); res.isError()) {
          mHooked = false;
          return res;
        }
        mHooked = true;
        return {};
      }
    }
  }
  return Error("Failed to find Dolphin process");
}

Expected<std::string> DolphinInterface::readGameID() {
  return readString(0x0, 6);
}

Result DolphinInterface::readFromRam(const DolphinAddress address, char *buffer,
                                     const std::size_t size) const {
  if (!mHooked) {
    return Error("Not hooked to Dolphin yet!");
  }

  if (!address.isValid()) {
    return Error("Reading invalid address");
  }

  struct iovec local, remote;
  local.iov_base = buffer;
  local.iov_len = size;
  remote.iov_base = (void *)(mRamAddress + address.getNormalised());
  remote.iov_len = size;
  auto result = process_vm_readv(mPid, &local,
                                 /*local iov count*/ 1, &remote,
                                 /*remote iov count*/ 1,
                                 /*flags*/ 0);

  if (result != size) {
    return Error("Failed to read from address " + address.getStr());
  }

  return {};
}

DolphinAddress
DolphinInterface::readPointer(const DolphinAddress address) const {
  auto maybeValue = readType<u32>(address);
  if (!maybeValue.has_value()) {
    return DolphinAddress();
  }
  return DolphinAddress(maybeValue.value());
}

Expected<std::string>
DolphinInterface::readString(const DolphinAddress address,
                             const std::size_t size) const {
  char buffer[size];
  if (auto res = readFromRam(address, buffer, size); res.isError()) {
    return res.getError();
  }

  return std::string(buffer, size);
}

Result DolphinInterface::findRamAddress() {
  auto mapsPath = fs::path("/proc") / std::to_string(mPid) / "maps";
  auto mapsFile = std::ifstream(mapsPath);
  for (std::string line; std::getline(mapsFile, line);) {

    if (auto strFound = line.find("/dev/shm/dolphin-emu");
        strFound != std::string::npos) {
      // Addresses are of form 7f2460000000-7f2462000000
      auto firstAddress = std::stoul(line.substr(0, 12), nullptr, 16);
      auto secondAddress = std::stoul(line.substr(13, 12), nullptr, 16);

      if (secondAddress - firstAddress == 0x2000000) {
        mRamAddress = firstAddress;
        return {};
      }
    }
  }
  return Error("Failed to find Dolphin RAM address");
}

void DolphinInterface::reverseBufferBytes(char *buffer, const size_t bufferSize,
                                          const size_t wordSize) const {
  for (int i = 0; i <= bufferSize; i += wordSize) {
    std::reverse(buffer + i, buffer + i + wordSize);
  }
}
