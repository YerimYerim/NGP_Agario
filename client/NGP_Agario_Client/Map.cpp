#include "Map.h"
#include <random>
#include <time.h>

Map::Map()
{
	feed = new CircleObject[Maxfeed];
	for (int i = 0; i < Maxfeed; ++i)
	{
		feed[i] = MakeRandomFeed();
	}

	player = new Player[PlayerNum];
	for (int i = 0; i < PlayerNum; ++i)
	{
		player[i].SetRandomPosition();
		player->SetSize(50);
	}
}

void Map::Update()
{
}

void Map::Draw(HDC hdc)
{
	HBRUSH Brush, oBrush;
	Brush = CreateSolidBrush(RGB(0, 200, 0));
	oBrush = (HBRUSH)SelectObject(hdc, Brush);

	for (int i = 0; i < Maxfeed; ++i)
	{
		feed[i].Draw(hdc);
	}

	SelectObject(hdc, oBrush);
	DeleteObject(Brush);

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
	CircleObject* object = new CircleObject();
	object->SetRandomPosition();
	object->SetSize(4);
	return *object;
}
