# Código en Ensamblador 8086 en DOSBox
Código en ensamblador 8086, basado relativamente en el curso
"[MASM] 8086 Assembly - Pong" (Programming Dimension, YouTube).

## Levantar el Entorno de Desarrollo
1. Descargar las herramientas de MASM desde:
   https://drive.google.com/drive/folders/1akM4UNg6StiVE3ehzEstOgOhEw1JBxA0
2. Copiar su contenido a `dosbox/TOOLS/` (esa carpeta no se sube al repositorio).
3. En la sección [autoexec] de la configuración de DOSBox, agregar:
   mount c "ruta\a\pong-assembly\dosbox"
   c:
   path Z:\;C:\TOOLS