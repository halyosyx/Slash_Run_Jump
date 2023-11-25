#pragma once
#include "raylib.h"

class BaseCharacter
{
public:
    BaseCharacter(Texture2D texture, Rectangle rect, Vector2 position);
    ~BaseCharacter();

private:

    Texture2D texture;
    Rectangle rect;
    Vector2 position;

    int frame;
    float runningTime;
    float updateTime;

};
