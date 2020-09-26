/**
 *******************************************************************************
 * \file    pwm_config.h
 *******************************************************************************
 * \brief   Application and Hardware specific configurations for PWM driver.
 *
 *
 * \version		1.0
 * \date		07.03.2008
 * \author		R.Zoss
 *
 *******************************************************************************
 */
#ifndef PWM_CONFIG_H_
#define PWM_CONFIG_H_

// IOs Configuration
/**
 * \brief	Pin für PWM-Ausgang ist PA0
 */
#define PWM_CH0_PIN		AT91C_PA0_PWM0

// CPRE: Channel Pre-scaler
/**
 * \brief Masterclock divided by 0
 */
#define MCK_1		0 
/**
 * \brief Masterclock divided by 1
 */
#define MCK_2 		1 
/**
 * \brief Masterclock divided by 2
 */
#define MCK_4		2 
/**
 * \brief Masterclock divided by 4
 */
#define MCK_8		3
/**
 * \brief Masterclock divided by 8
 */
#define MCK_16		4 
/**
 * \brief Masterclock divided by 16
 */
#define MCK_32		5 
/**
 * \brief Masterclock divided by 32
 */
#define MCK_64		6 
/**
 * \brief Masterclock divided by 64
 */
#define MCK_128		7 
/**
 * \brief Masterclock divided by 128
 */
#define MCK_256		8 
/**
 * \brief Masterclock divided by 256
 */
#define MCK_512		9
/**
 * \brief Masterclock divided by 1024
 */
#define MCK_1024	10
/**
 * \brief Externer Clock
 */
#define MCK_A		11
/**
 * \brief Externer Clock
 */
#define MCK_B		12


// TODO: Dynamisch berechnen  

/**
 * \brief Periode für Channel 0\n
 * 		  Formula: CPRD = T_s * MCK / (MCK_x * 2) = MCK / (MCK_x * f_s * 2)\n
 * 		  MCK = 48 MHz, T_s = 1/f_s
 */
#define PEM_CH0_PERIOD 		MCK / (2048 * 2)

/**
 * \brief Duty Cycle für Channel 0\n 
 * 		  Formula: CDTY = CPRD * d
 */
#define PEM_CH0_DUTY		(PEM_CH0_PERIOD*50)/100




#endif /*PWM_CONFIG_H_*/
