#include "Server.h"

#include "CommonTypes.h"
#include "DolphinAddress.h"
#include "DolphinInterface.h"
#include "Error.h"
#include "Game.h"
#include "Game2.h"
#include "json.hpp"
#include "sol/sol.hpp"

#include <arpa/inet.h>
#include <chrono>
#include <fmt/core.h>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

Server::Server(const int portNr, Game2 *game) : mGame(game) {
  mSocketFd =
      socket(/* IPv4 */ AF_INET, /* UDP */ SOCK_DGRAM, /* protocol */ 0);
  if (mSocketFd < 0) {
    throw std::runtime_error("Error creating socket.");
  }

  mServerAddress.sin_family = AF_INET;
  mServerAddress.sin_addr.s_addr = INADDR_ANY;
  mServerAddress.sin_port = htons(portNr);

  if (bind(mSocketFd, (struct sockaddr *)&mServerAddress,
           sizeof(mServerAddress)) < 0) {
    close(mSocketFd);
    throw std::runtime_error("Error binding socket.");
  }
}

Server::~Server() { close(mSocketFd); }

void Server::startConnection(std::stop_token stopToken) {
  sockaddr_in clientAddress;
  socklen_t len = sizeof(clientAddress);
  while (!stopToken.stop_requested()) {
    // Make call non-blocking, so we can exit when a stop is requested
    int flags = MSG_DONTWAIT;
    char buffer[256];
    auto result = recvfrom(mSocketFd, buffer, 255, flags, (sockaddr *)&clientAddress, &len);
    
    // It doesn't matter what we receive exactly, just that we receive
    // *something*
    if (result != -1) {
    // Get the current game state and send it back
    std::string jsonStr = mGame->getData().dump();
    sendto(mSocketFd, jsonStr.c_str(), jsonStr.size(), 0,
           (sockaddr *)&clientAddress, len);
      
    }
  }
}
