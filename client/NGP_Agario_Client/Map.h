#pragma once
#include "CircleObject.h"
#include "Player.h"
class Map
{
	CircleObject feed[500];
	Player player[4];
	void Update();
	void Draw();
	void CrashCheck();
};

