#pragma once
class Position
{
public:
	int x;
	int y;
	Position() { x = -10; y = -10; };
	Position(int X, int Y) { x = X; y = Y; };
	void SetPosition(int X ,int Y);
	Position GetPosition();
};

