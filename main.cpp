#include "DolphinInterface.h"
#include "Game2.h"
#include "GameObject.h"
#include "Server.h"

#include <arpa/inet.h>
#include <chrono>
#include <fmt/core.h>
#include <iostream>
#include <netinet/in.h>
#include <optional>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

using namespace std::chrono;

// Will be set to 1 if the program was killed (Ctrl-C)
// This allows us to exit gracefully.
static volatile int should_abort = 0;

namespace {
void signal_handler(int sig_code) {
  if (sig_code == SIGINT) {
    should_abort = 1;
  }
}
} // namespace

struct Arguments {
  int port = 9999;
  std::optional<std::string> dump;
};

Arguments parseArguments(int argc, char *argv[]) {
  Arguments args;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "-h" || arg == "--help") {
      std::cout << "usage: <something> [options]\n\n"
                   "-p, --port <number>\t\tport used by the server (9999 if "
                   "not specified)\n"
                   "-d, --dump <file>\t\tjson file to dump the data to"
                << std::endl;
      exit(0);
    } else if (arg == "-p" || arg == "--port") {
      args.port = atoi(argv[i++]);
    } else if (arg == "-d" || arg == "--dump") {
      args.dump = argv[i++];
    } else {
      std::cerr << "Unrecognised option '" << arg << "'" << std::endl;
      exit(1);
    }
  }
  return args;
}

int main(int argc, char *argv[]) {
  auto args = parseArguments(argc, argv);

  // Register Control-C signal handler
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = signal_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  try {
    // Gamestate thread (this reads data from Dolphin)
    auto game = Game2();
    auto scriptPath =
        "/home/atisha/Documents/MKW/live_values/source/scripts/test.lua";
    std::jthread gameThread(&Game2::startScriptLoop, &game, scriptPath,
                            args.dump);

    // Server thread
    auto server = Server(args.port, &game);
    std::jthread serverThread(&Server::startConnection, &server);

    while (should_abort == 0) {
      // Let threads run happily
    }

    gameThread.request_stop();
    serverThread.request_stop();

  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
