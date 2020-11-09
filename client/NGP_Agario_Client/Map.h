#pragma once
#include "CircleObject.h"
#include "Player.h"
class Map
{
	CircleObject* feed;
	const int Maxfeed = 500;
	Player* player;
	int PlayerNum = 1;
public:
	Map();
	void Update();
	void Draw(HDC hdc);
	void CrashCheck();
	CircleObject MakeRandomFeed();
};

