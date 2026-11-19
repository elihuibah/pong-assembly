## Semana 2 (06 - 12 de noviembre de 2026) - Elihú Ibarra (Rol A)

### Avance en DOSBox

- Ciclo principal del juego: se lee el reloj del sistema con INT 21h y solo se actualiza la pantalla cuando cambian las centésimas de segundo con tal de que el juego avance a un ritmo constante en lugar de tan rápido como le sea posible al procesador.
- Limpieza de pantalla en cada cuadro (CLEAR_SCREEN) antes de volver a dibujar.
- Movimiento de la pelota (MOVE_BALL), y su posiciónn se actualiza sumando BALL_VELOCITY_X y BALL_VELOCITY_Y en cada cuadro (frame).
- Rebote en las paredes superior e inferior, donde se compara la posición con los límites de la ventana (CMP y saltos condicionales) y se invierte la velocidad con NEG (negándola).
- Reinicio de la pelota (RESET_BALL_POSITION) cuando sale por la izquierda o la derecha.
- Dibujo de las dos paletas (DRAW_PADDLES) con ciclos, igual que con la pelota.
- Movimiento de las paletas con el teclados (MOVE_PADDLES): INT 16h con AH = 01h. Se revisa si hay una tecla en el buffer sin detener el juego, y AH=00h la lee. Se emplean las teclas 'W' y 'S' para la paleta izquierda y 'O' y 'L' para la derecha. Se implementaron límites para que las paletas no se salgan de la pantalla.

### Conceptos Revisados

- Saltos con signo (JL, JG) frente a saltos sin signo (JB, JA). Como la velocidad puede ser negativa, así­ que las comparaciones deben interpretarse con signo.
- NEG como forma de invertir una dirección.
- Diferencia entre leer el teclado con espera (bloqueo del juego) y revisar el buffer sin espera (INT 16h AH=01h).
- Uso del reloj del sistema como temporizador de un ciclo de juego.

### Organización en GitHub

- Se crearon las etiquetas del proyecto (bug, documentation, dosbox, feature, presentation, visual-studio).
- Se creó el milestone "Avance de la Semana 2" con fecha lí­mite del 12 de noviembre.
- Se creó el tablero de proyecto "Pingue-Pongue" (nombre del equipo) con sus respectivas columnas "Por Hacer", "En Progreso" y "Hecho".
- Se crearon ramas de trabajo: feature/keyboard-input en la que se trabajó con los registros encargados de los servicios de entrada de teclado en la arquitectura de ensamblador 8086; feature/collision en la que se trabajó con la colisión de la pelota y las paletas; y data_segment_port, en la que se está trabajando el 'port' del código y lógica desarrollada en DOSBox a Visual Studio con código embebido dentro de C++.
- Se inicio el proceso de embeber los avances de ensamblador a C++ en Visual Studio.
- Se agregaron las primeras variables globales del entorno: tamaño de pelota, paddlets y de ventana de juego.
- Se agregaron métodos para identificar las porciones de código ensamblador y señaralos con métodos.
  
### Dificultades
- Se lidió con un push rechazado (non-fast-forward).

## Semana 3 (13 - 19 de noviembre de 2026) - Elihú Ibarra (Rol A)

### Revisión e Integración
- Revisé y aprobé el pull request de Leonardo Piedras con la rama 'clear_screen' y lo fusioné a main (commit df37199). Con este cambio, el juego ya se ejecuta en Visual Studio.

### Avance en DOSBox

- Base del sistema de puntaje (commit 57cbcf8): variables para el puntaje de cada jugador y el punto se asigna cuando la pelota sale por el lado contrario.
- Texto del marcador en pantalla (commit b49850b):
  - INT 10h, AH = 02h: posiciona el cursor (BH = página, DH = fila, DL = columna).
  - INT 21h, AH = 09h: imprime la cadena cuya dirección está en DS:DX; la cadena debe terminar con el carácter '$'.
  - La dirección de cada cadena se carga en DX con OFFSET/LEA antes de llamar a la interrupción.
- Actualización y reinicio del puntaje (commit 557e2e6):
  - Procedimientos que actualizan el texto del puntaje de cada jugador.
  - Conversión del valor numérico a su caracter ASCCI sumando 30h (el código de '0', sólo funciona con puntajes de un dígito).
  - Los puntajes se reinician cuando un jugador llega a cierta cantidad de puntos.
- Pull request con los tres commits anteriores, fusionado a main (commit 0b12acb).

### Conceptos Revisados

- Diferencia entre servicios de BIOS (INT 10h, manejo de video y cursor) y servicio de DOS (INT 21h, salida de texto).
- Las cadenas en DOS terminan en '$', no en el caracter nulo como en C.
- Los dígitos en ASCII ocupan los códigos 30h a 39h, así que convertir un número de un dígito a caracter es una suma.
- Direccionamiento de memoria: pasar a una interrupción la dirección de un dato (DS:DX) en lugar del dato mismo.

### Organización en GitHub

- Primera revisión de código del equipo mediante pull request (rama 'clear_screen' de Leonardo Piedras).
- Trabajo en la rama 'feature/player-points', intergrada a main por pull request
  
### Dificultades
- Al subir la rama nueva, Git rechazó el push porque la rama no tenía rama remota asociada: se resolvió con 'gir push -u origin feature/player-points'.

## Semana 3 (13 - 19 de noviembre de 2026) - Leonardo Piedras

### Revisión e Integración

- Dividí el código de ensamblador para hacer el proceso de embeber en C++. Se crearon los métodos ClearScreen(), MoveBall(), ReadKeys(), MovePaddles(), DrawBall() y DrawPaddles(). Cada PROC del código de DOSBox ahora es una función de C++ con un bloque `__asm { }`, y se conservaron los mismos nombres de variables del tutorial (BALL_X, PADDLE_LEFT_Y, etc.) para que coincidan con el código de Elihú.
- El DATA SEGMENT se convirtió en variables globales de C++ de tipo `short` (16 bits, equivalente a DW), siguiendo la Regla 1 del plan de trabajo. Así el código conserva los registros AX, CX y DX del original.
- Se reemplazaron las interrupciones de DOS, que no pueden ejecutarse en Windows porque los programas corren en modo protegido:
  - INT 10h (dibujar pixel) → escritura directa en una pantalla virtual (`framebuffer`) de 320×200, un byte por pixel.
  - INT 16h (teclado) → función ReadKeys() con GetAsyncKeyState.
  - INT 21h, AH=2Ch (reloj) → Sleep(55), que da aproximadamente la misma velocidad que en DOSBox.
- Se creó la ventana de Windows, la paleta de 16 colores EGA (modo de video 0Dh) y el ciclo principal en main(), que llama a los métodos en el mismo orden que el ciclo CHECK_TIME del código original.
- Resultado: el juego corre en Visual Studio con la pelota rebotando y las dos paletas moviéndose.

### Avance en C++

- ClearScreen(): Reemplaza CLEAR_SCREEN. En lugar de reiniciar el modo de video en cada cuadro (lo que causaba parpadeo en DOS), llena la pantalla virtual con el color 0 (negro) usando `lea edi, framebuffer` y `rep stosb`, que repite la escritura 64,000 veces.

- MoveBall(): Porte de MOVE_BALL con cambios mínimos: mueve la pelota, la regresa al centro si sale por la izquierda o la derecha, invierte la velocidad vertical con `neg` al tocar los bordes superior e inferior, y detecta la colisión con cada paleta mediante cuatro comparaciones de traslape de rectángulos. Como no se puede usar RET dentro de un bloque `__asm`, cada RET se sustituyó por un salto a la etiqueta EXIT_MOVE_BALL, y el código de RESET_BALL_POSITION se colocó directamente dentro de la función.

- MovePaddles(): Porte de MOVE_PADDLES. Ya no lee el teclado con INT 16h: revisa las variables KEY_W, KEY_S, KEY_O y KEY_L que llena ReadKeys(). Conserva los límites superior e inferior de cada paleta. Con este cambio, las dos paletas pueden moverse al mismo tiempo, que era un requisito que la versión de DOS no cumplía porque INT 16h solo lee una tecla por cuadro.

- DrawBall(): Porte de DRAW_BALL. Conserva los ciclos anidados con CX (columna) y DX (línea) del original. La parte de INT 10h se reemplazó por el cálculo de la posición del pixel (`y * 320 + x`, usando `movzx` e `imul`) y la escritura del color 0Fh (blanco) con `mov byte ptr framebuffer[eax], 0Fh`. No se llama a funciones de C++ dentro del ciclo (Regla 2).

- DrawPaddles(): Igual que DrawBall(), pero dibuja la paleta izquierda y la derecha con PADDLE_WIDTH y PADDLE_HEIGHT.

- ReadKeys(): Función nueva en C++ que sustituye a INT 16h. Usa GetAsyncKeyState para saber si W, S, O, L o Esc están presionadas y guarda 1 o 0 en las variables que lee el ensamblador. Esc termina el juego.

### Conceptos Revisados

- Diferencia entre modo real (DOS) y modo protegido (Windows), y por qué las interrupciones INT 10h, 16h y 21h no se pueden usar en un programa de Windows.
- Registros de 16 bits (AX, CX, DX) contra 32 bits (EAX), y tamaños de datos: byte, word (DW = short) y dword.
- Instrucciones: mov, add, sub, inc, neg, cmp, saltos con signo (jl, jg, jng, jnl, je), movzx, imul de tres operandos, lea y rep stosb.
- Reglas del ensamblador en línea de Visual Studio (`__asm`): solo compila en x86/Win32, no se permite RET ni INT, y se pueden usar variables de C++ por nombre.
- Pantalla virtual (framebuffer): un arreglo donde cada byte es un pixel, y una paleta que traduce el número de color a un color real.
- C++: #define, variables globales, short y bool, funciones, arreglos, structs, operador ternario y máscaras de bits (`& 0x8000`).
- Uso del depurador de Visual Studio: breakpoints, ventana Watch y ventana Registers.

### Dificultades

- Al ejecutar el programa aparecía el error "Could not find a part of the path …\Debug\AssemblyApp.exe". El problema era que la compilación no se completaba, así que no se generaba el ejecutable. Se resolvió configurando el proyecto para x86, porque el ensamblador en línea no funciona en x64.
- Las variables se habían declarado como `int` y el código usaba EAX. Se cambiaron a `short` y AX para cumplir la Regla 1 del plan de trabajo.
- La pantalla virtual estaba declarada de 240 líneas en lugar de 200 (FB_H = 240), lo que hacía que la pelota rebotara antes del borde inferior de la ventana. [Confirmar: corregido a 200.]
- Fue necesario estudiar conceptos nuevos de C++ y de la API de Windows (ventanas, mensajes, paleta) para poder entender y explicar el código, además del ensamblador.
