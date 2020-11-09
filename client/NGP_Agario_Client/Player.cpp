#include "Player.h"

Direction Player::input(WPARAM Button)
{
	switch (Button)
	{
	case VK_LEFT:
		return LEFT;
		break;
	case VK_RIGHT:
		return RIGHT;
		break;
	case VK_UP:
		return UP;
		break;
	case VK_DOWN:
		return DOWN;
		break;
	default:
		break;
	}
}
