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

CircleObject* Map::GetFeed()
{
	return this->feed;
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
		char buffer[80];
		player[i].Draw(hdc);

		// score draw
		sprintf(buffer, "score\n %d", player[i].GetScore());
		SetBkMode(hdc, TRANSPARENT);
		RECT rec = { player[i].position.x - player[i].GetSize() * 0.5, player[i].position.y - player[i].GetSize() * 0.2 , player[i].position.x + player[i].GetSize() * 0.5,player[i].position.y + player[i].GetSize() };
		DrawText(hdc, buffer, strlen(buffer), &rec, DT_CENTER);

		//id draw
		char buffer2[80];
		sprintf(buffer2, "id :%d", i);
		RECT rec2 = { player[i].position.x - player[i].GetSize()  , player[i].position.y + player[i].GetSize() * 0.5 , player[i].position.x + player[i].GetSize(), player[i].position.y + player[i].GetSize() * 1.5 };
		DrawText(hdc, buffer2, strlen(buffer2), &rec2, DT_CENTER | DT_VCENTER);
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

void Map::Set(char* packet)
{
	std::string _packet = packet;
	
	auto Split = [](std::string& packetData, const char splitword)
	{
		int pos = packetData.find(splitword);
		if (pos == std::string::npos) return std::string("");

		std::string token = packetData.substr(0, pos);
		packetData.erase(0, pos + sizeof(splitword));

		return token;
	};
	
	// 앞의 정보 ( 좌표, 색상 )
	std::string front = Split(_packet, '>');
	//printf("_packet? %s\n", _packet.c_str());
	//printf("front? %s\n", front.c_str());
	std::string info = "";
	int count = 0;
	while (true)
	{
		info = Split(front, '|');
		if (info.empty())
		{
			break;
		}

		int x = atoi(Split(info, ',').c_str());
		int y = atoi(Split(info, ',').c_str());
		//int r = atoi(Split(info, ',').c_str());
		//int g = atoi(Split(info, ',').c_str());
		//int b = atoi(info.c_str());

		Position position = Position(x, y);
		this->feed[count].SetPosition(position);
		//this->feed[count].SetRGB(r, g, b);
		//printf("[Log] %d, %d, %d, %d, %d|", x, y, r, g, b);
		count++;
	}

	std::string back = _packet;
	count = 0;
	while (true)
	{
		info = Split(back, '|');
		if (info.empty()) break;

		int x = atoi(Split(info, ',').c_str());
		int y = atoi(Split(info, ',').c_str());
		int score = atoi(Split(info, ',').c_str());
		int size = atoi(info.c_str());

		Position position = Position(x, y);
		this->player[count].SetPosition(position);
		this->player[count].SetScore(score);
		this->player[count].SetSize(size);
		count++;
	}
}
