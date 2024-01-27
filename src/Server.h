#include <netinet/in.h>
#include <stop_token>

class Game;

class Server {
public:
  Server(const int portNr, Game *game);
  ~Server();

  void startConnection(std::stop_token stopToken);

private:
  int mSocketFd = -1;
  sockaddr_in mServerAddress;

  Game *mGame;
};
