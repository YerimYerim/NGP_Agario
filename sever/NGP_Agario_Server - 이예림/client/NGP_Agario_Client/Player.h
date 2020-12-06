#pragma once
#include <iostream>
#include "Direction.h"
#include "CircleObject.h"

class Player : public CircleObject
{
	std::string name;
	int score = 0;
public:
	Direction VerticalInput(WPARAM Button);
	Direction HorizontalInput(WPARAM Button);
	void VerticalMove(Direction dir);
	void HorizontalMove(Direction dir);
	void AddScore();
	int GetScore();
	void SetScore(int i);
};

