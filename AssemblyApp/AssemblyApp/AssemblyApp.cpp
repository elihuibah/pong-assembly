// AssemblyApp.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>

#define FB_W 320 //framebuffer width
#define FB_H 240 //framebuffer height
#define SCALE 3 //scale factor for the framebuffer

unsigned int framebuffer[FB_W * FB_H]; //framebuffer array that will hold the pixel data


// Data segment from ASM to C++ //
int WINDOW_WIDTH = 0x140; //DW 140h (320 decimal)
int WINDOW_HEIGHT = 0xC8; //DW 0C8h (200 decimal)
int WINDOW_BOUNDS = 6;

int BALL_ORIGINAL_X = 0xA0;
int BALL_ORIGINAL_Y = 0x64;
int BALL_X = BALL_ORIGINAL_X;
int BALL_Y = BALL_ORIGINAL_Y;
int BALL_SIZE = 4;
int BALL_VELOCITY_X = 5;
int BALL_VELOCITY_Y = 2;

int PADDLE_WIDTH = 5;
int PADDLE_HEIGHT = 0x1F;
int PADDLE_LEFT_X = 0x0A;
int PADDLE_LEFT_Y = 0x0A;
int PADDLE_RIGHT_X = 0x130;
int PADDLE_RIGHT_Y = 0x0A;
int PADDLE_VELOCITY = 5;


// Key configuration that replaces INT 16h in asm

int KEY_W = 0;
int KEY_S = 0;
int KEY_O = 0;
int KEY_L = 0;
// 1 = key is held down
bool running = true // if running is true, the game loop continues; if false, the game loop ends





int main()
{
    int a = 5, b = 7, resultado;
    __asm {
        mov eax, a
        add eax, b
        mov resultado, eax
    }
    std::cout << resultado << std::endl;
    return 0;
}
