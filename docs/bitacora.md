## Semana 2 (06 - 12 de noviembre de 2026) - Elih煤 Ibarra (Rol A)

### Avance en DOSBox

- Ciclo principal del juego: se lee el reloj del sistema ocon INT 21h y solo se actualiza la pantalla cuando cambian las cent茅simas de segundo con tal de que el juego avance a un ritmo constante en lugar de tan r谩pido como le sea posible al procesador.
- Limpieza de pantalla en cada cuadro (CLEAR_SCREEN) antes de volver a dibujar.
- Movimiento de la pelota (MOVE_BALL), y su posici贸n se actualiza sumando BALL_VELOCITY_X y BALL_VELOCITY_Y en cada cuadro (frame).
- Rebote en las paredes superior e inferior, donde se compara la posici贸n con los l铆mites de la ventana (CMP y saltos condicionales) y se invierte la velocidad con NEG (neg谩ndola).
- Reinicio de la pelota (RESET_BALL_POSITION) cuando sale por la izquierda o la derecha.
- Dibujo de las dos paletas (DRAW_PADDLES) con ciclos, igual que con la pelota.
- Movimiento de las paletas con el teclados (MOVE_PADDLES): INT 16h con AH = 01h. Se revisa si hay una tecla en el buffer sin detener el juego, y AH=00h la lee. Se emplean las teclas 'W' y 'S' para la paleta izquierda y 'O' y 'L' para la derecha. Se implementaron l铆mites para que las paletas no se salgan de la pantalla.

### Conceptos Revisados

- Saltos con signo (JL, JG) frente a saltos sin signo (JB, JA). Como la velocidad puede ser negativa, as铆 que las comparaciones deben interpretarse con signo.
- NEG como forma de invertir una direci贸n.
- Diferencia entre leer el teclado con espera (bloqueo del juego) y revisar el buffer sin espera (INT 16h AH=01h).
- Uso del reloj del sistema como temporizador de un ciclo de juego.

### Organizaci贸n en GitHub

- Se crearon las etiquetas del proyecto (bug, documentation, dosbox, feature, presentation, visual-studio)
- Se cre贸 el milestone "Avance de la Semana 2" con fecha l铆mite del 12 de noviembre.
- Se cre贸 el tablero de proyecto "Pingue-Pongue" (nombre del equipo) con sus respectivas columnas "Por Hacer", "En Progreso" y "Hecho".
- Se crearon ramas de trabajo: feature/keyboard-input en la que se trabaj贸 con los registros encargados de los servicios de entrada de teclado en la arquitectura de ensamblador 8086; feature/collision en la que se trabaj贸 con la colisi贸n de la pelota y las paletas; y data_segment_port, en la que se est谩 trabajando el 'port' del c贸digo y l贸gica desarrollada en DOSBox a Visual Studio con c贸digo embebido dentro de C++.
- Se inicio el proceso de embeber los avances de ensamblador a C++ en Visual Studio.
- Se agregaron las primeras variables globales del entorno: tama駉 de pelota, paddlets y de ventana de juego.
- Se agregaron m閠odos para identificar las porciones de c骴igo ensamblador y se馻ralos con m閠odos.
### Dificultades

- Se lidi贸 con un push rechazado (non-fast-forward).
