/**
 *******************************************************************************
 * \file    irq.c
 *******************************************************************************
 * \brief  Default Exception handlers
 *
 * \version	1.0
 * \date	27.11.06
 * \author	M.Muehlemann
 *
 *
 *******************************************************************************
 */

#include "AT91SAM7S256.h"
#include "printf.h"
#include "irq.h"

/**
 ******************************************************************************
 * function    	: AT91F_Default_FIQ_handler()
 ******************************************************************************
 * \brief
 * Default Interrupt Exception handler
 ******************************************************************************
 * \param     no parameters
 * \return    no return
 ******************************************************************************
 *  \author     M.Muehlemann
 *
 *  \date       27.11.06
 ******************************************************************************
 */
void AT91F_Default_IRQ_handler( void ){
	while(1);
}

/**
 ******************************************************************************
 * function    	: AT91F_Default_FIQ_handler()
 ******************************************************************************
 * \brief
 * Default Fast-Interrupt Exception handler
 ******************************************************************************
 * \param     no parameters
 * \return    no return
 ******************************************************************************
 *  \author     M.Muehlemann
 *
 *  \date       27.11.06
 ******************************************************************************
 */
void AT91F_Default_FIQ_handler( void ){
	while(1);
}

/**
 ******************************************************************************
 * function    	: AT91F_Default_SWI_handler()
 ******************************************************************************
 * \brief
 * Default Software Interrupt Exception handler
 ******************************************************************************
 * \param     no parameters
 * \return    no return
 ******************************************************************************
 *  \author     M.Muehlemann
 *
 *  \date       27.11.06
 ******************************************************************************
 */
void AT91F_Default_SWI_handler( void ){
	while(1);
}

/**
 ******************************************************************************
 * function    	: AT91F_Default_UNDEF_handler()
 ******************************************************************************
 * \brief
 * Default Undefined Instruction Exception handler
 ******************************************************************************
 * \param     no parameters
 * \return    no return
 ******************************************************************************
 *  \author     M.Muehlemann
 *
 *  \date       27.11.06
 ******************************************************************************
 */
void AT91F_Default_UNDEF_handler( void ){
	while(1);
}

/**
 ******************************************************************************
 * function    	: AT91F_Default_Spurious_handler()
 ******************************************************************************
 * \brief
 * The Advanced Interrupt Controller features protection against spurious 
 * interrupts. A spurious interrupt is defined as being the assertion of an
 * interrupt source long enough for the AIC to assert the nIRQ, but no
 * longer present when AIC_IVR is read.
 * (for more detauils refer Chapter 27.7.6 in the AT91SAM7Sxxx datasheet)
 ******************************************************************************
 * \param     no parameters
 * \return    no return
 ******************************************************************************
 *  \author     M.Muehlemann
 *
 *  \date       27.11.06
 ******************************************************************************
 */
void AT91F_Default_Spurious_handler( void ){
	while(1);
}

/**
 ******************************************************************************
 * function    	: AT91F_Default_FIQ_handler()
 ******************************************************************************
 * \brief
 * Default Software Interrupt Exception handler
 ******************************************************************************
 * \param     no parameters
 * \return    no return
 ******************************************************************************
 *  \author     M.Muehlemann
 *
 *  \date       27.11.06
 ******************************************************************************
 */
void AT91F_Default_Abort_handler( void ){
	while(1);
}
