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

	player = new Player[4];
	for (int i = 0; i < PlayerNum; ++i)
	{
		player[i].SetRandomPosition();
		player->SetSize(50);
	}
	AddPlayer();
}

void Map::Update()
{
	CrashCheckFeedAndPlayer();
	CrashCheckPlayers();
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

void Map::CrashCheckFeedAndPlayer()
{
	for (int i = 0; i < Maxfeed; ++i)
	{
		for (int j = 0; j < PlayerNum; ++j)
		{
			if (feed[i].CrashCheck(player[j]))
			{
				feed[i] = MakeRandomFeed();
				feed[i].SetSize(10);
				player[j].AddScore();
			}
		}
	}
}

void Map::CrashCheckPlayers()
{
	for (int i = 0; i < PlayerNum; ++i)
	{
		for (int j = i; j < PlayerNum; ++j)
		{
			if (player[i].CrashCheck(player[j]))
			{
				if (player[i].GetSize() > player[j].GetSize())
				{
					int score = player[i].GetScore() + player[j].GetScore();
					player[i].SetScore(score);
					player[j].SetRandomPosition();
					player[j].SetSize(50);
					player[j].SetScore(0);
				
				}
				if (player[i].GetSize() < player[j].GetSize())
				{
					int score = player[j].GetScore() + player[i].GetScore();
					player[j].SetScore(score);
					player[i].SetRandomPosition();
					player[i].SetSize(50);
					player[i].SetScore(0);
				}
			}
		}
	}
}

void Map::AddPlayer()
{
	player[PlayerNum].SetRandomPosition();
	player[PlayerNum].SetSize(50);
	PlayerNum++;
}

CircleObject Map::MakeRandomFeed()
{
	CircleObject* object = new CircleObject();
	object->SetRandomPosition();
	object->SetSize(10);
	return *object;
}
