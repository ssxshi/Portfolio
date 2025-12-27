#include <stdio.h>
#include <string.h>

#include "raylibInclude/raylib.h"
#include "Include/Button.h"

void display(double* num, double* prevNum){
    DrawRectangle(14, 14, 480, 100, BLACK);
    DrawRectangle(10, 10, 480, 100, (Color){200, 200, 200, 255});

    if (*num / 100000000 > 9){*num = *prevNum;}

    char charArr[11];
    snprintf(charArr, sizeof(charArr), "%g", *num);
    DrawText(charArr, 20, 25, 80, BLACK);
    DrawText(charArr, 18, 23, 80, WHITE);

    *prevNum = *num;
}

void addButtons(Button* buttons){
    buttons[0] = makeButton(250, 150, 60, 60, WHITE, "/", 50);
    buttons[1] = makeButton(350, 150, 60, 60, WHITE, "c", 50);
    buttons[2] = makeButton(150, 250, 60, 60, WHITE, "+", 50);
    buttons[3] = makeButton(250, 250, 60, 60, WHITE, "-", 50);
    buttons[4] = makeButton(350, 250, 60, 60, WHITE, "x", 50);
    buttons[5] = makeButton(150, 350, 60, 60, WHITE, "7", 50);
    buttons[6] = makeButton(250, 350, 60, 60, WHITE, "8", 50);
    buttons[7] = makeButton(350, 350, 60, 60, WHITE, "9", 50);
    buttons[8] = makeButton(150, 450, 60, 60, WHITE, "4", 50);
    buttons[9] = makeButton(250, 450, 60, 60, WHITE, "5", 50);
    buttons[10] = makeButton(350, 450, 60, 60, WHITE, "6", 50);
    buttons[11] = makeButton(150, 550, 60, 60, WHITE, "1", 50);
    buttons[12] = makeButton(250, 550, 60, 60, WHITE, "2", 50);
    buttons[13] = makeButton(350, 550, 60, 60, WHITE, "3", 50);
    buttons[14] = makeButton(150, 650, 60, 60, WHITE, "0", 50);
    buttons[15] = makeButton(250, 650, 60, 60, WHITE, ".", 50);
    buttons[16] = makeButton(350, 650, 60, 60, WHITE, "=", 50);
}

void selected(char option, double* num, char* op, double* num1, bool* decimalMode, double* decimalPlace, bool* justCalculated) {
    switch(option) {
        case 'c':
        case 'C':
            *num = 0;
            *num1 = 0;
            *op = '\0';
            *decimalMode = false;
            *decimalPlace = 0.1;
            *justCalculated = false;
            break;
        case '0' ... '9':
            if (*justCalculated) {
                *num = 0;
                *decimalMode = false;
                *decimalPlace = 0.1;
                *justCalculated = false;
            }
            if (*decimalMode) {
                *num += (option - '0') * (*decimalPlace);
                *decimalPlace /= 10;
            } else {
                *num = (*num) * 10 + (option - '0');
            }
            break;
        case '.':
            if (*justCalculated) {
                *num = 0;
                *decimalMode = false;
                *decimalPlace = 0.1;
                *justCalculated = false;
            }
            if (!*decimalMode) {
                *decimalMode = true;
                *decimalPlace = 0.1;
            }
            break;
        case '+':
        case '-':
        case 'x':
        case '/':
            *num1 = *num;
            *op = option;
            *num = 0;
            *decimalMode = false;
            *decimalPlace = 0.1;
            *justCalculated = false;
            break;
        case '=':
            if (*op == '+') *num = *num1 + *num;
            else if (*op == '-') *num = *num1 - *num;
            else if (*op == 'x') *num = *num1 * *num;
            else if (*op == '/') *num = (*num != 0) ? (*num1 / *num) : 0;
            *op = '\0';
            *num1 = 0;
            *decimalMode = false;
            *decimalPlace = 0.1;
            *justCalculated = true;
            break;
    }
}

int main(){
    double displayNum = 0;
    double previousNum = 0;
    double num1 = 0;
    char operation = '\0';
    bool justCalculated = false;

    bool decimalMode = false;
    double decimalPlace = 0.1;

    Button buttons[17] = {};
    int mouseX = (int)GetMousePosition().x;
    int mouseY = (int)GetMousePosition().y;

    InitWindow(500, 800, "Calculator");

    addButtons(buttons);

    while (!WindowShouldClose()){
        mouseX = (int)GetMousePosition().x;
        mouseY = (int)GetMousePosition().y;

        BeginDrawing();
        ClearBackground(GRAY);

        display(&displayNum, &previousNum);

        for (int i = 0; i < 17; i++) {
            drawButton(buttons[i]);
            if (clicked(buttons[i], mouseX, mouseY) == true && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) == true){
                selected(buttons[i].text[0], &displayNum, &operation, &num1, &decimalMode, &decimalPlace, &justCalculated);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}