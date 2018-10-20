#include "Player.h"

class Game {
  private : 
    Player * player[2];
  public:
    Game();
    int addPlayer(int life = 3, int bullet = 1);
	Player * getPlayer(int id);
	bool isFull();
	bool turnReady();
	void calculate();
  void deletePlayer(int idPlayer);
};
