/**
 *******************************************************************************
 * \file    gpio.c
 *******************************************************************************
 * \brief    GPIO Interrupt handler and init function.
 * 
 * \version		1.0
 * \date		21.04.2008
 * \author		R.Zoss
 *
 *******************************************************************************
 */

#include <printf.h>
#include <AT91SAM7S256.h>
#include <lib_AT91SAM7S256.h>
#include <irq.h>

#include "global.h"
#include "rfid.h"
#include "IOExp.h"
#include "uart.h"
#include "gpio.h"

/**
 *	\brief 	   Interrupt service routine for GPIOs.
 * 
 *	\return    function has no return
 */
void isr_gpio() __attribute__((interrupt("IRQ")));
void isr_gpio(){

	// clear the interrupt by reading this register
	unsigned int isr;
	while((isr = AT91C_BASE_PIOA->PIO_ISR) & AT91C_BASE_PIOA->PIO_IMR){
			
		if(isr & PIN_INT_IN){	// IO-Exp In Interrupt		
			IOE_Handle_Interrupt();
			TRACE("IOExp Interrupt, Taste %d, Switch %d\n\r",keyboard,switches);
			// Löschen der inzwischen angekommenen Interrupts
			isr = AT91C_BASE_PIOA->PIO_ISR & AT91C_BASE_PIOA->PIO_IMR;
		}else if(isr & PIN_RFID_IN){ // RFID Tag-IN Interrupt
			TRACE("RFID Karte im Feld \r\n");
		}
	}
	
	TRACE("Verlassen der ISR\n\r");
	// indicate that the isr is done
	AT91C_BASE_AIC->AIC_EOICR = 0x00000000;			
}

/**
 *	\brief	   initializes GPIOs and the Interrupt controller, 
 * 
 *	\return    NO_ERROR
 ******************************************************************************
 */
int GPIO_irq_init(){
	
	// configure GPIO controller
	AT91F_PIOA_CfgPMC(); 							// Enable Peripheral clock in PMC for PIOA	
	AT91F_PIO_CfgInput(AT91C_BASE_PIOA,PIN_INT_IN);	// configure PA30 as input
	AT91F_PIO_InterruptEnable(AT91C_BASE_PIOA,PIN_INT_IN);	// enable interrupts for PA30
	AT91F_PIO_CfgInput(AT91C_BASE_PIOA,PIN_RFID_IN);	// configure PA30 as input
	AT91F_PIO_InterruptEnable(AT91C_BASE_PIOA,PIN_RFID_IN);	// enable interrupts for PA30	
	
	// configure Interrupt controller
	AT91C_BASE_AIC->AIC_IDCR = 1 << AT91C_ID_PIOA;		//disable GPIO interrupts before chaning interrupt settings
	AT91C_BASE_AIC->AIC_SVR[AT91C_ID_PIOA] = (unsigned)isr_gpio;
	AT91C_BASE_AIC->AIC_SMR[AT91C_ID_PIOA] = AT91C_AIC_SRCTYPE_EXT_NEGATIVE_EDGE;
	AT91C_BASE_AIC->AIC_ICCR = 1 << AT91C_ID_PIOA;
	AT91C_BASE_AIC->AIC_IECR = 1 << AT91C_ID_PIOA;
	
	unsigned long isr = isr= AT91C_BASE_PIOA->PIO_ISR; // clear PIO Interrupt Status Register
	ENABLE_INTERRUPTS;
	return NO_ERROR;
}



