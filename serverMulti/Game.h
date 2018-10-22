#include "Player.h"

class Game {
  private : 
    Player * player[2]; //les deux joueurs
  public:
    Game();
    int addPlayer(int life = 3, int bullet = 1);
	Player * getPlayer(int id);
	bool isFull();
	bool isEmpty();
	bool turnReady();
  bool isReady();
	void calculate();
  void deletePlayer(int idPlayer);
};
