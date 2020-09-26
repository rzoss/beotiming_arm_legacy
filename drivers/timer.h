/**
 *******************************************************************************
 * \file    timer.h
 *******************************************************************************
 * \brief    TC Konfiguration und delay ms
 * 
 * \version		1.2
 * \date		13.04.2008
 * \author		M. Mühlemann, R. Zoss
 *
 *******************************************************************************
 */

#ifndef TIMER_H_
#define TIMER_H_




// Error
/** \brief	Wert des Parameters ist auserhalb des gültigen Bereichs (Grösser INT_MAX)*/
#define ERROR_TIMER_RANGE					1

extern volatile int flag50;
extern volatile int atTimeout;
extern volatile int timer_rfid;
extern volatile int timer2000;
extern volatile int timeoutCtr;
extern volatile int timer_display;
extern volatile int hw_on_timer;

int TC0_init();
void handler_timerIRQ( void );
int delay(int ms);

#endif /*TIMER_H_*/
