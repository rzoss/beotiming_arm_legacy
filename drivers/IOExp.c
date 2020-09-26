/**
 *******************************************************************************
 * \file    IOExp.c
 *******************************************************************************
 * \brief    Driver and state machine for IO-Expander Modul over I2C (PCF8574)
 * 
 * \version		1.0
 * \date		24.04.2008
 * \author		R.Zoss, M. Leiser
 *
 *******************************************************************************
 */
#include <AT91SAM7S256.h>
#include <lib_AT91SAM7S256.h>
#include <irq.h>

#include "IOExp.h"
#include "i2c.h"
#include "uart.h"
#include <printf.h>
#include "global.h"
#include "sd-card.h"
#include "timer.h"


/**
 * \brief	aktuelle Input Zustände
 */
char inputs = 0;

/**
 * \brief	aktuelle Output Zustände
 */
char outputs = 0;

/**
 * \brief	aktuelle keyboard Tasten
 */
int keyboard = 0;

/**
 * \brief	aktuelle Schalter Zustände
 */
int switches = 0;
/**
 * \brief	Event aufgetreten
 */
int event = 0;


/**
 * \brief	IO Expander State Machine
 */
void IOE(){
	//TRACE("AUFRUF: IOE STATE MACHINE\n\r");
	FTRACE("AUFRUF: IOE STATE MACHINE\r\n");
	switch(ioe_state){
			
			case IDLE_IOE: 
				Look_for_Event();
				break;
//			case SWITCH_EVENT:
//				Switch_FSM();
//				
//				break;
			case KEYBOARD_EVENT:
				Keyboard_FSM();			
							
				break;
		}//end switch case
		
	
}


/**
 * \brief	IO Expander State Machine for SWITCHES
 * 
 */
//void Switch_FSM(){						//wie brauchen wir die switches überhaupt?nur zum löschen!
//	TRACE("AUFRUF: IOE Switch_FSM\r\n");
//	FTRACE("AUFRUF: IOE Switch_FSM\r\n");
//	switch(ioe_state){
//			
//			case SWITCH_IDLE: 
//				//wenn Enter Knopf 
//				
//				break;
//			case UP:
//				
//				
//				break;
//			case DOWN:
//			
//							
//				break;
//			case ENTER:
//					
//										
//				break;
//		}//end switch case
//		
//	
//}

/**
 * \brief	IO Expander State Machine for KEYBOARD (wird momentan nicht verwendet)
 * 
 */
void Keyboard_FSM(){
	TRACE("AUFRUF: IOE Keyboard_FSM\r\n");
	FTRACE("AUFRUF: IOE Keyboard_FSM\r\n");
	switch(ioe_state){
			
			case KEYBOARD_IDLE: 
				if(keyboard==8){			//Enter debug mode with Key 8 -> Zum entfernen der SD-Karte!
					ioe_keyboard_state=DEBUG1;
				}
				break;
			case DEBUG1:
				//TODO: DEBUG Menu errichten (um SD-Karte zu entfernen und Systemstatus abzufragen)
				
				break;
			case DEBUG2:
					
							
				break;
		}//end switch case
		
	
}





/**
 * \brief	setzt alle Ausgänge gemäss dem Parameter
 * 
 * \param[in]	byte	Neue Ausgangswerte
 * 
 * \return 	NO_ERROR on sucess or a error code (see i2c.h)
 */
int IOE_setOutputs(char byte){
	int i2c_Err=0;
	outputs = byte;
	i2c_Err=i2c_write(PCF8574ADR_OUT,0,0,&byte,1);
	return i2c_Err;
}

/**
 * \brief	liest alle Eingänge in die gewählte Variable
 * 
 * \param[out]	byte	aktuelle Eingangswerte
 * 
 * \return 	NO_ERROR on sucess or a error code (see i2c.h)
 */
int IOE_getInputs(char * byte){
	int i2c_Err=0;
	int readBytes;
	i2c_Err=i2c_read(PCF8574ADR_IN,0,0,byte,1,&readBytes);
	inputs = *byte;
	return i2c_Err;
}

/**
 * \brief	setzt ein bestimmter Ausgang, ohne die Anderen zu verändern
 * 
 * \param[in]	port	zu verändernder Ausgang
 * \param[in]	bool	Wert des Ausgangs (1 / 0)
 * 
 * \return 	NO_ERROR on sucess or a error code (see i2c.h)
 */
int IOE_setOneOutput(char port, char bool){
	int i2c_Err = 0;
	if((outputs & (1<<port)) == bool)
		return NO_ERROR;  // keine Änderung
	// Änderung anwenden
	if(bool)
		outputs |= (1<<port); 
	else
		outputs &= ~(1<<port);
	// Neuer Zustand setzen
	IOE_setOutputs(outputs);
	return i2c_Err;
}

/**
 * \brief	liest ein Eingang in die gewählte Variable
 * 
 * \param[in]	port	zu lesender Port
 * \param[out]	bool	aktuelle Eingangswerte
 * 
 * \return 	NO_ERROR on sucess or a error code (see i2c.h)
 */
int IOE_getOneInput(char port, char * bool){
	int i2c_Err = 0;
	// IOs lesen
	i2c_Err = IOE_getInputs(bool);
	// Maskieren auf gewünsten Port
	*bool &= (1<<port);
	return i2c_Err;
}

/**
 * \brief	Initialisierung für die IO-Expander. 
 * 			Diese Funktion initialisiert auch die I2C-Schnittstelle.
 * 
 * \return 	NO_ERROR
 */
int IOE_Init(){
	
	ioe_state=IDLE_IOE;
	//ioe_switch_state=SWITCH_IDLE;
	ioe_keyboard_state=KEYBOARD_IDLE;
	
	// Enable Peripheral clock in PMC for PIOA
	AT91F_PIOA_CfgPMC();
	// Alle Ausgänge auf Null setzen
	IOE_setOutputs(0);
	// Setze alle Eingänge des Input-Moduls hoch und lese sie anschliessend aus
	// Somit sind alle Eingänge als Inputs gesetzt
	char temp=0xFF;
	i2c_write(PCF8574ADR_IN,0,0,&temp,1);
	IOE_getInputs(&temp);
	
	// Beide Keyboard Inputs aud Logisch 0 halten 
	IOE_setOneOutput(IOEXP_OUT_KEYB_1,FALSE); 
	IOE_setOneOutput(IOEXP_OUT_KEYB_2,FALSE); 
	
	// Konfigurieren der Interrupts in Externer Source
	return NO_ERROR;
}

/**
 * \brief	ISR für die IO-Expander Interrupts
 * 
 * \return 	no return
 */
//void IOE_Handle_Interrupt(){
//	IOE_getInputs(&inputs);
//	if((inputs & 0xF)!=0xF){ // Keybord
//		IOE_setOneOutput(IOEXP_OUT_KEYB_2,TRUE); // nur Linie 1 auf '0' ziehen
//		IOE_getInputs(&inputs);
//		if((inputs & 0xF)!=0xF){ // Taste ist in dieser Reihe
//			switch(~inputs & 0xF){
//				case (1<<IOEXP_IN_KEYB_3): keyboard = 1; break;
//				case (1<<IOEXP_IN_KEYB_4): keyboard = 2; break;
//				case (1<<IOEXP_IN_KEYB_5): keyboard = 3; break;
//				case (1<<IOEXP_IN_KEYB_6): keyboard = 4; break;
//				default: /* nichts */ break;
//			}
//			IOE_setOneOutput(IOEXP_OUT_KEYB_2,FALSE); // wieder beide Linien treiben
//		}else{		// Taste ist aus der zweiten Reihe
//			IOE_setOneOutput(IOEXP_OUT_KEYB_2,FALSE); // nur Linie 2 auf '0' ziehen
//			IOE_setOneOutput(IOEXP_OUT_KEYB_1,TRUE); 
//			IOE_getInputs(&inputs);
//			switch(~inputs & 0xF){
//				case (1<<IOEXP_IN_KEYB_3): keyboard = 5; break;
//				case (1<<IOEXP_IN_KEYB_4): keyboard = 6; break;
//				case (1<<IOEXP_IN_KEYB_5): keyboard = 7; break;
//				case (1<<IOEXP_IN_KEYB_6): keyboard = 8; break;
//				default: /* nichts */ break;
//			}
//			IOE_setOneOutput(IOEXP_OUT_KEYB_1,FALSE); // wieder beide Linien auf '0' ziehen 
//		}
//	}else{ 		// Switches
//		switch(inputs & 0x70){
//			case (1<<IOEXP_IN_SW1): switches = 1; break;
//			case (1<<IOEXP_IN_SW2): switches = 2; break;
//			case (1<<IOEXP_IN_SW3): switches = 3; break;
//		}
//	}
//}

/**
 * \brief	Funktion zum Behandeln von aufgetretenen Events
 * 
 * \return 	no return
 */
void Look_for_Event(){
	static int flag=0;
	IOE_getInputs(&inputs);
		if((inputs & 0xF)!=0xF){ // Keyboard EVENT
			if(!flag){
			event=1;
			flag=1;
			IOE_setOneOutput(IOEXP_OUT_KEYB_2,TRUE); // nur Linie 1 auf '0' ziehen
			//ioe_state=KEYBOARD_EVENT;				  //GO TO Keyboard Handler State
			IOE_getInputs(&inputs);
			if((inputs & 0xF)!=0xF){ // Taste ist in dieser Reihe
				switch(~inputs & 0xF){
					case (1<<IOEXP_IN_KEYB_3): keyboard = 1; break;
					case (1<<IOEXP_IN_KEYB_4): keyboard = 2; break;
					case (1<<IOEXP_IN_KEYB_5): keyboard = 3; break;
					case (1<<IOEXP_IN_KEYB_6): keyboard = 4; break;
					default: /* nichts */ break;
				}
				IOE_setOneOutput(IOEXP_OUT_KEYB_2,FALSE); // wieder beide Linien treiben
			}else{		// Taste ist aus der zweiten Reihe
				IOE_setOneOutput(IOEXP_OUT_KEYB_2,FALSE); // nur Linie 2 auf '0' ziehen
				IOE_setOneOutput(IOEXP_OUT_KEYB_1,TRUE); 
				IOE_getInputs(&inputs);
				switch(~inputs & 0xF){
					case (1<<IOEXP_IN_KEYB_3): keyboard = 5; break;
					case (1<<IOEXP_IN_KEYB_4): keyboard = 6; break;
					case (1<<IOEXP_IN_KEYB_5): keyboard = 7; break;
					case (1<<IOEXP_IN_KEYB_6): keyboard = 8; break;
					default: /* nichts */ break;
				}
				IOE_setOneOutput(IOEXP_OUT_KEYB_1,FALSE); // wieder beide Linien auf '0' ziehen 
			}
			}
		}else if((inputs & 0x70)!=0x70){ 		// Switches
			if(!flag){
			event=1;
			flag=1;
			
			//ioe_state=SWITCH_EVENT;				//GO TO SWITCH Handler State
			switch(~inputs & 0x70){
				case (1<<IOEXP_IN_SW1): switches = 1; break;
				case (1<<IOEXP_IN_SW2): switches = 2; break;
				case (1<<IOEXP_IN_SW3): switches = 3; break;
			}
			}
		}else{
			flag=0;
		}

}
