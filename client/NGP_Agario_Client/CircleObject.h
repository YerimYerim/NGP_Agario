#pragma once
#include "Position.h"
class CircleObject
{
	float size;
	Position position;
	
public:
	void SetPosition();
	Position GetPosition();
	void SetSize(int nsize);
	int GetSize();
	void CrashCheck(CircleObject& object);
	void Draw();
};

