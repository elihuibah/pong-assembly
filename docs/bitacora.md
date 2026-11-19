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
