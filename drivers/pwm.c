/**
 *******************************************************************************
 * \file    pwm.c
 *******************************************************************************
 * \brief    PWM driver for AT91SAM7Sxxx.
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
#include "pwm.h"
#include "pwm_config.h"

/**
 * \brief	Initialisierung für das PWM-Modul. 
 * 
 * \return 	NO_ERROR
 */
int pwm_init(){
        // enable PWM peripherals on PA23
        AT91F_PIO_CfgPeriph( AT91C_BASE_PIOA, PWM_CH0_PIN, 0 );

        // enable PWM clock in PMC
        AT91F_PWMC_CfgPMC();
        AT91F_PWMC_StopChannel( AT91C_BASE_PWMC, AT91C_PWMC_CHID0 );
       
        // setup the channel mode registers
        // CPRE = 0x0 : MCK_x
        // CALG = 1   : The period is center aligned
        // CPOL = 1   : The output waveform starts at a high level
        // CPD = 0    : Writing to the PWM_CUPDx will modify the duty cycle at the next period start event
        AT91C_BASE_PWMC_CH0->PWMC_CMR = MCK_1 | /*AT91C_PWMC_CPD |*/ AT91C_PWMC_CPOL | AT91C_PWMC_CALG;
           
        // setup the period registers
        AT91C_BASE_PWMC_CH0->PWMC_CPRDR = PEM_CH0_PERIOD;
           
        // setup the duty cycle registers (first value)
        AT91C_BASE_PWMC_CH0->PWMC_CDTYR = PEM_CH0_DUTY;
           
        // update the duty cycle
        //AT91C_BASE_PWMC_CH0->PWMC_CUPDR = 250;    
        return NO_ERROR;
}

/**
 * \brief	Starten des PWM-Signals
 * 
 * \return 	NO_ERROR
 */
int pwm_start(){
	// disable PWM channels 0
	AT91F_PWMC_StartChannel( AT91C_BASE_PWMC, AT91C_PWMC_CHID0 );
	return NO_ERROR;
}

/**
 * \brief	Stoppen des PWM-Signals 
 * 
 * \return 	NO_ERROR
 */
int pwm_stop(){
	// enable PWM channels 0
	AT91F_PWMC_StopChannel( AT91C_BASE_PWMC, AT91C_PWMC_CHID0 );
	return NO_ERROR;
}

/**
 * \brief	Verändern des Dutycycles
 * 
 * \param[in]	duty	neuer Dutycycle in %
 * 
 * \return 	NO_ERROR
 */
int pwm_update(short duty){
	// updates the Duty Cycle
	AT91C_BASE_PWMC_CH0->PWMC_CMR &= ~AT91C_PWMC_CPD;
	AT91C_BASE_PWMC_CH0->PWMC_CDTYR = (PEM_CH0_PERIOD*duty)/100;
	AT91C_BASE_PWMC_CH0->PWMC_CMR |= AT91C_PWMC_CPD;	
	return NO_ERROR;
}
