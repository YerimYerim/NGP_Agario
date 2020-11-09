#pragma once
#include "Position.h"
#include <wtypes.h>
class CircleObject
{
	float size;
	Position position;
	
public:
	Position GetPosition();
	int GetSize();
	void SetPosition(Position& pos);
	void SetSize(int nsize);
	void CrashCheck(CircleObject& object);
	void Draw(HDC hdc);
	void SetRandomPosition();
};

