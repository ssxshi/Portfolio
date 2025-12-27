#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include "../raylibInclude/raylib.h"

typedef unsigned int Uint;

typedef struct
{
    Uint x;
    Uint y;
    Uint width;
    Uint height;

    char* text;
    Uint fontSize;
    Color color;
} Button;

Button makeButton(Uint x, Uint y, Uint width, Uint height, Color color, char* text, Uint fontSize);

void drawButton(Button b);

bool clicked(Button b, int mouseX, int mouseY);

#endif