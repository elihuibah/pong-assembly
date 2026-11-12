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

// Was MOVE_BALL
void MoveBall() {
    __asm {
        ; Mover la pelota horizontalmente
        mov  eax, BALL_VELOCITY_X
        add  BALL_X, eax

        ; Limite izquierdo(BALL_X < WINDOW_BOUNDS)
        mov  eax, WINDOW_BOUNDS
        cmp  BALL_X, eax
        jl   RESET_POSITION

        ; Limite derecho(BALL_X > WINDOW_WIDTH - BALL_SIZE - WINDOW_BOUNDS)
        mov  eax, WINDOW_WIDTH
        sub  eax, BALL_SIZE
        sub  eax, WINDOW_BOUNDS
        cmp  BALL_X, eax
        jg   RESET_POSITION
        jmp  MOVE_BALL_VERTICALLY

        RESET_POSITION :
        ; Was CALL RESET_BALL_POSITION + RET->code placed here directly
            mov  eax, BALL_ORIGINAL_X
            mov  BALL_X, eax
            mov  eax, BALL_ORIGINAL_Y
            mov  BALL_Y, eax
            jmp  EXIT_MOVE_BALL; instead of RET

            MOVE_BALL_VERTICALLY :
        mov  eax, BALL_VELOCITY_Y
            add  BALL_Y, eax

            ; Limite superior
            mov  eax, WINDOW_BOUNDS
            cmp  BALL_Y, eax
            jl   NEG_VELOCITY_Y

            ; Limite inferior
            mov  eax, WINDOW_HEIGHT
            sub  eax, BALL_SIZE
            sub  eax, WINDOW_BOUNDS
            cmp  BALL_Y, eax
            jg   NEG_VELOCITY_Y

            ; Colision con la paleta derecha
            mov  eax, BALL_X
            add  eax, BALL_SIZE
            cmp  eax, PADDLE_RIGHT_X
            jng  CHECK_COLLISION_WITH_LEFT_PADDLE

            mov  eax, PADDLE_RIGHT_X
            add  eax, PADDLE_WIDTH
            cmp  BALL_X, eax
            jnl  CHECK_COLLISION_WITH_LEFT_PADDLE

            mov  eax, BALL_Y
            add  eax, BALL_SIZE
            cmp  eax, PADDLE_RIGHT_Y
            jng  CHECK_COLLISION_WITH_LEFT_PADDLE

            mov  eax, PADDLE_RIGHT_Y
            add  eax, PADDLE_HEIGHT
            cmp  BALL_Y, eax
            jnl  CHECK_COLLISION_WITH_LEFT_PADDLE

            jmp  NEG_VELOCITY_X

            CHECK_COLLISION_WITH_LEFT_PADDLE :
        mov  eax, BALL_X
            add  eax, BALL_SIZE
            cmp  eax, PADDLE_LEFT_X
            jng  EXIT_MOVE_BALL

            mov  eax, PADDLE_LEFT_X
            add  eax, PADDLE_WIDTH
            cmp  BALL_X, eax
            jnl  EXIT_MOVE_BALL

            mov  eax, BALL_Y
            add  eax, BALL_SIZE
            cmp  eax, PADDLE_LEFT_Y
            jng  EXIT_MOVE_BALL

            mov  eax, PADDLE_LEFT_Y
            add  eax, PADDLE_HEIGHT
            cmp  BALL_Y, eax
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
        mov  eax, PADDLE_VELOCITY
            sub  PADDLE_LEFT_Y, eax
            mov  eax, WINDOW_BOUNDS
            cmp  PADDLE_LEFT_Y, eax
            jl   FIX_PADDLE_LEFT_TOP_POSITION
            jmp  CHECK_RIGHT_PADDLE_MOVEMENT
            FIX_PADDLE_LEFT_TOP_POSITION :
        mov  PADDLE_LEFT_Y, eax
            jmp  CHECK_RIGHT_PADDLE_MOVEMENT

            MOVE_LEFT_PADDLE_DOWN :
        mov  eax, PADDLE_VELOCITY
            add  PADDLE_LEFT_Y, eax
            mov  eax, WINDOW_HEIGHT
            sub  eax, WINDOW_BOUNDS
            sub  eax, PADDLE_HEIGHT
            cmp  PADDLE_LEFT_Y, eax
            jg   FIX_PADDLE_LEFT_BOTTOM_POSITION
            jmp  CHECK_RIGHT_PADDLE_MOVEMENT
            FIX_PADDLE_LEFT_BOTTOM_POSITION :
        mov  PADDLE_LEFT_Y, eax

            ; ---- - Paleta derecha---- -
            CHECK_RIGHT_PADDLE_MOVEMENT:
        cmp  KEY_O, 1
            je   MOVE_RIGHT_PADDLE_UP
            cmp  KEY_L, 1
            je   MOVE_RIGHT_PADDLE_DOWN
            jmp  EXIT_PADDLE_MOVEMENT

            MOVE_RIGHT_PADDLE_UP :
        mov  eax, PADDLE_VELOCITY
            sub  PADDLE_RIGHT_Y, eax
            mov  eax, WINDOW_BOUNDS
            cmp  PADDLE_RIGHT_Y, eax
            jl   FIX_PADDLE_RIGHT_TOP_POSITION
            jmp  EXIT_PADDLE_MOVEMENT
            FIX_PADDLE_RIGHT_TOP_POSITION :
        mov  PADDLE_RIGHT_Y, eax
            jmp  EXIT_PADDLE_MOVEMENT

            MOVE_RIGHT_PADDLE_DOWN :
        mov  eax, PADDLE_VELOCITY
            add  PADDLE_RIGHT_Y, eax
            mov  eax, WINDOW_HEIGHT
            sub  eax, WINDOW_BOUNDS
            sub  eax, PADDLE_HEIGHT
            cmp  PADDLE_RIGHT_Y, eax
            jg   FIX_PADDLE_RIGHT_BOTTOM_POSITION
            jmp  EXIT_PADDLE_MOVEMENT
            FIX_PADDLE_RIGHT_BOTTOM_POSITION :
        mov  PADDLE_RIGHT_Y, eax

            EXIT_PADDLE_MOVEMENT :
        nop
    }
}

void ReadKeys() {
    KEY_W = (GetAsyncKeyState('W') & 0x8000) ? 1 : 0;
    KEY_S = (GetAsyncKeyState('S') & 0x8000) ? 1 : 0;
    KEY_O = (GetAsyncKeyState('O') & 0x8000) ? 1 : 0;
    KEY_L = (GetAsyncKeyState('L') & 0x8000) ? 1 : 0;
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) running = false;   // Esc = quit
}

// Was DRAW_BALL
void DrawBall() {
    __asm {
        mov  ecx, BALL_X; columna inicial(x)
        mov  edx, BALL_Y; linea inicial(y)

        DRAW_BALL_HORIZONTAL:
        ; --- replaces INT 10h / AH = 0Ch: pixel(ECX, EDX) = white-- -
            mov  eax, edx
            imul eax, WINDOW_WIDTH; EAX = y * 320
            add  eax, ecx; EAX = y * 320 + x(pixel number)
            mov  dword ptr framebuffer[eax * 4], 0FFFFFFh; *4: each pixel is 4 bytes
            ; -------------------------------------------------------- -

            inc  ecx
            mov  eax, ecx
            sub  eax, BALL_X
            cmp  eax, BALL_SIZE
            jng  DRAW_BALL_HORIZONTAL

            mov  ecx, BALL_X
            inc  edx
            mov  eax, edx
            sub  eax, BALL_Y
            cmp  eax, BALL_SIZE
            jng  DRAW_BALL_HORIZONTAL
    }
}

// Was DRAW_PADDLES
void DrawPaddles() {
    __asm {
        ; ---- - Paleta izquierda---- -
        mov  ecx, PADDLE_LEFT_X
        mov  edx, PADDLE_LEFT_Y

        DRAW_PADDLE_LEFT_HORIZONTAL :
        mov  eax, edx
            imul eax, WINDOW_WIDTH
            add  eax, ecx
            mov  dword ptr framebuffer[eax * 4], 0FFFFFFh

            inc  ecx
            mov  eax, ecx
            sub  eax, PADDLE_LEFT_X
            cmp  eax, PADDLE_WIDTH
            jng  DRAW_PADDLE_LEFT_HORIZONTAL

            mov  ecx, PADDLE_LEFT_X
            inc  edx
            mov  eax, edx
            sub  eax, PADDLE_LEFT_Y
            cmp  eax, PADDLE_HEIGHT
            jng  DRAW_PADDLE_LEFT_HORIZONTAL

            ; ---- - Paleta derecha---- -
            mov  ecx, PADDLE_RIGHT_X
            mov  edx, PADDLE_RIGHT_Y

            DRAW_PADDLE_RIGHT_HORIZONTAL :
        mov  eax, edx
            imul eax, WINDOW_WIDTH
            add  eax, ecx
            mov  dword ptr framebuffer[eax * 4], 0FFFFFFh

            inc  ecx
            mov  eax, ecx
            sub  eax, PADDLE_RIGHT_X
            cmp  eax, PADDLE_WIDTH
            jng  DRAW_PADDLE_RIGHT_HORIZONTAL

            mov  ecx, PADDLE_RIGHT_X
            inc  edx
            mov  eax, edx
            sub  eax, PADDLE_RIGHT_Y
            cmp  eax, PADDLE_HEIGHT
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
void Present(HWND hwnd) {
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = FB_W;
    bmi.bmiHeader.biHeight = -FB_H;   // negative = row 0 at the top, like VGA
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC hdc = GetDC(hwnd);
    RECT rc;
    GetClientRect(hwnd, &rc);
    StretchDIBits(hdc, 0, 0, rc.right, rc.bottom, 0, 0, FB_W, FB_H,
        framebuffer, &bmi, DIB_RGB_COLORS, SRCCOPY);
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
