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

bool Map::Update()
{
	CrashCheckFeedAndPlayer();
	return CrashCheckPlayers();
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
		RECT rec = { player[i].position.x - player[i].GetSize() * 0.5, player[i].position.y - player[i].GetSize() * 0.2 , player[i].position.x + player[i].GetSize() * 0.5,player[i].position.y + player[i].GetSize()  };
		DrawText(hdc, buffer, strlen(buffer), &rec, DT_CENTER);
		
		//id draw
		char buffer2[80];
		sprintf(buffer2, "id :%d" , i);
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

bool Map::CrashCheckPlayers()
{
	bool flag = false;
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
				flag = true;
			}
		}
	}

	return flag;
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

mapPack Map::GetPacket()
{
//	str = "";
//	int length = _msize(feed) / sizeof(CircleObject);
//#pragma region  Setting Packet
//	int R, G, B;
//	for (int i = 0; i < length; i++)
//	{
//		//feed[i].ReferenceRGB(R, G, B);
//		str += std::to_string(feed[i].position.x) + ',' + std::to_string(feed[i].position.y) + ',';
//			//+ std::to_string(R) + ',' + std::to_string(G) + ',' + std::to_string(B);
//		if (i < length - 1)
//		{
//			// add split word
//			str += '|';
//		}
//	}
//	// add split word [ info '<' player info ]
//	str +='>';
//	for (int i = 0; i < 4; i++)
//	{
//		str += std::to_string(player[i].GetPosition().x) + ',' +
//			std::to_string(player[i].GetPosition().y) + ',' +
//			std::to_string(player[i].GetScore()) + ',' +
//			std::to_string(player[i].GetSize());
//		
//		if (i < length - 1)
//		{
//			// add split word
//			str += '|';
//		}
//	}
//#pragma endregion
//
////	std::cout << str << std::endl;
//
//	return str.c_str();
	mapPack Pack;
	for (int i = 0; i < 500; ++i)
	{
		Pack.feedX[i] = feed[i].position.x;
		Pack.feedY[i] = feed[i].position.y;
	}
	for (int i = 0; i < 4; ++i)
	{
		Pack.PlayerX[i] = player[i].position.x;
		Pack.PlayerY[i] = player[i].position.y;
 		Pack.PlayerSize[i] = player[i].GetSize();
		Pack.PlayerScore[i] = player[i].GetScore();
	}
	Pack.PlayerNum = PlayerNum;
	return Pack;
}
