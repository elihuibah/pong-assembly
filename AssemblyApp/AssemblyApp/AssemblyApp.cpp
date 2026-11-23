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

// Player points
unsigned char PLAYER_ONE_POINTS = 0; // LEFT PLAYER SCORE
unsigned char PLAYER_TWO_POINTS = 0; // RIGHT PLAYER SCORE
unsigned char WINNER_INDEX = 0; // 1 or 2 when someone wins
unsigned char GAME_ACTIVE = 1; //1 = playing 0 = game over

char TEXT_PLAYER_ONE_POINTS[] = "0";
char TEXT_PLAYER_TWO_POINTS[] = "0";

unsigned char CURSOR_ROW = 0;
unsigned char CURSOR_COL = 0;
char* STRING_PTR = nullptr;
unsigned char CURRENT_SCENE = 0;
unsigned char EXITING_GAME = 0;
unsigned char AI_CONTROLLED = 0;

char TEXT_GAME_OVER_TITLE[] = "GAME OVER";
char TEXT_GAME_OVER_WINNER[] = "Player 0 won!";
char TEXT_GAME_OVER_PLAY_AGAIN[] = "Press R to play again";
char TEXT_GAME_OVER_MAIN_MENU[] = "Press E to exit to main menu";
char TEXT_MAIN_MENU_TITLE[] = "PONG ASSEMBLY | MAIN MENU";
char TEXT_MAIN_MENU_SINGLEPLAYER[] = "SINGLEPLAYER - S KEY";
char TEXT_MAIN_MENU_MULTIPLAYER[] = "MULTIPLAYER - M KEY";
char TEXT_MAIN_MENU_EXIT[] = "EXIT GAME - E KEY";
char TEXT_CREDITS_1[] = "Project by Mariana Flores,";
char TEXT_CREDITS_2[] = "Elihu Ibarra & Leonardo Piedras";
unsigned char KEY_PRESSED = 0;


// One glyph: which character it is + its 8x8 picture (1 byte per row, bit 7 = leftmost pixel)
struct Glyph {
    char character;
    unsigned char rows[8];
};

// Only the characters the game uses. Lowercase is drawn as uppercase (see DrawString).
const Glyph GLYPHS[] = {
    {'!', {0x18,0x18,0x18,0x18,0x00,0x00,0x18,0x00}},
    {'&', {0x38,0x6C,0x38,0x76,0xDC,0xCC,0x76,0x00}},
    {',', {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30}},
    {'-', {0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00}},
    {'|', {0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x00}},
    {'0', {0x3C,0x66,0x6E,0x76,0x66,0x66,0x3C,0x00}},
    {'1', {0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00}},
    {'2', {0x3C,0x66,0x06,0x0C,0x30,0x60,0x7E,0x00}},
    {'3', {0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00}},
    {'4', {0x0C,0x1C,0x3C,0x6C,0x7E,0x0C,0x0C,0x00}},
    {'5', {0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00}},
    {'6', {0x3C,0x66,0x60,0x7C,0x66,0x66,0x3C,0x00}},
    {'7', {0x7E,0x66,0x0C,0x18,0x18,0x18,0x18,0x00}},
    {'8', {0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00}},
    {'9', {0x3C,0x66,0x66,0x3E,0x06,0x66,0x3C,0x00}},
    {'A', {0x18,0x3C,0x66,0x66,0x7E,0x66,0x66,0x00}},
    {'B', {0x7C,0x66,0x66,0x7C,0x66,0x66,0x7C,0x00}},
    {'C', {0x3C,0x66,0x60,0x60,0x60,0x66,0x3C,0x00}},
    {'D', {0x78,0x6C,0x66,0x66,0x66,0x6C,0x78,0x00}},
    {'E', {0x7E,0x60,0x60,0x7C,0x60,0x60,0x7E,0x00}},
    {'F', {0x7E,0x60,0x60,0x7C,0x60,0x60,0x60,0x00}},
    {'G', {0x3C,0x66,0x60,0x6E,0x66,0x66,0x3C,0x00}},
    {'H', {0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00}},
    {'I', {0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00}},
    {'J', {0x1E,0x0C,0x0C,0x0C,0x0C,0x6C,0x38,0x00}},
    {'K', {0x66,0x6C,0x78,0x70,0x78,0x6C,0x66,0x00}},
    {'L', {0x60,0x60,0x60,0x60,0x60,0x60,0x7E,0x00}},
    {'M', {0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x00}},
    {'N', {0x66,0x76,0x7E,0x7E,0x6E,0x66,0x66,0x00}},
    {'O', {0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00}},
    {'P', {0x7C,0x66,0x66,0x7C,0x60,0x60,0x60,0x00}},
    {'Q', {0x3C,0x66,0x66,0x66,0x66,0x3C,0x0E,0x00}},
    {'R', {0x7C,0x66,0x66,0x7C,0x78,0x6C,0x66,0x00}},
    {'S', {0x3C,0x66,0x60,0x3C,0x06,0x66,0x3C,0x00}},
    {'T', {0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00}},
    {'U', {0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x00}},
    {'V', {0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00}},
    {'W', {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00}},
    {'X', {0x66,0x66,0x3C,0x18,0x3C,0x66,0x66,0x00}},
    {'Y', {0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00}},
    {'Z', {0x7E,0x06,0x0C,0x18,0x30,0x60,0x7E,0x00}}
};

// FONT[c] = glyph of ASCII code c. Global arrays start as all zeros, so missing characters
// (like the space) are drawn as blank cells.
unsigned char FONT[128][8];

// Copies each glyph from the list into its slot in FONT. Call once at start-up.
void InitFont() {
    int count = sizeof(GLYPHS) / sizeof(GLYPHS[0]);      // number of glyphs in the list
    for (int g = 0; g < count; g++) {
        unsigned char c = (unsigned char)GLYPHS[g].character;
        for (int row = 0; row < 8; row++) {
            FONT[c][row] = GLYPHS[g].rows[row];
        }
    }
}

void ResetBallPosition() {
    __asm {
        mov ax, BALL_ORIGINAL_X
        mov BALL_X, ax
        mov ax, BALL_ORIGINAL_Y
        mov BALL_Y, ax
    }
}

void UpdateTextPlayerOnePoints() {
    __asm {
        xor ax, ax; AX = 0
        mov  al, PLAYER_ONE_POINTS; ej.AL = 2
        add  al, 30h; numero->ASCII: 2 + 30h = 32h = '2'
        mov  byte ptr[TEXT_PLAYER_ONE_POINTS], al; primer caracter del texto = '2'
    }
}

void UpdateTextPlayerTwoPoints(){
    __asm {
        xor ax, ax; AX = 0
        mov  al, PLAYER_TWO_POINTS
        add  al, 30h
        mov  byte ptr[TEXT_PLAYER_TWO_POINTS], al

    }

}




void MoveBall() {
    __asm {
        ; Mover la pelota horizontalmente
        mov  ax, BALL_VELOCITY_X
        add  BALL_X, ax

        ; Limite izquierdo(BALL_X < WINDOW_BOUNDS)
        mov  ax, WINDOW_BOUNDS
        cmp  BALL_X, ax
        jl   GIVE_POINT_TO_PLAYER_TWO

        ; Limite derecho(BALL_X > WINDOW_WIDTH - BALL_SIZE - WINDOW_BOUNDS)
        mov  ax, WINDOW_WIDTH
        sub  ax, BALL_SIZE
        sub  ax, WINDOW_BOUNDS
        cmp  BALL_X, ax
        jg   GIVE_POINT_TO_PLAYER_ONE
        jmp  MOVE_BALL_VERTICALLY
         
        GIVE_POINT_TO_PLAYER_ONE :
        inc  PLAYER_ONE_POINTS; puntaje jugador 1 + 1
            call ResetBallPosition; CALL RESET_BALL_POSITION
            call UpdateTextPlayerOnePoints; CALL UPDATE_TEXT_PLAYER_ONE_POINTS
            cmp  PLAYER_ONE_POINTS, 05h; ¿llego a 5 puntos ?
            jge  GAME_OVER
            jmp  EXIT_MOVE_BALL; instead of RET

            GIVE_POINT_TO_PLAYER_TWO :
        inc  PLAYER_TWO_POINTS
            call ResetBallPosition
            call UpdateTextPlayerTwoPoints
            cmp  PLAYER_TWO_POINTS, 05h
            jge  GAME_OVER
            jmp  EXIT_MOVE_BALL; instead of RET

            GAME_OVER : ; algun jugador tiene 5 puntos
            cmp  PLAYER_ONE_POINTS, 05h
            jnl  WINNER_IS_PLAYER_ONE
            jmp  WINNER_IS_PLAYER_TWO

            WINNER_IS_PLAYER_ONE :
        mov  WINNER_INDEX, 01h
            jmp  CONTINUE_GAME_OVER

            WINNER_IS_PLAYER_TWO :
        mov  WINNER_INDEX, 02h

            CONTINUE_GAME_OVER :
        mov  PLAYER_ONE_POINTS, 00h; reiniciar puntajes
            mov  PLAYER_TWO_POINTS, 00h
            call UpdateTextPlayerOnePoints
            call UpdateTextPlayerTwoPoints
            mov  GAME_ACTIVE, 00h; detener el juego
            jmp  EXIT_MOVE_BALL

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

bool keyWasDown[256] = {};
void ReadKeyPressed() {
    const char menuKeys[] = { 'R','E','S','M' };
    KEY_PRESSED = 0;
    for (int i = 0; i < 4; i++) {
        char k = menuKeys[i];
        bool isDown = (GetAsyncKeyState(k) & 0x8000) != 0;
        if (isDown && !keyWasDown) {
            KEY_PRESSED = k;
        }
        keyWasDown[k] = isDown;
    }
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

// Replaces INT 10h AH=02h + INT 21h AH=09h.
// Draws the string at STRING_PTR, starting at text cell (CURSOR_ROW, CURSOR_COL).
// Each cell is 8x8 pixels, like the 40x25 text grid of video mode 0Dh.
void DrawString() {
    __asm {
        mov   esi, STRING_PTR; ESI->first character(like DX in DOS)

        NEXT_CHAR:
        movzx eax, byte ptr[esi]; EAX = current character
            cmp   al, 0; '\0' = end of string(DOS used '$')
            je    END_STRING

            ; --- lowercase->uppercase: 'a'..'z' -> 'A'..'Z' -- -
            cmp   al, 'a'
            jb    LOOKUP_GLYPH
            cmp   al, 'z'
            ja    LOOKUP_GLYPH
            sub   al, 20h; 'a' (61h) - 20h = 'A' (41h)

            LOOKUP_GLYPH:
        cmp   al, 7Fh; outside the ASCII table->skip
            ja    ADVANCE_CURSOR

            ; --- EBX->glyph = FONT + character * 8 -- -
            lea   ebx, FONT
            shl   eax, 3; EAX = character code * 8
            add   ebx, eax
        
            ; --- EDI->top - left pixel of the cell-- -
            movzx edi, CURSOR_ROW
            shl   edi, 3; y = row * 8
            imul  edi, edi, FB_W; EDI = y * 320
            movzx eax, CURSOR_COL
            shl   eax, 3; x = col * 8
            add   edi, eax; EDI = y * 320 + x
            lea   eax, framebuffer
            add   edi, eax; EDI = address of that pixel

            mov   dh, 8; 8 rows per character
            ROW_LOOP :
        mov   dl, byte ptr[ebx]; DL = bit pattern of this row
            mov   ecx, 0; column 0..7
            COL_LOOP:
        test  dl, 80h; is the leftmost bit 1 ?
            jz    SKIP_PIXEL
            mov   byte ptr[edi + ecx], 0Fh; white pixel
            SKIP_PIXEL :
        shl   dl, 1; move the next bit into bit 7
            inc   ecx
            cmp   ecx, 8
            jl    COL_LOOP

            inc   ebx; next row of the glyph
            add   edi, FB_W; next pixel row on screen
            dec   dh
            jnz   ROW_LOOP

            ADVANCE_CURSOR :
        inc   CURSOR_COL; the DOS cursor also moves right
            inc   esi; next character
            jmp   NEXT_CHAR

            END_STRING :
        nop
    }
}

// Was DRAW_UI
void DrawUI() {
    __asm {
        ; Texto del puntaje del jugador 1
        mov  CURSOR_ROW, 04h; MOV DH, 04h(fila)
        mov  CURSOR_COL, 06h; MOV DL, 06h(columna)
        lea  eax, TEXT_PLAYER_ONE_POINTS; LEA DX, TEXT_PLAYER_ONE_POINTS
        mov  STRING_PTR, eax
        call DrawString; MOV AH, 09h / INT 21h

        ; Texto del puntaje del jugador 2
        mov  CURSOR_ROW, 04h
        mov  CURSOR_COL, 1Fh
        lea  eax, TEXT_PLAYER_TWO_POINTS
        mov  STRING_PTR, eax
        call DrawString
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
        DrawUI();
        Present(hwnd);
        //temporal: until DRAW_GAME_OVER_MENU is ported, restart right away
        if (GAME_ACTIVE == 0) {
            GAME_ACTIVE = 1;
        }
        Sleep(55);

    }
    return 0;
}
