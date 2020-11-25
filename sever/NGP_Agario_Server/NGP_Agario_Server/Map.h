#pragma once
#include "CircleObject.h"
#include "Player.h"
class Map
{
	CircleObject* feed;
	const int Maxfeed = 500;
	int PlayerNum = 1;

public:
	Map();
	Player* player;
	void Update();
	void Draw(HDC hdc);
	void CrashCheckFeedAndPlayer();
	void CrashCheckPlayers();
	void AddPlayer();
	BOOL GameEnd();
	CircleObject MakeRandomFeed();

};


