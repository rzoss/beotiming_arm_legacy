/**
 *******************************************************************************
 * \file    uart.h
 *******************************************************************************
 * \brief   USART driver for AT91SAM7Sxxx
 * 			
 *
 * \version	1.0
 * \date	25.04.2008
 * \author  R.Zoss
 *
 *******************************************************************************
 */
#ifndef UART_H_
#define UART_H_

extern int i_Char;

/** \brief	Alle UART-Schnittstellen des Controllers*/
typedef enum _UART {UART0, UART1, UART_DBG} UART;

/** \brief	UART-Timeout*/
#define ERROR_USART_TIMEOUT				1


// Debuginformationen ein- bzw. ausschalten


#ifdef SMALL_DEBUG 
	/** \brief	Small Debug Funktionen werden mitkompiliert*/
	#define SMALL_TRACE(...) printf(__VA_ARGS__)
#else
	/** \brief	Small Debug Funktionen werden nicht mitkompiliert*/
	#define SMALL_TRACE(...)
#endif

#ifdef DEBUG  
	/** \brief	Debug Funktionen werden mitkompiliert */
	#define TRACE(...) printf(__VA_ARGS__)
#else
	/** \brief	Debug Funktionen werden nicht mitkompiliert*/
	#define TRACE(...)
#endif


int UART_init();
int UART_kbhit(UART uart);
int UART_getc(UART uart);
int UART_putc(UART uart, char c);
void timerTick1ms();
int UART_waitForRX(UART uart, int timeout);
int UART_waitForTX(UART uart, int timeout);

#endif /*USART_H_*/
