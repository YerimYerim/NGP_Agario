#include "CircleObject.h"
#include <random>
#include <random>
#include <time.h>
#include <iostream>


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

void CircleObject::SetRGB(int r, int g, int b)
{
    R = r;
    G = g;
    B = b;
}

int CircleObject::GetSize()
{
    return size;
}

BOOL CircleObject::CrashCheck(CircleObject& object)
{
    float distance = sqrt((this->position.x - object.position.x) * (this->position.x - object.position.x) + (this->position.y - object.position.y) * (this->position.y - object.position.y));
    if (distance < this->size / 2 + object.size / 2)
    {
        return true;
    }
    return false;
}

void CircleObject::Draw(HDC hdc)
{
    HBRUSH Brush, oBrush;
    Brush = CreateSolidBrush(RGB(R, G, B));
    oBrush = (HBRUSH)SelectObject(hdc, Brush);

    Ellipse(hdc, position.x - size / 2, position.y - size / 2, position.x + size / 2, position.y + size / 2);

    SelectObject(hdc, oBrush);
    DeleteObject(Brush);
}

void CircleObject::SetRandomPosition()
{
    std::default_random_engine dre(rand()+1);
    std::uniform_int_distribution<unsigned int> uid(20, 780);
    std::uniform_int_distribution<unsigned int> color(0, 250);
    int x = uid(dre);
    int y = uid(dre);
    R = color(dre);
    G = color(dre);
    B = color(dre);
    Position pos(x, y);
    this->SetPosition(pos);
}
