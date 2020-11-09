#include "CircleObject.h"
#include <random>
#include <random>
#include <time.h>


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
    Ellipse(hdc, position.x - size/2, position.y - size/2, position.x + size / 2, position.y + size / 2);
}

void CircleObject::SetRandomPosition()
{
    std::default_random_engine dre(rand());
    std::uniform_int_distribution<unsigned int> uid(20, 780);
    int x = uid(dre);
    int y = uid(dre);
    Position pos(x, y);
    this->SetPosition(pos);
}
