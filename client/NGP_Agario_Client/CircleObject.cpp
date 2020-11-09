#include "CircleObject.h"


void CircleObject::SetPosition(Position& pos)
{
    position.x = pos.x;
    position.y = pos.y;
}

Position CircleObject::GetPosition()
{
    return position;
}

void CircleObject::SetSize(int nsize)
{
    size = nsize;
}

int CircleObject::GetSize()
{
    return size;
}

void CircleObject::CrashCheck(CircleObject& object)
{
}

void CircleObject::Draw(HDC hdc)
{
    Ellipse(hdc, position.x - size, position.y - size, size * 2, size * 2);
}
