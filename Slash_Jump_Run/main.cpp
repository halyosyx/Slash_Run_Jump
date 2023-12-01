#include <cstdio>
#include <math.h>
#include "raylib.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 397;
const char* TITLE_NAME = "Warrior Runner";

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE_NAME);
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(GRAY);

        EndDrawing();
    }

    CloseWindow();
}