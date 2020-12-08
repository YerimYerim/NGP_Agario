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

	int PlayerX[4];
	int PlayerY[4];
	int PlayerSize[4];
	int PlayerScore[4];

	int PlayerNum = 0;
};