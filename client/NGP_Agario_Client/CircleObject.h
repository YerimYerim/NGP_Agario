#pragma once
#include "Position.h"
#include <wtypes.h>
class CircleObject
{
	float size;
	int R;
	int G;
	int B;
public:
	Position position;
	Position GetPosition();
	int GetSize();
	void SetPosition(Position& pos);
	void SetSize(int nsize);
	BOOL CrashCheck(CircleObject& object);
	void Draw(HDC hdc);
	void SetRandomPosition();
};

