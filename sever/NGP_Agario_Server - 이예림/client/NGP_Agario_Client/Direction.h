#pragma once

enum Direction
{
	UP, DOWN, RIGHT, LEFT
};

struct SendDirection {
	unsigned short id;
	Direction dir;
};