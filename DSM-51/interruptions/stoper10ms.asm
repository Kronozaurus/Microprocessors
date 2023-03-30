;********* Ustawienie TIMERÓW *********
LED EQU P1.7
;TIMER 0
T0_G EQU 0 ;GATE
T0_C EQU 0 ;COUNTER/-TIMER
T0_M EQU 1 ;MODE (0..3)
TIM0 EQU T0_M+T0_C*4+T0_G*8
;TIMER 1
T1_G EQU 0 ;GATE
T1_C EQU 0 ;COUNTER/-TIMER
T1_M EQU 0 ;MODE (0..3)
TIM1 EQU T1_M+T1_C*4+T1_G*8
TMOD_SET EQU TIM0+TIM1*16


TH0_SET EQU 256-36 ;10 ms
TL0_SET EQU 0


;**************************************
	LJMP START
;********* Przerwanie Timer 0 *********
	ORG 0BH
	MOV TH0,#TH0_SET ;TH0 na 10ms
	LCALL INC_SECOND ;wykonywane co 1 sek
	RETI
;**************************************
	ORG 100H
START:
	LCALL LCD_CLR
	MOV TMOD,#TMOD_SET ;Timer 0 liczy czas
	MOV TH0,#TH0_SET ;Timer 0 na 10ms
	MOV TL0,#TL0_SET
	SETB EA ;włącz zezwolenie ogólne na przerwania
	SETB ET0 ;włącz zezwolenie timera 0 na przerwania
	MOV R7,#100 ;zainicjuj R7: 100*10ms=1s
	MOV R6, #0 ;odliczanie sekund
	MOV R5, #0 ;odliczanie ms
	MOV R4, #10 ;odliczanie 10 sekund do sygnalizacji
	
	LCALL WAIT_ENTER
	SETB TR0 ;start Timera 0
	
DISP_LOOP:

	LCALL WAIT_KEY 
	CLR TR0 ;zatrzymanie stopera
	LCALL WAIT_KEY 
	SETB TR0 ;wznowienie stopera
	
	SJMP DISP_LOOP
	SJMP $ 

INC_SECOND:
	CJNE R6, #100, NO_RESET ;sprawdz, czy licznik przekroczyl wartosc 100 sekund
	MOV R6, #0 ;jesli tak, resetuj licznik, jesli nie skocz dalej
	
NO_RESET:
	DJNZ R7, INC_MS ;sprawdz czy minela sekunda, 
	;jesli nie zwieksz licznik ms i zmniejsz r7
	
	MOV R7,#100 ;ustaw ponownie R7: 100*10ms=1s
	MOV R5, #0 ;zresetuj wartosc ms
	SETB P1.5 ;wylaczenie brzeczyka po 1 sekundzie
	
	LCALL DISPLAY
	
	INC R6 ;zwiekszenie wartosci sekund
	
	DJNZ R4, INC_MS ;warunek obslugujacy sygnal dzwiekowy
	CLR P1.5 ;kiedy minie 10 sekund, zabrzeczy na sekunde
	MOV R4, #10 ;reset licznika 10 sekund 
	
	
INC_MS:
	INC R5 ;zwiekszenie wartosci ms
	LCALL DISPLAY
	
	RET ;powrót z podprogramu
	NOP

DISPLAY:
	LCALL LCD_CLR
	
	MOV A, R6
	MOV B, #10
	DIV AB ;start bcd
	SWAP A
	ORL A, B;end bcd
	
	LCALL WRITE_HEX
	
	MOV A, #','
	LCALL WRITE_DATA
	
	MOV A, R5
	MOV B, #10
	DIV AB ;start bcd
	SWAP A
	ORL A, B;end bcd

	LCALL WRITE_HEX
	
	RET
	NOP
	

