/**
 *******************************************************************************
 * \file    timer.c
 *******************************************************************************
 * \brief    TC Konfiguration und delay ms
 *  
 * \version		1.2
 * \date		13.04.2008
 * \author		M. Mühlemann, R. Zoss
 *
 *******************************************************************************
 */
#include <limits.h>
#include "AT91SAM7S256.h"
#include "lib_AT91SAM7S256.h"

#include "global.h"
#include "timer.h"
#include "display.h"
#include "../filesystem/diskio.h"
#include "uart.h"
#include "../system/irq.h"


/**
 *******************************************************************************
 * timer_rfid
 * \brief  Timer Zählvariable für RFID (2 Sekunden)
 *******************************************************************************
 */
volatile int timer_rfid = 2000;
/**
 *******************************************************************************
 * timer_display
 * \brief  Timer Zählvariable für Displaybeleuchtung (15 Sekunden)
 *******************************************************************************
 */
volatile int timer_display = 15000;

/**
 *******************************************************************************
 * flag50
 * \brief  Timer Zählvariable für Hauptzyklus (50ms)
 *******************************************************************************
 */
volatile int flag50 = 50;

/**
 *******************************************************************************
 * hw_on_timer
 * \brief  Timer Zählvariable für hw_on (1000ms)
 *******************************************************************************
 */
volatile int hw_on_timer = 1000;

/**
 *******************************************************************************
 * atTimeout
 * \brief  Timer Zählvariable für Timeout bei AT Befehlen (1 Sekunde)
 *******************************************************************************
 */
volatile int atTimeout = 1000;

/**
 *******************************************************************************
 * timer2000
 * \brief  Timer Zählvariable für Timeout bei Displayausgaben (2 Sekunden)
 *******************************************************************************
 */
volatile int timer2000 = 2000;

///**
// *******************************************************************************
// * flag200
// * \brief  Timer Zählvariable für IO (200ms)
// *******************************************************************************
// */
//volatile int flag200 = 200;

/**
 *******************************************************************************
 * flag1050
 * \brief  Timer Zählvariable fürs ein-/ausschalten des GSM Moduls (1050ms)
 *******************************************************************************
 */
volatile int flag1050 = 1050;

/**
 *******************************************************************************
 * timeoutCtr
 * \brief  Timer Zählvariable für delay
 *******************************************************************************
 */
volatile int timeoutCtr = 0;

/**
 * \fn int TC0_init()
 ******************************************************************************
 * \brief		Initialize Timer Counter 0
 ******************************************************************************
 * \return		NO_ERROR	Fehlerfreie Initialisierung
 ******************************************************************************
 */
int TC0_init(){
	
	AT91S_TC* pTC0 = AT91C_BASE_TC0;
	
	AT91F_TC0_CfgPMC();					// enable peripheral clock

	pTC0->TC_CMR = AT91C_TC_CPCTRG;		// TIMER_CLOCK1 = MCK/2
										// MCK/2 = 24 MHz											

	pTC0->TC_CCR = AT91C_TC_CLKDIS;		// clock disable
	int dummy = AT91C_BASE_TC0->TC_SR;	// clear overflow bits
	dummy = dummy;						// to prevent compiler warning
	pTC0->TC_IDR = 0xffffffff;			// disable all interrupts	
	pTC0->TC_CCR = AT91C_TC_CLKEN;		// clock enable
	pTC0->TC_RC = MCK/2/1000;			// ~1 ms	
	
	// configure interrupt controller
	AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC,
							AT91C_ID_TC0,
							1,
							AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE,
							handler_timerIRQ);
	AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_TC0);
	pTC0->TC_IER = AT91C_TC_CPCS;  //  IRQ enable CPC

	pTC0->TC_CCR = AT91C_TC_SWTRG;		// reset counter, start clock
	ENABLE_INTERRUPTS
	return NO_ERROR;
}

/**
 * \fn 		int delay(int ms)	
 ******************************************************************************
 * \brief	delays the count of milliseconds. First TC0 must be initialized
 * 			with TC0_init().
 ******************************************************************************
 * \param	ms	delay in ms
 * \return	NO_ERROR
 ******************************************************************************
 */
int delay(int ms){
	if(ms>INT_MAX)
		return ERROR_TIMER_RANGE;
	AT91C_BASE_TC0->TC_CCR = AT91C_TC_SWTRG;		// reset counter, start clock
	timeoutCtr = ms;
	while(timeoutCtr);

	return NO_ERROR;
}


/**
 * \fn 		void handler_timerIRQ( void )
 ******************************************************************************
 * \brief		Timer Interrupt-Service-Routine
 * 				Invoked every 1 ms
 ******************************************************************************
 * \return		no return
 ******************************************************************************
 */
void handler_timerIRQ( void ) __attribute__((interrupt("IRQ")));
void handler_timerIRQ( void ){
	static char mmc_timerproc_counter = 10;
	
	int dummy = AT91C_BASE_TC0->TC_SR; // clear overflow bits
	dummy = dummy; // to prevent comiler warnings
	
	//alle Timervariablen dekrementieren, wenn nötig
	if(timeoutCtr > 0){
		timeoutCtr--;
	}
	
	if(flag50){
		flag50--;
	}
	
	if(timer2000){
		timer2000--;
	}
	
	if(hw_on_timer){
			hw_on_timer--;
		}
	
	if(--timer_display <= 0){
		
		//Displaybeleuchtung ausschalten
		AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, PIN_DISP_LED); 
		
	}
	
	if(timer_rfid){
		timer_rfid--;
	}
	
	if(atTimeout){
		atTimeout--;
	}

	
	if(--mmc_timerproc_counter == 0){ // Funktion wird alle 10 ms gerufen. 
		disk_timerproc();
		mmc_timerproc_counter=10;
	}
	
	// Funktion für Uart internes delay
	timerTick1ms();
	
	
	AT91C_BASE_AIC->AIC_EOICR =0x01;
	
}
