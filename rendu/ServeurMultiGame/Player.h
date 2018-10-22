#include <WString.h>

class Player {
  private: 
    int life;
    int bullet;
	  int action;
	  int result;
    int otherAction;
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
