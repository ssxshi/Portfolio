#include "Button.h"

Button makeButton(Uint x, Uint y, Uint width, Uint height, Color color, char* text, Uint fontSize) {
    Button newB = {x, y, width, height, text, fontSize, color};
    return newB;
}

void drawButton(Button b) {
    DrawRectangle(b.x, b.y, b.width, b.height, b.color);

    DrawText(b.text, b.x + 4, b.y + 4, b.fontSize, BLACK);
    DrawText(b.text, b.x + 8, b.y + 8, b.fontSize, BLACK);
    DrawText(b.text, b.x + 6, b.y + 6, b.fontSize, WHITE);
}

bool clicked(Button b, int mouseX, int mouseY) {
    return mouseX >= b.x && mouseX <= b.x + b.width &&
           mouseY >= b.y && mouseY <= b.y + b.height;
}