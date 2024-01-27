#include "Game.h"

#include "Error.h"
#include "LuaInstance.h"

#include <memory>
#include <netinet/in.h>
#include <filesystem>
#include <stop_token>

class Game2;

class Server {
public:
  Server(const int portNr, Game2* game);
  ~Server();

  void startConnection(std::stop_token stopToken);

private:
  int mSocketFd = -1;
  sockaddr_in mServerAddress;
  LuaInstance mLua;

  Game2* mGame;
};
