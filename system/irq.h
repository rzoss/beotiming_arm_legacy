/**
 *******************************************************************************
 * \file    irq.h
 *******************************************************************************
 * \brief  Definitions for interrupt handling:
 * 			- Macros
 * 			- Function prototypes for Default exception handlers
 *
 * \version	1.0
 * \date	27.11.06
 * \author	M.Muehlemann
 *
 *
 *******************************************************************************
 */

#ifndef IRQ_H_
#define IRQ_H_


/**
 ******************************************************************************
 * macro    	: ENABLE_INTERRUPTS
 ******************************************************************************
 * \brief
 * Macro to enable intterrupts. Clears the I-Bit in the CPSR
 ******************************************************************************
 */
#define ENABLE_INTERRUPTS asm( 	"mrs  r0, cpsr\n\tbic  r0, r0,#0x00000080 \n\tmsr  cpsr, r0");	

/**
 ******************************************************************************
 * macro    	: DISABLE_INTERRUPTS
 ******************************************************************************
 * \brief
 * Macro to disable intterrupts. Sets the I-Bit in the CPSR
 ******************************************************************************
 */
#define DISABLE_INTERRUPTS asm( "mrs  r0, cpsr\n\torr  r0, r0,#0x00000080 \n\tmsr  cpsr, r0");	


void AT91F_Default_IRQ_handler( void ) __attribute__((interrupt("IRQ")));
void AT91F_Default_SWI_handler( void ) __attribute__((interrupt("SWI")));
void AT91F_Default_FIQ_handler(void)  __attribute__((interrupt("FIQ")));
void AT91F_Default_Abort_handler(void )__attribute__((interrupt("ABORT")));
void AT91F_Default_Spurious_handler(void )__attribute__((interrupt("IRQ")));

#endif /*IRQ_H_*/
