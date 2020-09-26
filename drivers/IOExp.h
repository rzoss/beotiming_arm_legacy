/**
 *******************************************************************************
 * \file    IOExp.h
 *******************************************************************************
 * \brief    Driver for IO-Expander Modul over I2C (PCF8574)
 * 
 * \version		1.0
 * \date		24.04.2008
 * \author		R.Zoss
 *
 *******************************************************************************
 */

#ifndef IOEXP_H_
#define IOEXP_H_

/**
 * \brief  Adresse des input IO-Expanders
 */
#define PCF8574ADR_IN	0x20
/**
 * \brief  Adresse des output IO-Expanders
 */
#define PCF8574ADR_OUT	0x21

// IOpins
/**
 * \brief  IO-Epanderinterrupt auf Pin PA26
 */
#define PIN_INT_OUT		AT91C_PIO_PA26
/**
 * \brief  IO-Epanderinterrupt auf Pin PA30
 */
#define PIN_INT_IN		AT91C_PIO_PA30

// Outputs
/**
 * \brief  IO-Expander output P0 ist die rote LED
 */
#define IOEXP_OUT_LED_ROT	0
/**
 * \brief  IO-Expander output P1 ist die grüne LED
 */
#define IOEXP_OUT_LED_GRUEN	1
/**
 * \brief  IO-Expander output P2 ist eine Leitung des Keyboards
 */
#define IOEXP_OUT_KEYB_1	2
/**
 * \brief  IO-Expander output P3 ist eine Leitung des Keyboards
 */
#define IOEXP_OUT_KEYB_2	3

// Inputs
/**
 * \brief  IO-Expander input P0 ist eine Leitung des Keyboards
 */
#define IOEXP_IN_KEYB_3		0
/**
 * \brief  IO-Expander input P1 ist eine Leitung des Keyboards
 */
#define IOEXP_IN_KEYB_4		1
/**
 * \brief  IO-Expander input P2 ist eine Leitung des Keyboards
 */
#define IOEXP_IN_KEYB_5		2
/**
 * \brief  IO-Expander input P3 ist eine Leitung des Keyboards
 */
#define IOEXP_IN_KEYB_6		3
/**
 * \brief  IO-Expander input P4 ist ein Taster
 */
#define IOEXP_IN_SW1		4
/**
 * \brief  IO-Expander input P5 ist ein Taster
 */
#define IOEXP_IN_SW2		5
/**
 * \brief  IO-Expander input P6 ist ein Taster
 */
#define IOEXP_IN_SW3		6



/**
 * \brief	Enumeration for IOE State machine
 */
int ioe_state;
enum IOE_STATE{
	IDLE_IOE,
//	SWITCH_EVENT,
	KEYBOARD_EVENT
};


/**
 * \brief	Enumeration for Keyboard State
 */
int ioe_keyboard_state;
enum KEYBOARD_EVENT{
	KEYBOARD_IDLE,
	DEBUG1,
	DEBUG2
};

// Variables
extern int event;
extern int keyboard;
extern int switches;

// Prototypes
void IOE();
//void Switch_FSM();
void Keyboard_FSM();
int IOE_Init();
int IOE_setOutputs(char byte);
int IOE_setOneOutput(char port, char bool);
int IOE_getInputs(char * byte);
int IOE_getOneInput(char port, char * bool);
void Look_for_Event();
void IOE_Handle_Interrupt();

#endif /* IOEXP_H_ */


