/**
 *******************************************************************************
 * \file    display.h
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

#ifndef DISPLAY_H_
#define DISPLAY_H_



// IO pins
/**
 * \brief	Display RS ist Pin PA17
 */
#define PIN_DISP_RS			AT91C_PIO_PA17
/**
 * \brief	Display Reset ist Pin PA18
 */
#define PIN_DISP_RESET		AT91C_PIO_PA18
/**
 * \brief	Display LED ist Pin PA28
 */
#define PIN_DISP_LED		AT91C_PIO_PA28

// Error
/**
 * \brief	Die Positions oder Zeilen Nummer ist ungültig
 */
#define ERROR_DISP_OUT_OF_AREA				1


// Display Commands
/**
 * \brief	löscht das Display
 */
#define CLEAR_DISPLAY 						0x01
/**
 * \brief	setzt den Cursor auf die Home Position (0,0)
 */
#define RETURN_HOME							0x02
/**
 * \brief	display shift
 */
#define CURSOR_SHIFT 						0x06
/**
 * \brief	Display ein, Cursor ein, Position aus
 */
#define DISPLAY_ON_CURSOR_ON_POSITION_OFF	0x0E
/**
 * \brief	Display ein, Cursor aus, Position ein
 */
#define DISPLAY_ON_CURSOR_OFF_POSITION_ON	0x0D
/**
 * \brief	Display ein, Cursor ein, Position ein
 */
#define DISPLAY_ON_CURSOR_ON_POSITION_ON	0x0F
/**
 * \brief	Display ein, Cursor aus, Position aus
 */
#define DISPLAY_ON_CURSOR_OFF_POSITION_OFF	0x0C
/**
 * \brief	Display aus
 */
#define DISPLAY_OFF							0x08

/**
 * \brief	Befehlstabelle 0 wählen
 */
#define INSTRUCTION_TABLE_0					0x00
/**
 * \brief	Befehlstabelle 1 wählen
 */
#define INSTRUCTION_TABLE_1					0x01
/**
 * \brief	Befehlstabelle 2 wählen
 */
#define INSTRUCTION_TABLE_2					0x02

/**
 * \brief	Cursor setzen
 */
#define SET_CURSOR_ADDRESS					0x80
/**
 * \brief	Zeile 1 und 2 zusammenfassen
 */
#define DOUBLE_HIGH_POS_SEL_1_2				0x18
/**
 * \brief	Zeile 2 und 3 zusammenfassen
 */
#define DOUBLE_HIGH_POS_SEL_2_3				0x10
/**
 * \brief	Alles einzeilig
 */
#define DOUBLE_HIGH_POS_SEL					0x2C

// Prototypes
int Disp_Init();
int Disp_Command(int Data);
int Disp_Char(int Data);
int Disp_Clear();
int Disp_SetCursor(int line, int position);
int Disp_WriteLine(char * string, int lineNbr);
int Disp_EraseLine(char * string, int lineNbr);
int Disp_DoubleHigh(char bool, int position);
inline int Disp_LED_manual(char bool);
int Disp_LED_auto();

#endif /*DISPLAY_H_*/
