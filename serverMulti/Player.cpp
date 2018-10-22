#include "Player.h"

Player::Player(int life, int bullet) {
  this->life = life;
  this->bullet = bullet;
  action = 0;
  result = 1;
  otherAction = 0;
  getresult = false;
}

int Player::getLife()
{
	return life;
}

int Player::getBullet()
{
	return bullet;
}

int Player::getOtherAction() {
  return otherAction;
}

void Player::setOtherAction(int action) {
  otherAction = action;
}

void Player::resetPlayer(int life, int bullet)
{
  this->life = life;
  this->bullet = bullet;
	action = 0;
  result = 1;
  otherAction = 0;
  getresult = false;
}

bool Player::setAction(int action)
{
	if (action == 1) {
		if (bullet >= 1) {
			this->action = action;
			return true;
		}
		else {
			return false;
		}
	}
	this->action = action;
	return true;
}

int Player::getAction()
{
	return action;
}

void Player::shoot()
{
	bullet--;
}

void Player::getHit()
{
	life--;
}

int Player::getResult()
{
	return result;
}

void Player::setResult(int result)
{
	this->result = result;
}

void Player::reload()
{
	bullet++;
}
