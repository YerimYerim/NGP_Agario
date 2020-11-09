#pragma once
#include "CircleObject.h"
#include "Direction.h"
#include <iostream>
class Player : public CircleObject
{
	std::string name;
	int score = 0;
public:
	Direction VerticalInput(WPARAM Button);
	Direction HorizontalInput(WPARAM Button);
	void VerticalMove(Direction dir);
	void HorizontalMove(Direction dir);
	void GetScore();
};

