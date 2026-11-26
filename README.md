# Pong in x86 Assembly

Final project for **Fundamentals of Operating Systems** (Ramón Santana Fernández, PhD), CETYS Universidad Campus Internacional Ensenada.

A recreation of the classic **Pong**, written in x86 assembly in two versions:

| Version | Folder | Runs on | How it talks to the hardware |
|---|---|---|---|
| **DOS (original)** | `dosbox/` | DOSBox, 16-bit real mode | BIOS and DOS interrupts (`INT 10h`, `INT 16h`, `INT 21h`) |
| **Windows (port)** | `AssemblyApp/` | Windows, 32-bit (x86) | C++ with embedded `__asm` blocks + the Win32 API |

The game logic (ball physics, collisions, paddles, AI, score and menus) is **the same assembly code** in both versions. The port only replaces the parts that used interrupts, because a Windows program can't call BIOS/DOS interrupts.

---

## Features

- Two paddles that move independently and at the same time.
- Ball physics with bounces off the paddles and the top and bottom edges.
- Score display. The first player to reach **5 points** wins.
- **Main menu:** single player (vs. AI), multiplayer (local, same keyboard), or exit.
- **AI paddle:** follows the ball only when it's approaching, with a deadzone so it doesn't jitter. It moves slower than a player, so it can be beaten.
- **Serve delay:** the ball waits about 1 second before each serve.
- **Game Over screen:** shows the winner. You can play again or go back to the menu.
- **ESC during a game** returns to the main menu. This is available in the Windows version only.
- **Mode indicator:** shows SINGLEPLAYER or MULTIPLAYER at the top of the screen.

## Controls

| Screen | Key | Action |
|---|---|---|
| Main menu | `S` | Single player (vs. AI) |
| | `M` | Multiplayer |
| | `E` | Exit the game |
| In game | `W` / `S` | Left paddle up / down |
| | `O` / `L` | Right paddle up / down (multiplayer only) |
| | `ESC` | Back to the main menu (score resets) |
| Game Over | `R` | Play again |
| | `E` | Back to the main menu |

---

## Running the Windows version (Visual Studio)

### Requirements
- **Windows 10 or 11**
- **Visual Studio 2022**, version **17.13 or later**. The solution uses the newer `.slnx` format, which older versions can't open.
- The **"Desktop development with C++"** workload. You can add it from the Visual Studio Installer → *Modify*.

### Steps
1. **Clone the repository:**
   ```bash
   git clone https://github.com/elihuibah/pong-assembly.git
   ```
   You can also use **Code → Download ZIP** on GitHub and extract it.
2. **Open the solution:** in Visual Studio, go to **File → Open → Project/Solution** and select `AssemblyApp/AssemblyApp.slnx`.
3. **Select the x86 platform.** In the toolbar, change the platform dropdown from `x64` to **`x86`** (it may appear as *Win32*).
   > ⚠️ This step is required. MSVC only supports inline `__asm` on 32-bit x86. On x64 the project won't compile.
4. **Build:** **Build → Rebuild Solution**. The Output window should end with `1 succeeded`.
5. **Run:** press **Ctrl + F5**, or **F5** to run with the debugger. A *"Pong - x86 Assembly"* window opens with the main menu.

### Troubleshooting

| Problem | Fix |
|---|---|
| *"Unable to start program ... .exe. The system cannot find the file"* | The build failed, so no `.exe` was created. Open **View → Error List**, fix the errors and rebuild. |
| *"inline assembler not supported"* / `__asm` errors on x64 | Switch the platform to **x86** (step 3). |
| The `.slnx` file doesn't open | Update Visual Studio to 17.13 or later from the Visual Studio Installer. |
| Errors only appear after building | IntelliSense doesn't check code inside `__asm`. Always use **Build → Rebuild Solution** and read the **Error List**. |

---

## Running the DOS version

The original source is in `dosbox/Pong.asm`. It's meant to be assembled and run inside **DOSBox**. See the `dosbox/` folder for the files.

---

## How the port works

All the game logic lives in `AssemblyApp/AssemblyApp/AssemblyApp.cpp`. Each DOS procedure became a C++ function containing an `__asm { }` block, with the same names as in the original (for example, `MOVE_BALL` → `MoveBall()`).

### Replacing the interrupts

| DOS | Windows port |
|---|---|
| `INT 10h` (video mode 0Dh, draw pixel) | Writes to a `framebuffer[320 × 200]` array (1 byte per pixel), copied to the window with `StretchDIBits` and the 16-color EGA palette |
| `INT 16h` (read held keys) | `ReadKeys()` → `GetAsyncKeyState` fills `KEY_W`, `KEY_S`, `KEY_O`, `KEY_L` |
| `INT 16h` AH=00h (wait for a key in menus) | `ReadKeyPressed()` detects new presses with edge detection, without blocking the game loop |
| `INT 10h` AH=02h + `INT 21h` AH=09h (print text) | `DrawString()` draws text from an 8×8 bitmap font table |
| `INT 21h` AH=2Ch (system time, frame timing) | `Sleep(55)` → ~18 frames per second |
| `INT 21h` AH=4Ch (exit to DOS) | `ConcludeExitGame()` ends the main loop. `main()` returns and the OS releases the process |

### Main loop
```
main():  InitFont → create window → loop:
           read input (ReadKeys, ReadKeyPressed)
           RunFrame()   ← scene state machine: main menu / game / game over
           Present()    ← framebuffer → window
           Sleep(55)
```

### Porting rules
- `DW` → `short` (16-bit), `DB` → `unsigned char` (8-bit). The code keeps using the 16-bit registers `AX`, `BX`, `CX`, `DX`, as in DOS. 32-bit registers are only used for memory addresses.
- No `RET` or `INT` inside `__asm`. Each block exits through a final label followed by `nop`.
- `CX` and `DX` are saved with `push`/`pop` before any call inside a drawing loop.

---

## Repository structure

```
pong-assembly/
├── AssemblyApp/          Visual Studio port (C++ + inline x86 assembly)
│   ├── AssemblyApp.slnx  ← open this in Visual Studio
│   └── AssemblyApp/
│       └── AssemblyApp.cpp
├── dosbox/               Original DOS version (Pong.asm)
├── docs/                 Project documentation
└── README.md
```

## Team

- **Mariana Angélica Flores Vega**: testing, documentation and presentations
- **Elihú Alejandro Ibarra Hernández**: original DOS version
- **Leonardo Roberto Piedras Fuente**: Visual Studio port

Software Engineering, CETYS Universidad Campus Internacional Ensenada, 2026.
