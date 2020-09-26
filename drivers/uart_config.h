/**
 *******************************************************************************
 * \file    uart_config.h
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

#ifndef UART_CONFIG_H_
#define UART_CONFIG_H_


/** \brief	Benutze UART-Schnittstelle 0*/
#define UART_USE_0
/** \brief	Benutze UART-Schnittstelle 1*/
//#define UART_USE_1
/** \brief	Benutze UART-Schnittstelle DBG*/
#define UART_USE_DBG

#ifdef UART_USE_0
	/** \brief	Pins für UART 0 sind PA5 und PA6*/
	#define UART_PIN_CONFIG_0  ((unsigned int) AT91C_PA5_RXD0    )| \
						  	   ((unsigned int) AT91C_PA6_TXD0    )
	/** \brief	Modus von UART0 ist NO FLOW CONTROL*/
	#define UART_MODE_0  	  ( AT91C_US_USMODE_NORMAL + \
    							AT91C_US_NBSTOP_1_BIT + \
    							AT91C_US_PAR_NONE + \
    							AT91C_US_CHRL_8_BITS + \
    							AT91C_US_CLKS_CLOCK )		///< Content of the UART Mode register
	/** \brief	Baudrate von UART0 ist 38400 */					  	  		
	#define UART_BAUD_0 		38400   	
#endif


#ifdef UART_USE_1
	/** \brief	Pins für UART 1 sind PA21 und PA22*/
	#define UART_PIN_CONFIG_1   ((unsigned int) AT91C_PA22_TXD1    )| \
						  	  	((unsigned int) AT91C_PA21_RXD1    )
	/** \brief	Modus von UART0 ist Asynchrone*/
	#define UART_MODE_1  	  ( AT91C_US_USMODE_NORMAL + \
								AT91C_US_NBSTOP_1_BIT + \
								AT91C_US_PAR_NONE + \
								AT91C_US_CHRL_8_BITS + \
								AT91C_US_CLKS_CLOCK )		///< Content of the UART Mode register
	/** \brief	Baudrate von UART0 ist 4800 */					  	  		
	#define UART_BAUD_1 		4800   	


#endif


#ifdef UART_USE_DBG
	/** \brief	Modus von UART0 ist Asynchrone*/
	#define UART_MODE_DBG  	  ( AT91C_US_USMODE_NORMAL + \
								AT91C_US_NBSTOP_1_BIT + \
								AT91C_US_PAR_NONE + \
								AT91C_US_CHRL_8_BITS + \
								AT91C_US_CLKS_CLOCK )		///< Content of the UART Mode register
	/** \brief	Baudrate von UART0 ist 115200 */				  	  		
	#define UART_BAUD_DBG 		115200   	


#endif


#endif /*UART_CONFIG_H_*/
