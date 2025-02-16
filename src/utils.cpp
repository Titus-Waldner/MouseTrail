// src/utils.cpp
#include "utils.h"
#include <cstdlib>  // for rand()

COLORREF RandomHeartColor()
{
    int r = 200 + (rand() % 26);   // 180..255
    int g = rand() % 121;         //   0..120
    int b = 150 + (rand() % 106); // 100..255

    // Occasionally bright red
    if (rand() % 5 == 0) {
        r = 255;
        g = rand() % 60;
        b = 80 + (rand() % 40);
    }
    return RGB(r, g, b);
}
