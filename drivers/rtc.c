/**
 *******************************************************************************
 * \file    rtc.c
 *******************************************************************************
 * \brief    RTC driver and state machine for Maxim DS1338Z
 * 
 * \version		2.0
 * \date		24.04.2008
 * \author		R.Zoss, M.Leiser
 *
 *******************************************************************************
 */

#include "i2c.h"
#include "global.h"
#include "rtc.h"
#include "uart.h"
#include "timer.h"
#include "rfid.h"
#include "sd-card.h"
#include "display.h"
#include "ADWandler.h"
#include "gsmgps.h"
#include "../system/printf.h"

void rtc_calc_timestring(TIME *datum);

//globale Variablen


/**sleep
 * \brief	indiziert den Schlafzustand

 */
int sleep;


/**start_sync
 * \brief	Startanlage soll synchronisieren	

 */
int start_sync;

/**time_valid
 * \brief	zeigt ob die Zeit im RTC über GPS synchronisiert wurde und damit gültig ist.

 */
int time_valid;

/**
 * \brief	RTC State Machine
 * 
 */
void rtc(){
	char dummy[100];
	TIME date;
	
	int error1=0; //error1 ist für adwandler read
	
	//TRACE("AUFRUF: RTC STATE MACHINE\r\n");
	//FTRACE("AUFRUF: RTC STATE MACHINE\r\n");
	//fill in the actual time
	rtc_get_time(&actTime);

	
	switch(rtc_state){
		
		case START_RTC: 
			TRACE("RTC START State\n\r");
			FTRACE("RTC START State\r\n");
			//warten bis Zeit über GPS synchronisiert wurde
			//neu 22.07.08 AD Wandler
				//error1=ADW_read();
				error1=ADW_read_inc();
				TRACE("%d \n\r",error1);
				sprintf(dummy,"I Last: %d\n\rU Last: %d\n\rI Solar: %d\n\rU Solar: %d\n\r",  ADWerte.ILast,ADWerte.ULast,ADWerte.ISolar,ADWerte.USolar);
				TRACE("%s\n\r",dummy);
				
			//end neu 22.07.08
			if(time_valid){
				rtc_state=WAKE;
				TRACE("RTC goes to WAKE State\r\n");
				FTRACE("RTC goes to WAKE State\r\n");
			}
			break;
		case WAKE: 
			//neu 22.07.08 AD Wandler
			//error1=ADW_read();
			error1=ADW_read_inc();
			TRACE("%d \n\r",error1);
			sprintf(dummy,"I Last: %d\n\rU Last: %d\n\rI Solar: %d\n\rU Solar: %d\n\r",  ADWerte.ILast,ADWerte.ULast,ADWerte.ISolar,ADWerte.USolar);
			TRACE("%s\n\r",dummy);
			
			//end neu 22.07.08
			
			
			//i2c tester
			rtc_get_time(&date);
			if(date.minuten==0 && date.stunden==INITSTRUCT.WAKEUP_TIME && !INITSTRUCT.STARTorFINISH){
				start_sync=1;
			}
			
			
		
					
			sprintf(dummy,"%02d.%02d - %02d:%02d:%02d",date.tage,date.monate, date.stunden,date.minuten,date.sekunden);
			//Disp_WriteLine(dummy,3);
			TRACE("%s\n\r",dummy);
			
			
			if(actTime.stunden==INITSTRUCT.GOODNIGHT_TIME && !sleep && actTime.minuten==0){
				//Alles in Schlaf versetzen und andere Geräte dazu veranlassen wenn GOODNIGHTTIME erreicht
				
				sleep=1;			//andere State machines wissen lassen, dass sleeptime ist
				rtc_state=SLEEP;	//in den Schlafzustand wechseln
				TRACE("RTC goes to SLEEP State\r\n");
				FTRACE("RTC goes to SLEEP State\r\n");
			}
			break;
		case SLEEP:
			rtc_get_time(&date);
			if(date.minuten==0 && date.stunden==INITSTRUCT.WAKEUP_TIME && !INITSTRUCT.STARTorFINISH){
				start_sync=1;
			}
			if(!((AT91C_BASE_PIOA->PIO_PDSR) & PIN_RFID_OUT)){
				//Alles aufwecken und andere Geräte dazu veranlassen wenn WAKEUPTIME erreicht
				
				sleep=0;			//ander State machines wissen, dass waketime ist
				rtc_state=WAKE;		//in den Wake Zustand wechseln
				TRACE("RTC goes to WAKE State\r\n");
				FTRACE("RTC goes to WAKE State\r\n");
			}
			break;
	}		//end switch case
			
	
	
}


/**
 * \brief	Initialisiert den RealTimeClock Baustein DS1338Z
 * 
 * \param[out]	actTime		Pointer auf die Zeitstruktur für die aktuelle Zeit
 * 
 * return NO_ERROR on sucess or a error code (see i2c.h) 
 */
int rtc_init(TIME* actTime){
	int i2c_Err;
	
	rtc_state = START_RTC;
	sleep=0;
	time_valid=0;
	
	
	i2c_Err = rtc_get_time(actTime);

	return i2c_Err;
}

/**
 * \brief	Liefert den Registerinhalt von regAdr (1 Byte in data) des RealTimeClock Baustein DS1338Z
 * 
 * \param[in]	regAdr		Adresse des Registers im RTC
 * \param[out]	data		gelesene Daten
 * \param[in]	length		Länge der erwarteten Daten
 * 
 * return NO_ERROR on sucess or a error code (see i2c.h) 
 */
int rtc_read_reg(char * regAdr ,char *data, char length)
{	int i2c_Err;
	int readBytes;
	
	i2c_Err = i2c_write(RTC_ADR,0,0,regAdr,1);
	i2c_Err |= i2c_read(RTC_ADR,0,0,data,length,&readBytes);
	
	return i2c_Err;
}

/**
 * \brief	Sendet Daten an Adresse devAdr, in Register regAdr (1 Byte in data) des RTC
 * 
 * \param[in]	data		zu schreibende Daten
 * \param[in]	length		Länge der zu schreibenden Daten
 * 
 * return NO_ERROR on sucess or a error code (see i2c.h) 
 */
int rtc_write_reg(char *data, char length)
{	int i2c_Err;
	
	i2c_Err = i2c_write(RTC_ADR,0,0,data,length);
	return i2c_Err;
}

/**
 * \brief	beschreibt den String in der Struktur TIME mit der aktuellen Zeit
 * 
 * \param[in]	datum		Zeitstruktur
 * 
 * return no retrun 
 */
void rtc_calc_timestring(TIME *datum){
	sprintf(datum->string,"%02d.%02d.20%02d %02d:%02d:%02d",datum->tage, datum->monate, datum->jahre,datum->stunden,datum->minuten,datum->sekunden);
}

/**
 * \brief	Liest die aktuelle Zeit vom RTC
 * 
 * \param[in]	date		Pointer auf die Zeitstruktur
 * 
 * return NO_ERROR on sucess or a error code (see i2c.h) 
 */
int rtc_get_time(TIME* date)
{	int i2c_Err = 0;
	int readBytes;
	char data[8];
	
	data[0] = RTC_REG_SEC;
	
	i2c_Err |= i2c_write(RTC_ADR,0,0,data,1);
	i2c_Err |= i2c_read(RTC_ADR,0,0,data,7,&readBytes);
	date->sekunden = BCD_TO_DEC(data[0]);
	date->minuten = BCD_TO_DEC(data[1]);
	date->stunden = BCD_TO_DEC(data[2]);
	date->wochentag = BCD_TO_DEC(data[3]);
	date->tage = BCD_TO_DEC(data[4]);
	date->monate = BCD_TO_DEC(data[5]);
	date->jahre = BCD_TO_DEC(data[6]);	
	
	rtc_calc_timestring(date);
	return i2c_Err;
}

/**
 * \brief	Setzt Uhrzeit und Datum im RTC
 * 
 * \param[in]	date		Pointer auf die Zeitstruktur
 * 
 * return NO_ERROR on sucess or a error code (see i2c.h) 
 */
int rtc_set_time(TIME* date){
	int i2c_Err = 0;
	char data[8];
	
	data[0] = RTC_REG_SEC;
	data[1] = DEC_TO_BCD(date->sekunden);
	data[2] = DEC_TO_BCD(date->minuten);
	data[3]	= DEC_TO_BCD(date->stunden);
	data[4] = DEC_TO_BCD(date->wochentag);
	data[5] = DEC_TO_BCD(date->tage);
	data[6] = DEC_TO_BCD(date->monate);
	data[7] = DEC_TO_BCD(date->jahre);
	
	date->milisekunden = 0;
	i2c_Err |= i2c_write(RTC_ADR,0,0,data,8);
	return i2c_Err;
}

/**
 * \brief	Setzt Uhrzeit und Datum im RTC nach dem NMEA Struct vom GPS
 * 
 * 
 * 
 * return NO_ERROR on sucess or a error code (see i2c.h) 
 */
int rtc_set_time_from_NMEA(){
	int i2c_Err = 0;
	char data[8];
	
	data[0] = RTC_REG_SEC;
	if(NMEA_Protocol.milisecond>500)
		data[1] = DEC_TO_BCD(NMEA_Protocol.second+1);
	else
		data[1] = DEC_TO_BCD(NMEA_Protocol.second);
	data[2] = DEC_TO_BCD(NMEA_Protocol.minute);
	data[3]	= DEC_TO_BCD(NMEA_Protocol.hour);
	data[4] = 0;								//DEC_TO_BCD(date->wochentag);
	data[5] = DEC_TO_BCD(NMEA_Protocol.day);
	data[6] = DEC_TO_BCD(NMEA_Protocol.month);
	data[7] = DEC_TO_BCD(NMEA_Protocol.year);
	
	i2c_Err |= i2c_write(RTC_ADR,0,0,data,8);
	return i2c_Err;
}

