/**
 *******************************************************************************
 * \file    ADWandler.c
 *******************************************************************************
 * \brief    Driver für AD Wandler auf Connector Board
 * 
 * \version		1.0
 * \date		17.08.2008
 * \author		M. Leiser
 *
 *******************************************************************************
 */
#include <AT91SAM7S256.h>
#include <lib_AT91SAM7S256.h>
#include <irq.h>

#include "ADWandler.h"
#include "i2c.h"
#include "uart.h"
#include <printf.h>
#include "global.h"
#include "sd-card.h"
#include "gsmgps.h"
#include "timer.h"



//NEU 30-07-2008

/**
 * \brief	liest alle Eingänge in die gewählte Variable 
 * 
 * \return 	NO_ERROR on sucess or a error code (see i2c.h)
 */
int ADW_read_inc(){
	char ADWcontrol;
	char ADWanswer[5];
	int bytesread;
	//int value;
	int i2c_Err=0;
	//werte auf i2c lesen Kanal 1 I Last
	ADWcontrol=ADW_CONTROL_INC;
	i2c_Err=i2c_write(ADWADR,0,0,&ADWcontrol,1);
	i2c_Err=i2c_read(ADWADR,0,0,ADWanswer,5,&bytesread);

	//verarbeitung
	
	//umrechnen!!!! Alle Werte zwischen 0 und 5 Volt
	//ILast 0A = 2.5V = 128 (133mV/A) wenig....
	//USolar
	//ISolar
	//ULast
	
	ADWerte.ILast=ADWanswer[1];
	ADWerte.USolar=ADWanswer[2];
	ADWerte.ISolar=ADWanswer[3];
	ADWerte.ULast=ADWanswer[4];
	
	return i2c_Err;
}


//END NEU 30-07-2008

/**
 * \brief	liest alle Eingänge in die gewählte Variable
 * 
 * \return 	NO_ERROR on sucess or a error code (see i2c.h)
 */
int ADW_read(){
	char ADWcontrol;
	char ADWanswer;
	int bytesread;
	//int value;
	int i2c_Err=0;
	//werte auf i2c lesen Kanal 1 I Last
	ADWcontrol=ADW_CONTROL1;
	i2c_Err=i2c_write(ADWADR,0,0,&ADWcontrol,1);
	i2c_Err=i2c_read(ADWADR,0,0,&ADWanswer,1,&bytesread);
	i2c_Err=i2c_read(ADWADR,0,0,&ADWanswer,1,&bytesread);

	//verarbeitung
	//value=selfAtoi(&ADWanswer);
	//umrechnen
	
	ADWerte.ILast=ADWanswer;
	//werte auf i2c lesen Kanal 2 U Solar
	ADWcontrol=ADW_CONTROL2;
	i2c_Err=i2c_write(ADWADR,0,0,&ADWcontrol,1);
	i2c_Err=i2c_read(ADWADR,0,0,&ADWanswer,1,&bytesread);
	i2c_Err=i2c_read(ADWADR,0,0,&ADWanswer,1,&bytesread);

	//verarbeitung
	//value=selfAtoi(&ADWanswer);
	//umrechnen
	
	ADWerte.USolar=ADWanswer;
	//werte auf i2c lesen Kanal 3 I Solar
	ADWcontrol=ADW_CONTROL3;
	i2c_Err=i2c_write(ADWADR,0,0,&ADWcontrol,1);
	i2c_Err=i2c_read(ADWADR,0,0,&ADWanswer,1,&bytesread);
	i2c_Err=i2c_read(ADWADR,0,0,&ADWanswer,1,&bytesread);

	//verarbeitung
	//value=selfAtoi(&ADWanswer);
	//umrechnen
	
	ADWerte.ISolar=ADWanswer;
	//werte auf i2c lesen Kanal 4 U Last
	ADWcontrol=ADW_CONTROL4;
	i2c_Err=i2c_write(ADWADR,0,0,&ADWcontrol,1);
	i2c_Err=i2c_read(ADWADR,0,0,&ADWanswer,1,&bytesread);
	i2c_Err=i2c_read(ADWADR,0,0,&ADWanswer,1,&bytesread);

	//verarbeitung
	//value=selfAtoi(&ADWanswer);
	//umrechnen
	
	ADWerte.ULast=ADWanswer;
	
	return i2c_Err;
}






