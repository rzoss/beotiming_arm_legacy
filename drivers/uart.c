/**
 *******************************************************************************
 * \file    uart.c
 *******************************************************************************
 * \brief    USART driver for AT91SAM7Sxxx.
 *
 *
 * \version		1.0
 * \date		25.04.2008
 * \author		R. Zoss
 *
 *******************************************************************************
 */

#include <AT91SAM7S256.h>
#include <lib_AT91SAM7S256.h>

#include "../system/printf.h"
#include "uart.h"
#include "uart_config.h"
#include "gsmgps.h"
#include "global.h"
#include "sd-card.h"


/**
 * \brief	Globale Variablen
 */
int ExtGsmInt=0;
int i_Char=0;

/**
 * \brief	Timer Zählvariable für internes Timeout
 */
volatile int uart_delayCntr = 0;	///< internal counter, decremented by a timer interrupt


/**
 * \brief	usart interrupt service routine
 * \return 	no return
 */
void UART_isr( void ) __attribute__((interrupt("IRQ")));
void UART_isr(){
char chr;

//	UART0 ISR
	if ((AT91C_BASE_US0->US_CSR & AT91C_US_RXRDY) == AT91C_US_RXRDY){
		chr = AT91F_US_GetChar((AT91PS_USART) AT91C_BASE_US0);	
	
		
		if (!(chr == '\0')){ 			//empfangenes Zeichen nicht 0
			if ((ExtSent == 0) ){ 		// nichts gesendet, WICHTIG: hier noch Variable startedup prüfen!
				//gsm_state = RECEIVE_SMS; 		//vorbereitet um SMS empfangen zu können (nichts gesendet aber etwas empfangen)
				
			}
			if ((i_Char >= (USART_ANSWER_LEN-1))){  
				// stop buffer overflow
				i_Char = 0;
			}
			answer[i_Char] = chr;
			answer[i_Char+1] = '\0';
			i_Char++;
			ExtGsmInt = 1;
			
			checkEndOfResponse(); //checks if End of Response reached
		}
		
		
		
		
		//gsmgps_to_debug(chr); 							//gibt den char weiter auf die debugschnittstelle
		
		//gsmgps_recieve_answer(chr);						//schreibt den Char in einen String.
		
	}
	
//	UART1 ISR
	
//	if ((AT91C_BASE_US1->US_CSR & AT91C_US_RXRDY) == AT91C_US_RXRDY){
//			chr = AT91F_US_GetChar((AT91PS_USART) AT91C_BASE_US1);	
//		
//			//gps_to_debug(chr); 							//gibt den char weiter nach gsmgps.c
//			
//	}
	AT91C_BASE_AIC->AIC_EOICR = 0x01;					// interrupt was processed
}

/**
 * \brief	Initialisiert die Seriellen Schnittstellen, die in uart_config.h 
 * 			konfiguriert und ausgewählt wurden.
 * 
 * 
 * \return 0=NO_ERROR	Sucessfull
 * */
int UART_init(){

	
			
	// confuigure RX and TX pins
	#ifdef	UART_USE_0
		AT91C_BASE_PIOA->PIO_PPUDR = AT91C_PA5_RXD0; //disable Pullup on RXD0
		AT91F_US0_CfgPMC();
		AT91F_PIO_CfgPeriph(
			AT91C_BASE_PIOA, // PIO controller base address
			UART_PIN_CONFIG_0, // Peripheral A
			0); // Peripheral B
	
		AT91F_US_Configure (
				(AT91PS_USART) AT91C_BASE_US0,
				MCK,
				UART_MODE_0,			
				UART_BAUD_0,			 	// baudrate to be programmed
				0);							// timeguard to be programmed
		
		// Enable Transmitter
		AT91F_US_EnableTx((AT91PS_USART) AT91C_BASE_US0);
		AT91F_US_EnableRx((AT91PS_USART) AT91C_BASE_US0);
	#endif
		
	#ifdef UART_USE_1	
		
		AT91C_BASE_PIOA->PIO_PPUDR = AT91C_PIO_PA21; 	//disable Pullup on RXD1
		AT91F_US1_CfgPMC();
		// Open PIO for UART1
		AT91F_PIO_CfgPeriph(
			AT91C_BASE_PIOA, // PIO controller base address
			UART_PIN_CONFIG_1, // Peripheral A
			0); // Peripheral B	
		
		AT91F_US_Configure (
				(AT91PS_USART) AT91C_BASE_US1,
				MCK,
				UART_MODE_1,			
				UART_BAUD_1,			 	// baudrate to be programmed
				0);							// timeguard to be programmed	
		
		// Enable Transmitter
		AT91F_US_EnableTx((AT91PS_USART) AT91C_BASE_US1);
		AT91F_US_EnableRx((AT91PS_USART) AT91C_BASE_US1);
	#endif
		
    #ifdef UART_USE_DBG
		AT91F_DBGU_CfgPMC();
		// Open PIO for DBGU
		AT91F_DBGU_CfgPIO();
		
		AT91F_US_Configure (
				(AT91PS_USART) AT91C_BASE_DBGU,
				MCK,
				UART_MODE_DBG,			
				UART_BAUD_DBG,			 	// baudrate to be programmed
				0);							// timeguard to be programmed
		// Enable Transmitter
		AT91F_US_EnableTx((AT91PS_USART) AT91C_BASE_DBGU);
		AT91F_US_EnableRx((AT91PS_USART) AT91C_BASE_DBGU);
	#endif
	
		
	// Zusätzliche Konfiguration für Interrupt
	AT91F_US_EnableIt(AT91C_BASE_US0, AT91C_US_RXRDY /*| AT91C_US_ENDRX*/);
	AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC, AT91C_ID_US0, 3,
							AT91C_AIC_SRCTYPE_POSITIVE_EDGE, UART_isr);
	AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_US0);
			
	//Enable the RX and TX PDC transfer requests
	AT91C_BASE_PDC_US0->PDC_PTCR = AT91C_PDC_TXTEN | AT91C_PDC_RXTEN;		
	return NO_ERROR;
}

/**
 * 
 * \brief		Waits until a byte was received over the serial port.
 * 
 * \param[in]	uart		UART-Schnittstelle (1 / 2 / DBG)
 * \param[in]	timeout		Timeout in ms
 * 
 * \return	0=NO_ERROR				Sucessfull\n
 * 			1=ERROR_USART_TIMEOUT	Timeout error
 * 
 * */
int UART_waitForRX(UART uart, int timeout){
	uart_delayCntr = timeout;
	switch(uart){
	case UART0: 
		while(!AT91F_US_RxReady(AT91C_BASE_US0) && (uart_delayCntr != 0));
		break;
	case UART1:
		while(!AT91F_US_RxReady(AT91C_BASE_US1) && (uart_delayCntr != 0));
		break;
	case UART_DBG:
		while(!AT91F_US_RxReady(AT91C_BASE_US1) && (uart_delayCntr != 0));
		break;		
	}
	if(uart_delayCntr == 0){
		return ERROR_USART_TIMEOUT;
	}
	return NO_ERROR;
}


/**
 * 
 * \brief		Waits until a byte was received over the serial port.
 * 
 * \param[in]	uart		UART-Schnittstelle (1 / 2 / DBG)
 * \param[in]	timeout		Timeout in ms
 * 
 * \return	0=NO_ERROR				Sucessfull\n
 * 			1=ERROR_USART_TIMEOUT	Timeout error
 * 
 * */
int UART_waitForTX(UART uart, int timeout){
	uart_delayCntr = timeout;
	switch (uart) {
	case UART0:
		while (!AT91F_US_TxReady(AT91C_BASE_US0) && (uart_delayCntr != 0));
		break;
	case UART1:
		while (!AT91F_US_TxReady(AT91C_BASE_US1) && (uart_delayCntr != 0));
		break;
	case UART_DBG:
		while (!AT91F_US_TxReady(AT91C_BASE_US1) && (uart_delayCntr != 0));
		break;
	}
	if(uart_delayCntr == 0){
		return ERROR_USART_TIMEOUT;
	}
	return NO_ERROR;
}
	
/**
 * \brief	check if character is in input buffer for specified usart 
 * 
 * \param[in]   uart	UART-Schnittstelle (1 / 2 / DBG)
 * 
 * \return 0=NO_ERROR	Sucessfull
 */
int UART_kbhit(UART uart){
	switch (uart) {
	case UART0:
		return AT91F_US_RxReady((AT91PS_USART) AT91C_BASE_US0) != 0;
		break;
	case UART1:
		return AT91F_US_RxReady((AT91PS_USART) AT91C_BASE_US1) != 0;
		break;
	case UART_DBG:
		return AT91F_US_RxReady((AT91PS_USART) AT91C_BASE_DBGU) != 0;
		break;
	}
	return NO_ERROR;
}

/**
 * \brief	check character in input buffer for specified usart 
 * 
 * \param[in]   uart	UART-Schnittstelle (1 / 2 / DBG)
 * 
 * \return read Character
 * 
 */
int UART_getc(UART uart){
	switch (uart) {
	case UART0:
		if (UART_kbhit(UART0))
			return AT91F_US_GetChar((AT91PS_USART) AT91C_BASE_US0);
		break;
	case UART1:
		if (UART_kbhit(UART1))
			return AT91F_US_GetChar((AT91PS_USART) AT91C_BASE_US1);
		break;
	case UART_DBG:
		if (UART_kbhit(UART_DBG))
			return AT91F_US_GetChar((AT91PS_USART) AT91C_BASE_DBGU);
		break;
	}
	return NO_ERROR;
}

/**
 * \brief		put character in output buffer for specified usart 
 * 
 * \param[in]   uart	UART-Schnittstelle (1 / 2 / DBG)
 * \param[in]   c		Zu übertragender Character
 * 
 * \return 0=NO_ERROR	Sucessfull
 * 
 */
int UART_putc(UART uart, char c){
	switch(uart) {
	case UART0:
		while(!AT91F_US_TxReady(AT91C_BASE_US0));
		AT91F_US_PutChar(AT91C_BASE_US0,c);
		break;
	case UART1:
		while(!AT91F_US_TxReady(AT91C_BASE_US1));
		AT91F_US_PutChar(AT91C_BASE_US1,c);
		break;
	case UART_DBG:
		while(!(AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_TXRDY));
		AT91C_BASE_DBGU->DBGU_THR = c;
		break;
	}
	return NO_ERROR;
}

/**
 * 
 * \brief	Timer tick. Used for internal delay();
 * 			Must be called from a timer interrupt routine every millisecond.
 * 
 * \return no return
 * */
void timerTick1ms(){
	if(uart_delayCntr){
		uart_delayCntr--;
	}
}


