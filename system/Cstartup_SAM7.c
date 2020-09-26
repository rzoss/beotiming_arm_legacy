/**
 *******************************************************************************
 * \file    CStartup_SAM7.c
 *******************************************************************************
 * \brief * Low-Level initialisation like:
 * - Watchdog
 * - Clocks
 * - Interrupt controller
 * 
 * \version	1.0
 * \date	27.11.06
 * \author	M.Muehlemann
 *
 *
 *******************************************************************************
 */

// Include the board file description
#include "AT91SAM7S256.h"
#include "lib_AT91SAM7S256.h"
#include "irq.h"

/**
 ******************************************************************************
 * function    	: AT91F_LowLevelInit()
 ******************************************************************************
 * \brief
 * Low-Level initialisation like:
 * - Watchdog
 * - Clocks
 * - Interrupt controller
 ******************************************************************************
 * \param     no parameters
 * \return    no return
 ******************************************************************************
 */
void AT91F_LowLevelInit( void)
{
    int            i;
    AT91PS_PMC     pPMC = AT91C_BASE_PMC;

    //* Watchdog Disable
    AT91C_BASE_WDTC->WDTC_WDMR= AT91C_WDTC_WDDIS;
    
    *AT91C_MC_FMR = AT91C_MC_FWS_1FWS;

    //* Set Master-Clock at 48 MHz and the Core-Clock at 96 MHz
    // 1 Enabling the Main Oscillator:
    // SCK = 1/32768 = 30.51 uSecond
    // Start up time = 8 * 6 / SCK = 56 * 30.51 = 1,46484375 ms
    pPMC->PMC_MOR = (( (AT91C_CKGR_OSCOUNT & (0x06 <<8) )| AT91C_CKGR_MOSCEN ));
    
    // Wait the startup time
    while(!(pPMC->PMC_SR & AT91C_PMC_MOSCS));
    
    // 3 Setting PLL and divider:
    // PLL Clock = 96.1097 MHz = (18.432/14*73)
	pPMC->PMC_PLLR =
	  ((AT91C_CKGR_DIV & 0x0E) |		// DIV =14 Fin = 1.3165 =(18,432 / 14)
	  (AT91C_CKGR_PLLCOUNT & (28<<8)) | // PLLCOUNT 28 = 0.000844 /(1/32768)
	  (AT91C_CKGR_MUL & (0x48<<16)) 	// MUL 25+1: Fout =	96.1097 =(3,6864 * 73)
	  );

    // Wait the startup time
    while(!(pPMC->PMC_SR & AT91C_PMC_LOCK));
    while(!(pPMC->PMC_SR & AT91C_PMC_MCKRDY));
    
    // 4. Selection of Master Clock and Processor Clock
    // select the PLL clock divided by 2 (MCK = 48.05 MHz)
    pPMC->PMC_MCKR =  AT91C_PMC_PRES_CLK_2 ;
    while(!(pPMC->PMC_SR & AT91C_PMC_MCKRDY));

    pPMC->PMC_MCKR |= AT91C_PMC_CSS_PLL_CLK  ;
    
    // wait until the master clock is ready
    while(!(pPMC->PMC_SR & AT91C_PMC_MCKRDY));

    // Set up the default interrupts handler vectors
    AT91C_BASE_AIC->AIC_SVR[0] = (int) AT91F_Default_FIQ_handler ;
    
    for (i=1;i < 31; i++)
    {
      AT91C_BASE_AIC->AIC_SVR[i] = (int) AT91F_Default_IRQ_handler ;
    }
    AT91C_BASE_AIC->AIC_SPU  = (int) AT91F_Default_Spurious_handler ;

}

