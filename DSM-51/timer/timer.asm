LED EQU P1.7
;********* Ustawienie TIMERów *********
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
;50[ms] = 50 000[ŠS]*(11.0592[MHz]/12) =
; = 46 080 cykli = 180 * 256
TH0_SET EQU 256-180
TL0_SET EQU 0
;**************************************
	LJMP START
	ORG 100H
START:

	LCALL LCD_CLR
	MOV R0, #0

	LCALL WPROWADZ
	MOV R1, A ;godzina startowa
	LCALL WPROWADZ
	MOV R2, A ;minuta startowa
	LCALL WPROWADZ
	MOV R3, A ;sekunda startowa

	LCALL LCD_CLR	
	LCALL WAIT_ENTER
	LCALL LCD_CLR
	
	LCALL WPROWADZ
	MOV R4, A ;godziny alarmu
	LCALL WPROWADZ
	MOV R5, A ;minuty alarmu
	LCALL WPROWADZ
	MOV R6, A ;sekundy alarmu
	
	MOV TMOD,#TMOD_SET ;Timer 0 liczy czas
	MOV TH0,#TH0_SET ;Timer 0 na 50ms
	MOV TL0,#TL0_SET
	SETB TR0 ;start Timera

	
MINUTY: ;etykieta sprawdzajaca czy minelo 60 sekund, zeby dodac kolejna minute
	CJNE R3,#60, SEKUNDY ;sprawdz czy minelo 60 sekund, jak nie skocz do SEKUNDY
	
	MOV R3, #0 ;zeruje licznik sekund
	INC R2 ;dodaje minute
	
GODZINY: ;etykieta sprawdzajaca czy minelo 60 minut, zeby dodac kolejna godzine
	CJNE R2, #60, MINUTY
	
	MOV R2, #0 ;zeruje licznik minut
	INC R1 ;dodaje godzine
	
RESET: ;etykieta sprawdzajaca, czy minely 24 godziny i zerujaca je jezeli tak
	CJNE R1, #24, MINUTY

	MOV R1, #0 ;zeruje godzine
	
SEKUNDY: ;etykieta odliczajaca 1 sekundę i wyświetlająca czas

	MOV R7,#20 ;odczekaj czas 20*50ms=1s
	LCALL WYSWIETL

	MOV A, R1
	CJNE A, 04H, BRAK_ALARMU ;jesli godziny sie nie zgadzaja (04H to miejsce w pamieci wartosci rejestru R4 przechowujacego godzine alarmu) idz do brak alarmu
	MOV A, R2
	CJNE A, 05H, BRAK_ALARMU ;jesli minuty sie nie zgadzaja idz dalej (do etykiety BRAK ALARMU)
	MOV A, R3
	CJNE A, 06H, BRAK_ALARMU ;jesli sekundy sie nie zgadzaja idz do etykiety brak alarmu
	
	CPL LED
	CLR P1.5 ;wlaczamy brzeczyk
	MOV R0, #3 ;zaczynamy odliczanie 2 sekund przez ktore bedzie trwal alarm, 
	;3 poniewaz jedna sekunda przeskakuje natychmiast

BRAK_ALARMU: ;jesli brak alarmu, ale tez jesli byl alarm wykonujemy ta etykiete
	INC R3 ;zwiekszamy sekunde
	DJNZ R0, TIME_N50 
	;jezeli R0, zawierajacy czas trwania alarmu jest rozny od 0, zmniejszamy czas alarmu o 1 i przechodzimy dalej do TIME_N50 
	;pomijajac nastepne instrukcje wylaczenia brzeczyka
	
	CPL LED
	SETB P1.5 ;wylaczamy brzeczyk, pominiete jesli R7 rozne od 0

TIME_N50:
	JNB TF0,$ ;czekaj, aż Timer 0
	;odliczy 50ms
	MOV TH0,#TH0_SET ;TH0 na 50ms
	CLR TF0 ;zerowanie flagi timera 0
	DJNZ R7,TIME_N50 ;odczekanie N*50ms

	
	LJMP MINUTY ;skaczemy znowu do etykiety MINUTY i tak w kolko
	LJMP $

WPROWADZ:
	LCALL WAIT_KEY ; Wczytaj liczbę dziesiątek
	MOV B,#10 ; pomnóż
	MUL AB ; przez 10
	MOV R0,A ; zapisz liczbę w R0
	LCALL WAIT_KEY ;wczytaj liczbę jedności
	ADD A,R0 ; dodaj liczbę jedności do R0
	
	RET ; wyjdź z podprogramu. Wynik w A.
	
	
WYSWIETL:
	LCALL LCD_CLR
	
	MOV B, #10
	MOV A, R1
	DIV AB ;start bcd
	SWAP A
	ORL A, B;end bcd
	LCALL WRITE_HEX
	
	MOV A, #';'
	LCALL WRITE_DATA
	
	MOV A, R2
	MOV B, #10
	DIV AB ;start bcd
	SWAP A
	ORL A, B;end bcd
	LCALL WRITE_HEX
	
	MOV A, #';'
	LCALL WRITE_DATA
	
	MOV A, R3
	MOV B, #10
	DIV AB ;start bcd
	SWAP A
	ORL A, B;end bcd
	LCALL WRITE_HEX
	
	RET

