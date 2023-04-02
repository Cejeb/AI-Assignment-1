#include "raylib.h"

int currency = 0;
int HealthPotions = 0;

int shop(void)
{
    if (currency >= 20)
    {
        currency = currency - 20;
        HealthPotions++;
    }
    return 0;
}
//This version uses a window pop up, but we think an ingame system similar to Spelunky would suit better, leaving this commented out for now
/*int shop(void)
{
    const int screenWidth = 640;
    const int screenHeight = 480;

    InitWindow(screenWidth, screenHeight, "Menu Window");

    Rectangle BuyButton = { screenWidth / 2 - 50, 200, 100, 40 };
    Rectangle ExitButton = { screenWidth / 2 - 50, 260, 100, 40 };

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (CheckCollisionPointRec(GetMousePosition(), BuyButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (currency >= 20)
            {
                currency = currency - 20;
            }
        }

        if (CheckCollisionPointRec(GetMousePosition(), ExitButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            break;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangleRec(BuyButton, GRAY);
        DrawText("Buy Health Potion", BuyButton.x + 10, BuyButton.y + 10, 20, WHITE);

        DrawRectangleRec(ExitButton, GRAY);
        DrawText("Leave Shop", ExitButton.x + 10, ExitButton.y + 10, 20, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}*/