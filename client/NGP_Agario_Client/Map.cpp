#include "Map.h"
#include <random>
#include <time.h>

Map::Map()
{
	feed = new CircleObject();
	for (int i = 0; i < Maxfeed; ++i)
	{
		feed[i] = MakeRandomFeed();
	}
}

void Map::Update()
{
}

void Map::Draw(HDC hdc)
{
	for (int i = 0; i < Maxfeed; ++i)
	{
		feed[i].Draw(hdc);
	}
	for (int i = 0; i < PlayerNum; ++i)
	{
		player[i].Draw(hdc);
	}
}

void Map::CrashCheck()
{
}

CircleObject Map::MakeRandomFeed()
{
	std::default_random_engine dre(time(nullptr));
	std::uniform_int_distribution<unsigned int> uid(10,800);
	CircleObject* object = new CircleObject();
	int x = uid(dre);	
	int y = uid(dre);
	Position pos(x, y);
	object->SetPosition(pos);
	object->SetSize(20);
}
