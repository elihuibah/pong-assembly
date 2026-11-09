STACK SEGMENT PARA STACK
	DB 64 DUP (' ')
STACK ENDS

DATA SEGMENT PARA 'DATA'
	
	TIME_AUX DB 0 ; Variable usada al checar si el tiempo ha cambiado
	
	BALL_X DW 0Ah ; Posición en x (columna) de la pelota
	BALL_Y DW 0Ah ; Posición en y (línea) de la pelota
	BALL_SIZE DW 04h ; Tamaño de la pelota (cuántos pixeles tiene la pelota en ancho y largo)

DATA ENDS

CODE SEGMENT PARA 'CODE'

	MAIN PROC FAR
	ASSUME CS:CODE,DS:DATA,SS:STACK ; Asumir los segmentos de code, data y stack a los respectivos registros
	PUSH DS 						; Hacer push al stack el segmento DS
	SUB AX,AX						; Limpiar el registro AX
	PUSH AX							; Empujar AX al STACK
	MOV AX,DATA						; Guardar en el registro AX el contenido del segmento 'DATA'
	MOV DS,AX						; Guardar en el segmento DS los contenidos de AX
	POP AX							; Liberar el item de la parte superior del stack al registro AX
	POP AX							; Liberar el item de la parte superior del stack al registro AX
	
		MOV AH,00h ; Establecer configuración de modo de video
		MOV AL,0Dh ; Elegir el modo de video
		INT 10h ; Ejecutar la configuración
		
		MOV AH,0Bh ; Establecer configuración
		MOV BH,00h ; Color de fondo
		MOV BL,00h ; Elegir el color negro como el color de fondo
		INT 10h ; Ejecutar la cofiguración
		
		CHECK_TIME:
			MOV AH,2Ch ; Obtener el tiempo del sistema
			INT 21h ; CH = hora, CL = minuto, DH = segundo, DL = 1/100 segundos
					
			CMP DL,TIME_AUX	; ¿es el tiempo actual igual al anterior (TIME_AUX)?
			JE CHECK_TIME   ; si es el mismo, checar de nuevo
			; si es diferente, entonces dibujar, mover, y demás...
			
			MOV TIME_AUX,DL ; Actualizar tiempo	
			CALL DRAW_BALL
			
			JMP CHECK_TIME ; Después de lo anterior, checar el tiempo de nuevo

		RET
	MAIN ENDP
	
	DRAW_BALL PROC NEAR
	
		MOV CX,BALL_X ; Establecer la columna inicial (x)
		MOV DX,BALL_Y ; Establecer la linea inicial (Y)
		
		DRAW_BALL_HORIZONTAL:
			MOV AH,0Ch ; Establecer configuración de dibujado de pixel
			MOV AL,0Fh ; Elegir el color blanco como el color del pixel
			MOV BH,00h ; Establecer el número de página
			INT 10h ; Ejecutar la configuración
			
			INC CX	; CX = CX + 1
			MOV AX,CX ; CX - BALL_X > BALL_SIZE (SÍ -> Ir a la siguiente línea, NO -> Ir a la siguiente columna)
			SUB AX,BALL_X
			CMP AX,BALL_SIZE
			JNG DRAW_BALL_HORIZONTAL
			
			MOV CX,BALL_X ; El registro CX vuelve a la columna inicial
			INC DX ; Avanzar una línea
			
			MOV AX,DX ; DX - BALL_Y > BALL_SIZE (SÍ -> Salir de este proceso (procedure), NO -> continuar a la siguiente línea)
			SUB AX,BALL_Y
			CMP AX,BALL_SIZE
			JNG DRAW_BALL_HORIZONTAL			
			
		RET
	DRAW_BALL ENDP

CODE ENDS
END