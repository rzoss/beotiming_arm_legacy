/**
 *******************************************************************************
 * \file    pwm.h
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
#ifndef PWM_H_
#define PWM_H_


int pwm_init();
int pwm_start();
int pwm_stop();
int pwm_update(short duty);

#endif /*PWM_H_*/
