// AssemblyApp.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <windows.h>

#define FB_W 320 //framebuffer width
#define FB_H 200 //framebuffer height
#define SCALE 3 //scale factor for the framebuffer

unsigned char framebuffer[FB_W * FB_H]; //framebuffer array that will hold the pixel data

void ClearScreen() {
    __asm {
        lea  edi, framebuffer; EDI = address of the first pixel
        mov  ecx, FB_W* FB_H; ECX = 64, 000 pixels
        mov  al, 00h; AL = color 0 (black), like BL = 00h in the DOS code
        rep  stosb; repeat ECX times : [EDI] = AL, EDI += 1
    }
}

// Data segment from ASM to C++ //
short WINDOW_WIDTH = 0x140; //DW 140h (320 decimal)
short WINDOW_HEIGHT = 0xC8; //DW 0C8h (200 decimal)
short WINDOW_BOUNDS = 6;

short BALL_ORIGINAL_X = 0xA0;
short BALL_ORIGINAL_Y = 0x64;
short BALL_X = BALL_ORIGINAL_X;
short BALL_Y = BALL_ORIGINAL_Y;
short BALL_SIZE = 4;
short BALL_VELOCITY_X = 5;
short BALL_VELOCITY_Y = 2;

short PADDLE_WIDTH = 5;
short PADDLE_HEIGHT = 0x1F;
short PADDLE_LEFT_X = 0x0A;
short PADDLE_LEFT_Y = 0x0A;
short PADDLE_RIGHT_X = 0x130;
short PADDLE_RIGHT_Y = 0x0A;
short PADDLE_VELOCITY = 5;


// Key configuration that replaces short 16h in asm

short KEY_W = 0;
short KEY_S = 0;
short KEY_O = 0;
short KEY_L = 0;
// 1 = key is held down
bool running = true; // if running is true, the game loop continues; if false, the game loop ends

// Was MOVE_BALL
void MoveBall() {
    __asm {
        ; Mover la pelota horizontalmente
        mov  ax, BALL_VELOCITY_X
        add  BALL_X, ax

        ; Limite izquierdo(BALL_X < WINDOW_BOUNDS)
        mov  ax, WINDOW_BOUNDS
        cmp  BALL_X, ax
        jl   RESET_POSITION

        ; Limite derecho(BALL_X > WINDOW_WIDTH - BALL_SIZE - WINDOW_BOUNDS)
        mov  ax, WINDOW_WIDTH
        sub  ax, BALL_SIZE
        sub  ax, WINDOW_BOUNDS
        cmp  BALL_X, ax
        jg   RESET_POSITION
        jmp  MOVE_BALL_VERTICALLY

        RESET_POSITION :
        ; Was CALL RESET_BALL_POSITION + RET->code placed here directly
            mov  ax, BALL_ORIGINAL_X
            mov  BALL_X, ax
            mov  ax, BALL_ORIGINAL_Y
            mov  BALL_Y, ax
            jmp  EXIT_MOVE_BALL; instead of RET

            MOVE_BALL_VERTICALLY :
        mov  ax, BALL_VELOCITY_Y
            add  BALL_Y, ax

            ; Limite superior
            mov  ax, WINDOW_BOUNDS
            cmp  BALL_Y, ax
            jl   NEG_VELOCITY_Y

            ; Limite inferior
            mov  ax, WINDOW_HEIGHT
            sub  ax, BALL_SIZE
            sub  ax, WINDOW_BOUNDS
            cmp  BALL_Y, ax
            jg   NEG_VELOCITY_Y

            ; Colision con la paleta derecha
            mov  ax, BALL_X
            add  ax, BALL_SIZE
            cmp  ax, PADDLE_RIGHT_X
            jng  CHECK_COLLISION_WITH_LEFT_PADDLE

            mov  ax, PADDLE_RIGHT_X
            add  ax, PADDLE_WIDTH
            cmp  BALL_X, ax
            jnl  CHECK_COLLISION_WITH_LEFT_PADDLE

            mov  ax, BALL_Y
            add  ax, BALL_SIZE
            cmp  ax, PADDLE_RIGHT_Y
            jng  CHECK_COLLISION_WITH_LEFT_PADDLE

            mov  ax, PADDLE_RIGHT_Y
            add  ax, PADDLE_HEIGHT
            cmp  BALL_Y, ax
            jnl  CHECK_COLLISION_WITH_LEFT_PADDLE

            jmp  NEG_VELOCITY_X

            CHECK_COLLISION_WITH_LEFT_PADDLE :
        mov  ax, BALL_X
            add  ax, BALL_SIZE
            cmp  ax, PADDLE_LEFT_X
            jng  EXIT_MOVE_BALL

            mov  ax, PADDLE_LEFT_X
            add  ax, PADDLE_WIDTH
            cmp  BALL_X, ax
            jnl  EXIT_MOVE_BALL

            mov  ax, BALL_Y
            add  ax, BALL_SIZE
            cmp  ax, PADDLE_LEFT_Y
            jng  EXIT_MOVE_BALL

            mov  ax, PADDLE_LEFT_Y
            add  ax, PADDLE_HEIGHT
            cmp  BALL_Y, ax
            jnl  EXIT_MOVE_BALL

            jmp  NEG_VELOCITY_X

            NEG_VELOCITY_Y :
        neg  BALL_VELOCITY_Y
            jmp  EXIT_MOVE_BALL; instead of RET

            NEG_VELOCITY_X :
        neg  BALL_VELOCITY_X

            EXIT_MOVE_BALL : ; the single exit point
            nop
    }
}

// New: replaces INT 16h. Fills the KEY_ variables for the assembly to use.
void ReadKeys() {
    KEY_W = (GetAsyncKeyState('W') & 0x8000) ? 1 : 0;
    KEY_S = (GetAsyncKeyState('S') & 0x8000) ? 1 : 0;
    KEY_O = (GetAsyncKeyState('O') & 0x8000) ? 1 : 0;
    KEY_L = (GetAsyncKeyState('L') & 0x8000) ? 1 : 0;
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) running = false;   // Esc = quit
}

// Was MOVE_PADDLES
void MovePaddles() {
    __asm {
        ; ---- - Paleta izquierda---- -
        cmp  KEY_W, 1
        je   MOVE_LEFT_PADDLE_UP
        cmp  KEY_S, 1
        je   MOVE_LEFT_PADDLE_DOWN
        jmp  CHECK_RIGHT_PADDLE_MOVEMENT

        MOVE_LEFT_PADDLE_UP :
        mov  ax, PADDLE_VELOCITY
            sub  PADDLE_LEFT_Y, ax
            mov  ax, WINDOW_BOUNDS
            cmp  PADDLE_LEFT_Y, ax
            jl   FIX_PADDLE_LEFT_TOP_POSITION
            jmp  CHECK_RIGHT_PADDLE_MOVEMENT
            FIX_PADDLE_LEFT_TOP_POSITION :
        mov  PADDLE_LEFT_Y, ax
            jmp  CHECK_RIGHT_PADDLE_MOVEMENT

            MOVE_LEFT_PADDLE_DOWN :
        mov  ax, PADDLE_VELOCITY
            add  PADDLE_LEFT_Y, ax
            mov  ax, WINDOW_HEIGHT
            sub  ax, WINDOW_BOUNDS
            sub  ax, PADDLE_HEIGHT
            cmp  PADDLE_LEFT_Y, ax
            jg   FIX_PADDLE_LEFT_BOTTOM_POSITION
            jmp  CHECK_RIGHT_PADDLE_MOVEMENT
            FIX_PADDLE_LEFT_BOTTOM_POSITION :
        mov  PADDLE_LEFT_Y, ax

            ; ---- - Paleta derecha---- -
            CHECK_RIGHT_PADDLE_MOVEMENT:
        cmp  KEY_O, 1
            je   MOVE_RIGHT_PADDLE_UP
            cmp  KEY_L, 1
            je   MOVE_RIGHT_PADDLE_DOWN
            jmp  EXIT_PADDLE_MOVEMENT

            MOVE_RIGHT_PADDLE_UP :
        mov  ax, PADDLE_VELOCITY
            sub  PADDLE_RIGHT_Y, ax
            mov  ax, WINDOW_BOUNDS
            cmp  PADDLE_RIGHT_Y, ax
            jl   FIX_PADDLE_RIGHT_TOP_POSITION
            jmp  EXIT_PADDLE_MOVEMENT
            FIX_PADDLE_RIGHT_TOP_POSITION :
        mov  PADDLE_RIGHT_Y, ax
            jmp  EXIT_PADDLE_MOVEMENT

            MOVE_RIGHT_PADDLE_DOWN :
        mov  ax, PADDLE_VELOCITY
            add  PADDLE_RIGHT_Y, ax
            mov  ax, WINDOW_HEIGHT
            sub  ax, WINDOW_BOUNDS
            sub  ax, PADDLE_HEIGHT
            cmp  PADDLE_RIGHT_Y, ax
            jg   FIX_PADDLE_RIGHT_BOTTOM_POSITION
            jmp  EXIT_PADDLE_MOVEMENT
            FIX_PADDLE_RIGHT_BOTTOM_POSITION :
        mov  PADDLE_RIGHT_Y, ax

            EXIT_PADDLE_MOVEMENT :
        nop
    }
}


// Was DRAW_BALL
void DrawBall() {
    __asm {
        mov  cx, BALL_X; columna inicial(x)
        mov  dx, BALL_Y; linea inicial(y)

        DRAW_BALL_HORIZONTAL:
        ; --- replaces INT 10h / AH = 0Ch, AL = 0Fh-- -
            movzx eax, dx; EAX = y(16 -> 32 bits)
            imul  eax, eax, FB_W; EAX = y * 320
            movzx esi, cx; ESI = x
            add   eax, esi; EAX = y * 320 + x
            mov   byte ptr framebuffer[eax], 0Fh; color 0Fh = white
            ; ---------------------------------------- -

            inc  cx
            mov  ax, cx
            sub  ax, BALL_X
            cmp  ax, BALL_SIZE
            jng  DRAW_BALL_HORIZONTAL

            mov  cx, BALL_X
            inc  dx
            mov  ax, dx
            sub  ax, BALL_Y
            cmp  ax, BALL_SIZE
            jng  DRAW_BALL_HORIZONTAL
    }
}


// Was DRAW_PADDLES
void DrawPaddles() {
    __asm {
        ; ---- - Paleta izquierda---- -
        mov  cx, PADDLE_LEFT_X
        mov  dx, PADDLE_LEFT_Y

        DRAW_PADDLE_LEFT_HORIZONTAL :
        movzx eax, dx
            imul  eax, eax, FB_W
            movzx esi, cx
            add   eax, esi
            mov   byte ptr framebuffer[eax], 0Fh

            inc  cx
            mov  ax, cx
            sub  ax, PADDLE_LEFT_X
            cmp  ax, PADDLE_WIDTH
            jng  DRAW_PADDLE_LEFT_HORIZONTAL

            mov  cx, PADDLE_LEFT_X
            inc  dx
            mov  ax, dx
            sub  ax, PADDLE_LEFT_Y
            cmp  ax, PADDLE_HEIGHT
            jng  DRAW_PADDLE_LEFT_HORIZONTAL

            ; ---- - Paleta derecha---- -
            mov  cx, PADDLE_RIGHT_X
            mov  dx, PADDLE_RIGHT_Y

            DRAW_PADDLE_RIGHT_HORIZONTAL :
        movzx eax, dx
            imul  eax, eax, FB_W
            movzx esi, cx
            add   eax, esi
            mov   byte ptr framebuffer[eax], 0Fh

            inc  cx
            mov  ax, cx
            sub  ax, PADDLE_RIGHT_X
            cmp  ax, PADDLE_WIDTH
            jng  DRAW_PADDLE_RIGHT_HORIZONTAL

            mov  cx, PADDLE_RIGHT_X
            inc  dx
            mov  ax, dx
            sub  ax, PADDLE_RIGHT_Y
            cmp  ax, PADDLE_HEIGHT
            jng  DRAW_PADDLE_RIGHT_HORIZONTAL
    }
}

// ===================== WINDOW (boilerplate) =====================

// Función para que Windows se comunique con la ventana de Juego
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_DESTROY) {           // el usuario presionó x
        running = false;
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);   
}

HWND CreateGameWindow() {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"PongWindow";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClass(&wc);

    RECT r = { 0, 0, FB_W * SCALE, FB_H * SCALE };
    AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);

    return CreateWindow(L"PongWindow", L"Pong - x86 Assembly",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
        r.right - r.left, r.bottom - r.top,
        nullptr, nullptr, wc.hInstance, nullptr);
}

// Copies the framebuffer to the window (stretched to fit)
// Header + 16-color table (mode 0Dh = 16 colors)
struct BITMAPINFO16 {
    BITMAPINFOHEADER header;
    RGBQUAD colors[16];
};

// EGA palette. RGBQUAD order is { Blue, Green, Red, 0 }
const RGBQUAD EGA_PALETTE[16] = {
    {0x00,0x00,0x00,0}, {0xAA,0x00,0x00,0}, {0x00,0xAA,0x00,0}, {0xAA,0xAA,0x00,0}, // 0 black, 1 blue, 2 green, 3 cyan
    {0x00,0x00,0xAA,0}, {0xAA,0x00,0xAA,0}, {0x00,0x55,0xAA,0}, {0xAA,0xAA,0xAA,0}, // 4 red, 5 magenta, 6 brown, 7 light gray
    {0x55,0x55,0x55,0}, {0xFF,0x55,0x55,0}, {0x55,0xFF,0x55,0}, {0xFF,0xFF,0x55,0}, // 8 dark gray, 9 light blue, 10 light green, 11 light cyan
    {0x55,0x55,0xFF,0}, {0xFF,0x55,0xFF,0}, {0x55,0xFF,0xFF,0}, {0xFF,0xFF,0xFF,0}  // 12 light red, 13 light magenta, 14 yellow, 15 white
};

// Copies the framebuffer to the window, translating color numbers through the palette
void Present(HWND hwnd) {
    BITMAPINFO16 bmi = {};
    bmi.header.biSize = sizeof(BITMAPINFOHEADER);
    bmi.header.biWidth = FB_W;
    bmi.header.biHeight = -FB_H;      // negative = row 0 at the top, like VGA
    bmi.header.biPlanes = 1;
    bmi.header.biBitCount = 8;        // 1 byte per pixel
    bmi.header.biCompression = BI_RGB;
    bmi.header.biClrUsed = 16;        // our table has 16 colors
    for (int i = 0; i < 16; i++) bmi.colors[i] = EGA_PALETTE[i];

    HDC hdc = GetDC(hwnd);
    RECT rc;
    GetClientRect(hwnd, &rc);
    StretchDIBits(hdc, 0, 0, rc.right, rc.bottom, 0, 0, FB_W, FB_H,
        framebuffer, (BITMAPINFO*)&bmi, DIB_RGB_COLORS, SRCCOPY);
    ReleaseDC(hwnd, hdc);
}




int main()
{
    HWND hwnd = CreateGameWindow();
    while (running) {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        ReadKeys();
        ClearScreen();
        MoveBall();
        DrawBall();
        MovePaddles();
        DrawPaddles();
        Present(hwnd);
        Sleep(55);

    }
    return 0;
}
