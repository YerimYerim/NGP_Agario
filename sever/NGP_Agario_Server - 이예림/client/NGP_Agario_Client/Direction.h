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