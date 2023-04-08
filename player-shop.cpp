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
        currency = currency - 20;
        HealthPotions++;
        busy = true;
    }
    return 0;
}
