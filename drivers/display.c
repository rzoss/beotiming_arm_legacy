/**
 *******************************************************************************
 * \file    display.c
 *******************************************************************************
 * \brief    Display driver for EA Dog-M.
 *
 *
 * \version		1.0
 * \date		07.03.2008
 * \author		R.Zoss
 *
 *******************************************************************************
 */

#include <AT91SAM7S256.h>
#include <lib_AT91SAM7S256.h>

#include "global.h"
#include "spi.h"
#include "display.h"
#include "timer.h"
#include "../system/irq.h"

/**
 * \brief   Speicher die aktuelle Displaykonfiguration
 */
char disp_function = 0x38; // Function Set ohne Double High


/**
 * \fn 		int Disp_Command(int Data)
 * 
 * \brief  	Diese Funktion sendet einen Befehl an das Display. (RS-PIN tief)
 *  
 * \param[in]  Data   Displaybefehl (1 Byte)
 * 
 * \return NO_ERROR    Fehlerfreie Übertragung
 **/
int Disp_Command(int Data)
{
	int i;
	// Command Mode vorbereiten; setze RS low
	AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, PIN_DISP_RS);

	/* Starte übertragung */
	spi_write(Data,1,0,1);
	
	//Übertragung abschließen durch RS toggle
	AT91F_PIO_SetOutput(AT91C_BASE_PIOA, PIN_DISP_RS);	
	AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, PIN_DISP_RS);
	
	// Warten > 26.3 us
	for(i=0;i<200;i++); //waits 33us (200 * 8 Instructions)
	return NO_ERROR;
}

/**
 * \fn 			int Disp_Char(int Data)
 * \brief      Diese Funktion sendet ein Zeichen an das Display. (RS-PIN hoch)
 * 
 * \param[in]  Data   Zeichen (1 Byte)
 * 
 * \return NO_ERROR		Fehlerfreie Übertragung
 * 
 **/
int Disp_Char(int Data)
{
	int i;
	// Command Mode vorbeireiten; setze RS low
	AT91F_PIO_SetOutput(AT91C_BASE_PIOA, PIN_DISP_RS);

	/* Starte übertragung */
	spi_write(Data,1,0,1);
	
	//Übertragung abschließen durch RS toggle
	AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, PIN_DISP_RS);	
	AT91F_PIO_SetOutput(AT91C_BASE_PIOA, PIN_DISP_RS);
	
	// Warten > 26.3 us
	for(i=0;i<200;i++); //waits 33us (200 * 8 Instructions)
	return NO_ERROR;
}

/**
 * \fn		int Disp_Clear()
 * \brief	Diese Funktion löscht das Display. Die dazu benötigte Zeit wird gewartet.
 * 
 * \return NO_ERROR		Fehlerfreie Übertragung
 **/
int Disp_Clear(){
	Disp_Command(CLEAR_DISPLAY);
	delay(50);
	Disp_Command(CURSOR_SHIFT);
	delay(50);
	return NO_ERROR;
}

/**
 * \fn			int Disp_SetCursor(int line, int position)
 * \brief		Diese Funktion setzt den Cursor im Diplay an die gewünschte Stelle.
 * 
 * \param[in] line 	Linie (1-3)
 * \param[in] position	Position in der Linie (1-16)
 * 
 * \return NO_ERROR		Fehlerfreie Übertragung
 **/
int Disp_SetCursor(int line, int position){
	if(line > 3 || position > 16){
		return ERROR_DISP_OUT_OF_AREA;
	}
	Disp_Command(disp_function | INSTRUCTION_TABLE_0);
	Disp_Command(SET_CURSOR_ADDRESS | ((line-1)<<4) | (position-1));
	return NO_ERROR;
}


/**
 * \fn		int Disp_EraseLine(char * string, int lineNbr)
 * \brief	Diese Funktion schreibt einen String auf die gewählte Zeile ohne den Beleuchtungstimer raufzustellen. Falls der
 * 			String länger als 16 Zeichen ist, wird er auf der nächsten Zeile weitergeführt.
 * 
 * \param[in]   string		Zeichenkette
 * \param[in]	lineNbr 	Zeilennummer (1-3)
 * 
 * \return   NO_ERROR	  Fehlerfreie Übertragung
 **/
int Disp_EraseLine(char * string, int lineNbr){
	int disp_Err = 0;
	disp_Err = Disp_SetCursor(lineNbr,1);
	if(disp_Err == ERROR_DISP_OUT_OF_AREA)
		return ERROR_DISP_OUT_OF_AREA;
	while (*string){
		
		Disp_Char(*string);
		++string;
	}
	return NO_ERROR;
}


/**
 * \fn		int Disp_WriteLine(char * string, int lineNbr)
 * \brief	Diese Funktion schreibt einen String auf die gewählte Zeile. Displaybeleuchtung wird eingeschaltet. Falls der
 * 			String länger als 16 Zeichen ist, wird er auf der nächsten Zeile weitergeführt.
 * 
 * \param[in]   string		Zeichenkette
 * \param[in]	lineNbr 	Zeilennummer (1-3)
 * 
 * \return   NO_ERROR	  Fehlerfreie Übertragung
 **/
int Disp_WriteLine(char * string, int lineNbr){
	Disp_LED_manual(TRUE);
	timer_display=15000; 		//15 sekunden Displaybeleuchtungstimer setzen
	Disp_EraseLine(string, lineNbr);
	
	return NO_ERROR;
}

/**
 * \brief 	Schaltet entweder Zeile 1 und 2 oder die Zeile 2 und 3 zusammen.
 * 
 * \param[in]    bool 			Ein- / Ausschalten
 * \param[in]	 position		TRUE = 2 und 3, FALSE = 1 und 2
 * 
 * \return   NO_ERROR	  Fehlerfreie Ausführung
 * 
 **/
int Disp_DoubleHigh(char bool, int position){
	if(bool){
		Disp_Command(disp_function | INSTRUCTION_TABLE_2);
		if(position)
			Disp_Command(DOUBLE_HIGH_POS_SEL_2_3);
		else
			Disp_Command(DOUBLE_HIGH_POS_SEL_1_2);
		disp_function |= (1<<2); // Double High bit setzen 
		Disp_Command(disp_function | INSTRUCTION_TABLE_0);
	}else{
		disp_function &= ~(1<<2); // Double High bit löschen 
		Disp_Command(disp_function | INSTRUCTION_TABLE_0);
	}
	return NO_ERROR;
}

/**
 * \fn	   int Disp_Init()
 * \brief  Initialisation des Displays. Diese Funktion startet
 * 		   ebenfalls den Timer 0 und dessen Interrupts.
 * 
 * \return   NO_ERROR	  Fehlerfreie Ausführung
 * */
int Disp_Init(){
	// Timer initialisieren 
	//TC0_init();
	
	// Enable Peripheral clock in PMC for PIOA
	AT91F_PIOA_CfgPMC();
	// Configure Pins to direct drive
	AT91F_PIO_CfgDirectDrive(AT91C_BASE_PIOA, PIN_DISP_RS | PIN_DISP_RESET  | PIN_DISP_LED);
	// configure IOs: RS, Reset
	AT91F_PIO_CfgOutput(AT91C_BASE_PIOA, PIN_DISP_RS | PIN_DISP_RESET | PIN_DISP_LED);
	// Initialisieren des Resets des Diplays
	AT91F_PIO_SetOutput(AT91C_BASE_PIOA, PIN_DISP_RESET);
	// löschen der Outputs und Reset des Diplays
	AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, PIN_DISP_RS | PIN_DISP_RESET);
	// Beenden des Display Resets
	delay(1);
	AT91F_PIO_SetOutput(AT91C_BASE_PIOA, PIN_DISP_RESET | PIN_DISP_LED);
	
	// Initialisierung des SPI-Controllers
	spi_init();
	// Warten > 40 ms	
	delay(40);
	
	// Initialisierung für 3.3 V
	Disp_Command(disp_function | INSTRUCTION_TABLE_1); // 8-Bit Datenlänge, 3 Zeilen, Instruction table 1
	Disp_Command(0x15);	// BS: 1/5, 3-zeiliges LCD
	Disp_Command(0x55); // Booster ein, Kontrast C5, C4setzen
	Disp_Command(0x6E); // Spannungsfolger und Verstärkung setzen
	delay(200);
	Disp_Command(0x70); // Kontrast C3, C2, C1 setzen
	Disp_Command(disp_function | INSTRUCTION_TABLE_0); // Instuction table 0
	Disp_Command(DISPLAY_ON_CURSOR_OFF_POSITION_OFF); // Display ein, Cursor ein, Cursor blinken
	Disp_Clear();
	return NO_ERROR;
	
}

/**
 * \brief	Schaltet die Displaybeleuchtung ein bzw. aus.
 * 
 * \param[in]	bool	TRUE = ein, FALSE = aus
 * */
inline int Disp_LED_manual(char bool){
	
	if(bool) // Display LED ein
		AT91F_PIO_SetOutput(AT91C_BASE_PIOA, PIN_DISP_LED);
	else
		AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, PIN_DISP_LED);
	return NO_ERROR;	
}

