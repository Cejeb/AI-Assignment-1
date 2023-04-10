#include "raylib.h"
#include <iostream>
#include <thread>

int currency = 0;
int HealthPotions = 0;
bool busy = false;

int shop(void)
{
    if (currency >= 20 && busy == false)
    {
        busy = true;
        currency = currency - 20;
        HealthPotions++;
    }
    return 0;
}