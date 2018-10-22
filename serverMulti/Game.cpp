#include "Game.h"

Game::Game(){
	//player[0] = new Player(-1, -1, -1)
	player[0] = nullptr;
	player[1] = nullptr;
}

// fonction d'ajout d'un joueur à la partie, return l'id du joueur
int Game::addPlayer(int life, int bullet) {
	if (player[0] == nullptr) {
		player[0] = new Player(life, bullet);
		return 0;
	}
	else if (player[1] == nullptr) {
		player[1] = new Player(life, bullet);
		return 1;
	}
	else {
		return -1;
	}
}

//supprime le joueur
void Game::deletePlayer(int idPlayer){
  delete player[idPlayer];
  player[idPlayer] = nullptr;
}

/: retourne l'addresse du joueur
Player * Game::getPlayer(int id)
{	
  return player[id];
}

//verifie que la partie est pleine
bool Game::isFull()
{
	if ((player[0] != nullptr) && (player[1] != nullptr)) {
		return true;
	}
	else {
		return false;
	}
}

//verifie que la partie est vide
bool Game::isEmpty()
{
	if ((player[0] == nullptr) && (player[1] == nullptr)) {
		return true;
	}
	else {
		return false;
	}
}

//verifie que les deux joueurs sont prêt à commencer une nouvelles partie
bool Game::isReady()
{
  if ((player[0]->getResult() == 1) && (player[1]->getResult() == 1)) {
    return true;
  }
  else {
    return false;
  }
}

//verifie que les deux joueurs on joué
bool Game::turnReady()
{
	if ((player[0]->getAction() != 0) && (player[1]->getAction() != 0)) {
		return true;
	}
	else {
		return false;
	}
}

// calcul le résultat en fonction des actions des joueurs
void Game::calculate()
{
	for (int i = 0; i < 2; i++) {
		if (player[i]->getAction() == 1) {
			player[i]->shoot();
			int other = (i + 1) % 2;
			if (player[other]->getAction() == 3) {
				player[other]->getHit();
				if (player[other]->getLife() == 0) {
					player[other]->setResult(0);
					player[i]->setResult(2);
				}
			}
		}
		if (player[i]->getAction() == 3) {
			player[i]->reload();
		}
	}
}
