#pragma once

// 좌표 구조체
struct Position
{
	int x;
	int y;
};

struct PlayerInfo
{
	Position posPlayer;
	bool IsPlayerReady = false;
};

