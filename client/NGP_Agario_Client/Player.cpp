#include "Player.h"

Direction Player::VerticalInput(WPARAM Button)
{
	if (GetAsyncKeyState(VK_DOWN) < 0) {
		return DOWN;
	}
	if (GetAsyncKeyState(VK_UP) < 0) {
		return UP;
	}

}
Direction Player::HorizontalInput(WPARAM Button)
{

	if (GetAsyncKeyState(VK_LEFT) < 0) {
		return LEFT;
	}
	if (GetAsyncKeyState(VK_RIGHT) < 0) {
		return RIGHT;
	}

	
}

void Player::VerticalMove(Direction dir)
{
	switch (dir)
	{
	case UP:
		this->position.moveUp();
		break;
	case DOWN:
		this->position.moveDown();
		break;
	}
}

void Player::HorizontalMove(Direction dir)
{
	switch (dir)
	{
	case RIGHT:
		this->position.moveRight();
		break;
	case LEFT:
		this->position.moveLeft();
		break;
	}
}

