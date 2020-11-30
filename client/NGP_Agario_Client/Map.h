#pragma once
#include "CircleObject.h"
#include "Player.h"
class Map
{
	CircleObject* feed;
	const int Maxfeed = 500;

public:

	Map();
	int PlayerNum = 1;
	Player* player;
	CircleObject* GetFeed();

	void Update();
	void Draw(HDC hdc);
	void CrashCheckFeedAndPlayer();
	void CrashCheckPlayers();
	void AddPlayer();
	BOOL GameEnd();
	CircleObject MakeRandomFeed();

	void Set(char* packet);
};


