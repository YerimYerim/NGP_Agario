#pragma once
#include "CircleObject.h"
#include "Direction.h"
#include <iostream>
class Player : public CircleObject
{
	std::string name;
	int score;
	Direction input();
};

