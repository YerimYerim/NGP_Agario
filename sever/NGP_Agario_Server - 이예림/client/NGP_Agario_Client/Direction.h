#pragma once

enum Direction
{
	UP, DOWN, RIGHT, LEFT
};

struct SendDirection {
	unsigned short id;
	int x;
	int y;
	//Direction dir;
};
struct mapPack
{
	int feedX[500];
	int feedY[500];

	int PlayerX[2];
	int PlayerY[2];
	int PlayerSize[2];

	int PlayerScore[2];
};