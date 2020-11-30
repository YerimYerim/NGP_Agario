#pragma once

enum Direction
{
	UP, DOWN, RIGHT, LEFT
};

struct RecvDirection {
	unsigned short id;
	Direction dir;
};

