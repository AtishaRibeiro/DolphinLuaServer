#include "Game.h"
#include "Server.h"

#include <iostream>
#include <thread>

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
  std::string script = "UKNOWN SCRIPT";
};

Arguments parseArguments(int argc, char *argv[]) {
  Arguments args;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "-h" || arg == "--help") {
      std::cout << "usage: <something> [options]\n\n"
                   "-p, --port <number>\t\tport used by the server (9999 if "
                   "not specified)\n"
                   "-d, --dump <file>\t\tjson file to dump the data to\n"
                   "-s, --script <file>\t\tlua file"
                << std::endl;
      exit(0);
    } else if (arg == "-p" || arg == "--port") {
      args.port = atoi(argv[++i]);
    } else if (arg == "-d" || arg == "--dump") {
      args.dump = argv[++i];
    } else if (arg == "-s" || arg == "--script") {
      args.script = argv[++i];
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
    auto game = Game();
    std::jthread gameThread(&Game::startScriptLoop, &game, args.script,
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
