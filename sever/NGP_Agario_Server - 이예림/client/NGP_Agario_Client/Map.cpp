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
	//AddPlayer();
}

void Map::Update()
{
	if (GameEnd())
	{
		exit(1);
	}
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

BOOL Map::GameEnd()
{
	for (int i = 0; i < PlayerNum; ++i)
	{
		if (player[i].GetSize() > 300)
		{
			return true;
		}
	}
	return false;
}

CircleObject Map::MakeRandomFeed()
{
	CircleObject* object = new CircleObject();
	object->SetRandomPosition();
	object->SetSize(10);
	return *object;
}

const char* Map::GetPacket(std::string& str)
{
	str = "";
	int length = _msize(feed) / sizeof(CircleObject);
#pragma region  Setting Packet
	int R, G, B;
	for (int i = 0; i < length; i++)
	{
		feed[i].ReferenceRGB(R, G, B);
		str += std::to_string(feed[i].position.x) + ',' + std::to_string(feed[i].position.y) +','
			+ std::to_string(R) + ',' + std::to_string(G) + ',' + std::to_string(B);
		if (i < length - 1)
		{
			// add split word
			str += '|';
		}
	}
	// add split word [ info '<' player info ]
	str +='>';
	for (int i = 0; i < 4; i++)
	{
		str += std::to_string(player[i].GetPosition().x) + ',' +
			std::to_string(player[i].GetPosition().y) + ',' +
			std::to_string(player[i].GetScore()) + ',' +
			std::to_string(player[i].GetSize());
		
		if (i < length - 1)
		{
			// add split word
			str += '|';
		}
	}
#pragma endregion

//	std::cout << str << std::endl;

	return str.c_str();
}
