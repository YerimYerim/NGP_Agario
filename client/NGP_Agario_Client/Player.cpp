#include "Player.h"

Direction Player::input(WPARAM Button)
{
	if (Button == VK_LEFT || Button == VK_RIGHT || Button == VK_UP || Button == VK_DOWN)
	{
		if (GetKeyState(VK_LEFT) < 0) {
			return LEFT;
		}
		if (GetKeyState(VK_RIGHT) < 0) {
			return RIGHT;
		}
		if (GetKeyState(VK_DOWN) < 0) {
			return DOWN;
		}
		if (GetKeyState(VK_UP) < 0) {
			return UP;
		}
	}
}

void Player::move(Direction dir)
{

	switch (dir)
	{
	case UP:
		this->position.moveUp();
		break;
	case DOWN:
		this->position.moveDown();
		break;
	case RIGHT:
		this->position.moveRight();
		break;
	case LEFT:
		this->position.moveLeft();
		break;
	default:
		break;
	}
}
