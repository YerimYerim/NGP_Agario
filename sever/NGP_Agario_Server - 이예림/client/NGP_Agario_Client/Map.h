#pragma once
#include "CircleObject.h"
#include "Player.h"
#include <string>
class Map
{
	CircleObject* feed;
	const int Maxfeed = 500;
	

public:
	Map();
	int PlayerNum = 1;
	Player* player;
	bool Update();
	void Draw(HDC hdc);
	void CrashCheckFeedAndPlayer();
	bool CrashCheckPlayers();
	void AddPlayer();
	BOOL GameEnd();
	CircleObject MakeRandomFeed();

	mapPack GetPacket();
};


