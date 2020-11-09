#include "Position.h"
#include <iostream>

void Position::SetPosition(int X, int Y)
{
	x = X;
	y = Y;
}

Position Position::GetPosition()
{
	return *this;
}

void Position::moveRight()
{
	x += 10;
}

void Position::moveLeft()
{
	x -= 10;
}

void Position::moveUp()
{
	y -= 10;
}

void Position::moveDown()
{
	y += 10;
}


