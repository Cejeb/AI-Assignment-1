#include "raylib.h"

int currency = 0;

int shop(void)
{
    const int screenWidth = 640;
    const int screenHeight = 480;

    InitWindow(screenWidth, screenHeight, "Menu Window");

    Rectangle button1Rec = { screenWidth / 2 - 50, 200, 100, 40 };
    Rectangle button2Rec = { screenWidth / 2 - 50, 260, 100, 40 };

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (CheckCollisionPointRec(GetMousePosition(), button1Rec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            break;

        if (CheckCollisionPointRec(GetMousePosition(), button2Rec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            break;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangleRec(button1Rec, GRAY);
        DrawText("Button 1", button1Rec.x + 10, button1Rec.y + 10, 20, WHITE);

        DrawRectangleRec(button2Rec, GRAY);
        DrawText("Button 2", button2Rec.x + 10, button2Rec.y + 10, 20, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}