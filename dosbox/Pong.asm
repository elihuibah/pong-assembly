STACK SEGMENT PARA STACK
	DB 64 DUP (' ')
STACK ENDS

DATA SEGMENT PARA 'DATA'

	WINDOW_WIDTH DW 140h ; Ancho de la pantalla (ventana, 320 px)
	WINDOW_HEIGHT DW 0C8h ; Altura de la pantalla (ventana, 200 px)
	WINDOW_BOUNDS DW 6    ; Variable usada para checar colisiones tempranamente
	
	TIME_AUX DB 0 ; Variable usada al checar si el tiempo ha cambiado
	
	BALL_ORIGINAL_X DW 0A0h
	BALL_ORIGINAL_Y DW 64h
	BALL_X DW 0A0h ; Posición en x (columna) de la pelota
	BALL_Y DW 64h ; Posición en y (línea) de la pelota
	BALL_SIZE DW 04h ; Tamaño de la pelota (cuántos pixeles tiene la pelota en ancho y largo)
	BALL_VELOCITY_X DW 05h ; Velocidad en el eje x de la pelota
	BALL_VELOCITY_Y DW 02h ; Velocidad en el eje y de la pelota
	
	PADDLE_LEFT_X DW 0Ah
	PADDLE_LEFT_Y DW 0Ah
	
	PADDLE_RIGHT_X DW 130h
	PADDLE_RIGHT_Y DW 0Ah
	
	PADDLE_WIDTH DW 05h
	PADDLE_HEIGHT DW 1Fh
	PADDLE_VELOCITY DW 05h

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
	
		CALL CLEAR_SCREEN
		
		CHECK_TIME:
			MOV AH,2Ch ; Obtener el tiempo del sistema
			INT 21h ; CH = hora, CL = minuto, DH = segundo, DL = 1/100 segundos
					
			CMP DL,TIME_AUX	; ¿es el tiempo actual igual al anterior (TIME_AUX)?
			JE CHECK_TIME   ; si es el mismo, checar de nuevo
			; si es diferente, entonces dibujar, mover, y demás...
			
			MOV TIME_AUX,DL ; Actualizar tiempo	
			
			CALL CLEAR_SCREEN
			
			CALL MOVE_BALL
			CALL DRAW_BALL
			
			CALL MOVE_PADDLES
			CALL DRAW_PADDLES
			
			JMP CHECK_TIME ; Después de lo anterior, checar el tiempo de nuevo

		RET
	MAIN ENDP
	
	MOVE_BALL PROC NEAR
	
		MOV AX,BALL_VELOCITY_X
		ADD BALL_X,AX		   ; Mover la pelota horizontalmente
		
		MOV AX,WINDOW_BOUNDS
		CMP BALL_X,AX		
		JL RESET_POSITION	; BALL_X < 0 + WINDOW_BOUNDS (Y -> colisiona)		
		
		MOV AX,WINDOW_WIDTH
		SUB AX,BALL_SIZE
		SUB AX,WINDOW_BOUNDS
		CMP BALL_X,AX		; BALL_X > WINDOW_WIDTH - BALL_SIZE - WINDOW_BOUNDS (Y -> colisiona)
		JG RESET_POSITION
		
		MOV AX,BALL_VELOCITY_Y
		ADD BALL_Y,AX			; Mover la pelota verticalmente
		
		MOV AX,WINDOW_BOUNDS
		CMP BALL_Y,AX ; BALL_Y < 0 + WINDOW_BOUNDS (Y -> colisiona)
		JL NEG_VELOCITY_Y	
		
		MOV AX,WINDOW_HEIGHT
		SUB AX,BALL_SIZE
		SUB AX,WINDOW_BOUNDS
		CMP BALL_Y,AX		
		JG NEG_VELOCITY_Y ; BALL_Y > WINDOW_HEIGHT - BALL_SIZE - WINDOW_BOUNDS (Y -> colisiona)
		
		RET
		
		RESET_POSITION:
			CALL RESET_BALL_POSITION
			RET
		
		NEG_VELOCITY_Y:
			NEG BALL_VELOCITY_Y ; BALL_VELOCITY_Y = -BALL_VELOCITY_Y
			RET
			
	MOVE_BALL ENDP
	
	MOVE_PADDLES PROC NEAR
		; Movimiento de la paleta izquierda
		; Checar si alguna tecla está siendo presionada (NO -> Checar en la otra paleta)
		MOV AH,01h
		INT 16h
		JZ CHECK_RIGHT_PADDLE_MOVEMENT ; ZF = 1, JZ -> Saltar Sí Zero
		
		; Checar cuál tecla está siendo presionada (AL = caracter ASCII)
		MOV AH,00h
		INT 16h
		
		; Si es la tecla 'w' o 'W', mover hacia arriba
		CMP AL,77h ; 'w'
		JE MOVE_LEFT_PADDLE_UP
		CMP AL,57h ; 'W'
		JE MOVE_LEFT_PADDLE_UP
		
		; Si es la tecla 's' o 'S', mover hacia abajo
		CMP AL,73h ; 's'
		JE MOVE_LEFT_PADDLE_DOWN
		CMP AL,53h ; 'S'
		JE MOVE_LEFT_PADDLE_DOWN
		JMP CHECK_RIGHT_PADDLE_MOVEMENT
		
		MOVE_LEFT_PADDLE_UP:
			MOV AX,PADDLE_VELOCITY
			SUB PADDLE_LEFT_Y,AX
			
			MOV AX,WINDOW_BOUNDS
			CMP PADDLE_LEFT_Y,AX
			JL FIX_PADDLE_LEFT_TOP_POSITION
			JMP CHECK_RIGHT_PADDLE_MOVEMENT
			
			FIX_PADDLE_LEFT_TOP_POSITION:
				MOV AX,WINDOW_BOUNDS
				MOV PADDLE_LEFT_Y,AX
				JMP CHECK_RIGHT_PADDLE_MOVEMENT
		
		MOVE_LEFT_PADDLE_DOWN:
			MOV AX,PADDLE_VELOCITY
			ADD PADDLE_LEFT_Y,AX
			MOV AX,WINDOW_HEIGHT
			SUB AX,WINDOW_BOUNDS
			SUB AX,PADDLE_HEIGHT
			CMP PADDLE_LEFT_Y,AX
			JG FIX_PADDLE_LEFT_BOTTOM_POSITION
			JMP CHECK_RIGHT_PADDLE_MOVEMENT
			
			FIX_PADDLE_LEFT_BOTTOM_POSITION:
				MOV PADDLE_LEFT_Y,AX
				JMP CHECK_RIGHT_PADDLE_MOVEMENT
		
		; Movimiento de la paleta derecha
		CHECK_RIGHT_PADDLE_MOVEMENT:
		
		; Si es la tecla 'o' o 'O', mover hacia arriba
		CMP AL,6Fh ; 'o'
		JE MOVE_RIGHT_PADDLE_UP
		CMP AL,4Fh ; 'O'
		JE MOVE_RIGHT_PADDLE_UP
		
		; Si es la tecla 'l' o 'L', mover hacia abajo
		CMP AL,6Ch ; 'l'
		JE MOVE_RIGHT_PADDLE_DOWN
		CMP AL,4Ch ; 'L'
		JE MOVE_RIGHT_PADDLE_DOWN
		JMP EXIT_PADDLE_MOVEMENT ; Salir del procedure
		
		MOVE_RIGHT_PADDLE_UP:
			MOV AX,PADDLE_VELOCITY
			SUB PADDLE_RIGHT_Y,AX
			
			MOV AX,WINDOW_BOUNDS
			CMP PADDLE_RIGHT_Y,AX
			JL FIX_PADDLE_RIGHT_TOP_POSITION
			JMP EXIT_PADDLE_MOVEMENT
			
			FIX_PADDLE_RIGHT_TOP_POSITION:
				MOV AX,WINDOW_BOUNDS
				MOV PADDLE_RIGHT_Y,AX
				JMP EXIT_PADDLE_MOVEMENT
		
		MOVE_RIGHT_PADDLE_DOWN:
			MOV AX,PADDLE_VELOCITY
			ADD PADDLE_RIGHT_Y,AX
			MOV AX,WINDOW_HEIGHT
			SUB AX,WINDOW_BOUNDS
			SUB AX,PADDLE_HEIGHT
			CMP PADDLE_RIGHT_Y,AX
			JG FIX_PADDLE_RIGHT_BOTTOM_POSITION
			JMP EXIT_PADDLE_MOVEMENT
			
			FIX_PADDLE_RIGHT_BOTTOM_POSITION:
				MOV PADDLE_RIGHT_Y,AX
				JMP EXIT_PADDLE_MOVEMENT
		
		EXIT_PADDLE_MOVEMENT:
			RET
				
	MOVE_PADDLES ENDP
	
	RESET_BALL_POSITION PROC NEAR
	
		MOV AX,BALL_ORIGINAL_X
		MOV BALL_X,AX
		
		MOV AX,BALL_ORIGINAL_Y
		MOV BALL_Y,AX
		
		RET
	RESET_BALL_POSITION ENDP
	
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
	
	DRAW_PADDLES PROC NEAR
		MOV CX,PADDLE_LEFT_X ; Establecer la columna inicial (x)
		MOV DX,PADDLE_LEFT_Y ; Establecer la linea inicial (Y)
		
		DRAW_PADDLE_LEFT_HORIZONTAL:
			MOV AH,0Ch ; Establecer configuración de dibujado de pixel
			MOV AL,0Fh ; Elegir el color blanco como el color del pixel
			MOV BH,00h ; Establecer el número de página
			INT 10h ; Ejecutar la configuración
			
			INC CX	; CX = CX + 1
			MOV AX,CX ; CX - PADDLE_LEFT_X > PADDLE_WIDTH (SÍ -> Ir a la siguiente línea, NO -> Ir a la siguiente columna)
			SUB AX,PADDLE_LEFT_X
			CMP AX,PADDLE_WIDTH
			JNG DRAW_PADDLE_LEFT_HORIZONTAL
			
			MOV CX,PADDLE_LEFT_X ; El registro CX vuelve a la columna inicial
			INC DX ; Avanzar una línea
			
			MOV AX,DX ; DX - PADDLE_LEFT_Y > PADDLE_HEIGHT (SÍ -> Salir de este proceso (procedure), NO -> continuar a la siguiente línea)
			SUB AX,PADDLE_LEFT_Y
			CMP AX,PADDLE_HEIGHT
			JNG DRAW_PADDLE_LEFT_HORIZONTAL
		
		MOV CX,PADDLE_RIGHT_X ; Establecer la columna inicial (x)
		MOV DX,PADDLE_RIGHT_Y ; Establecer la linea inicial (Y)
		
		DRAW_PADDLE_RIGHT_HORIZONTAL:
			MOV AH,0Ch ; Establecer configuración de dibujado de pixel
			MOV AL,0Fh ; Elegir el color blanco como el color del pixel
			MOV BH,00h ; Establecer el número de página
			INT 10h ; Ejecutar la configuración
			
			INC CX	; CX = CX + 1
			MOV AX,CX ; CX - PADDLE_RIGHT_X > PADDLE_WIDTH (SÍ -> Ir a la siguiente línea, NO -> Ir a la siguiente columna)
			SUB AX,PADDLE_RIGHT_X
			CMP AX,PADDLE_WIDTH
			JNG DRAW_PADDLE_RIGHT_HORIZONTAL
			
			MOV CX,PADDLE_RIGHT_X ; El registro CX vuelve a la columna inicial
			INC DX ; Avanzar una línea
			
			MOV AX,DX ; DX - PADDLE_RIGHT_Y > PADDLE_HEIGHT (SÍ -> Salir de este proceso (procedure), NO -> continuar a la siguiente línea)
			SUB AX,PADDLE_RIGHT_Y
			CMP AX,PADDLE_HEIGHT
			JNG DRAW_PADDLE_RIGHT_HORIZONTAL
			
		RET
		
	DRAW_PADDLES ENDP
	
	CLEAR_SCREEN PROC NEAR
		MOV AH,00h ; Establecer configuración de modo de video
		MOV AL,0Dh ; Elegir el modo de video
		INT 10h ; Ejecutar la configuración
		
		MOV AH,0Bh ; Establecer configuración
		MOV BH,00h ; Color de fondo
		MOV BL,00h ; Elegir el color negro como el color de fondo
		INT 10h ; Ejecutar la cofiguración
		
		RET
	CLEAR_SCREEN ENDP

CODE ENDS
END