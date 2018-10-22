#include <WString.h>

class Player {
  private: 
    int life; 
    int bullet;
	  int action; // 0: aucune; 1: tirer; 2:bloquer; 3:recharger
	  int result; // 0 :perdu; 1:continuer; 2:ganger
    int otherAction; // action de l'autre joueur
  public:
    Player(int life = 3, int bullet = 1);
    int getLife();
    int getBullet();
    void resetPlayer(int life, int bullet);
  	bool setAction(int action);
  	int getAction();
    int getOtherAction();
    void setOtherAction(int action);
  	void shoot();
  	void getHit();
  	int getResult();
  	void setResult(int result);
  	void reload();
    bool getresult;
};
