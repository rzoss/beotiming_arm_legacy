/**
 *******************************************************************************
 * \file    rfid.c
 *******************************************************************************
 * \brief    RFID driver and state machine for I2C-RFID-Reader/Writer (RFM-030).
 * 
 * \version		1.0
 * \date		20.03.2008
 * \author		R.Zoss, M. Leiser
 *
 *******************************************************************************
 */
#include <AT91SAM7S256.h>
#include <lib_AT91SAM7S256.h>
#include <string.h>

#include "global.h"
#include "rfid.h"
#include "gsmgps.h"
#include "timer.h"
#include "i2c.h"
#include "pwm.h"
#include "uart.h"
#include "display.h"
#include "rtc.h"
#include "string.h"
#include "IOExp.h"
#include <printf.h>
#include "sd-card.h"

/**
 * \brief	Global Vars
 */
/**
 * \brief	Zeigt an, ob Daten zum senden auf der SD-Karte liegen
 */
int dataTOsend;
/**
 * \brief	Timeout für Versuch die Karte auszuwählen
 */
int timeout;
/**
 * \brief	rfid ERROR Variable
 */
int rfid_Err;
/**
 * \brief	Zeigt an, ob eine Karte im Feld ist
 */
unsigned int tag_detected = NO_TAG_PRESENT;

/**
 * \brief	Temporärer Datenbuffer
 */
char TempBuf[26];

/**
 * \brief	Informationen über das aktuelle Tag, falls sich eines im Feld befindet
 */
TAG tag = {{0},{0},0,0};



/**
 * rfid()
 * 
 * \brief	 RFID State Machine
 * 			
 */
void rfid(){
	//char data[16];
	TIME date;
	RFIDDATE startdate_rfid;
	RFIDDATE enddate_rfid;
	RFIDTIME racetime;
	short Streckenkey;
	long raceseconds;
	char dummy[100];
	static int read;
	//TRACE("AUFRUF: RFID STATE MACHINE\n\r");
	FTRACE("AUFRUF: RFID STATE MACHINE\r\n");	
		switch(rfid_state){
				
				case RFID_IDLE: 
					
					if(!AT91F_PIO_IsInputSet(AT91C_BASE_PIOA, PIN_RFID_OUT) && !tag_detected && startedup){
							IOE_setOneOutput(IOEXP_OUT_LED_ROT,1);
							tag_detected = TAG_PRESENT;
							rfid_state=RFID_READ_CARD;
							TRACE("RFID READ_CARD\r\n");
							FTRACE("RFID READ_CARD\r\n");
							
						}else if(sleep){
							TRACE("RFID goes to sleep\n\r");
							//setzen des IN-Signals (Rising edge power down)
							AT91F_PIO_SetOutput(AT91C_BASE_PIOA, PIN_RFID_IN);
							RFID_suspend();
							rfid_state=RFID_SLEEP;

						}else if(AT91F_PIO_IsInputSet(AT91C_BASE_PIOA, PIN_RFID_OUT)&& startedup && timer_rfid==0){
							tag_detected = NO_TAG_PRESENT;
							IOE_setOneOutput(IOEXP_OUT_LED_ROT,0);
							IOE_setOneOutput(IOEXP_OUT_LED_GRUEN,0);
							Disp_EraseLine("                ",1);
							Disp_EraseLine("                ",2);
							Disp_EraseLine("                ",3);
							timer_rfid=1000;
					}
					break;
				case RFID_READ_CARD:
					rfid_Err=RFID_select();
					if(rfid_Err==ERROR_RFID_NOTAG){
						timer_rfid=1000;
						rfid_state=RFID_ERROR;
						TRACE("Keine Karte im Feld! \n\r");
						tag_detected=NO_TAG_PRESENT;
						IOE_setOneOutput(IOEXP_OUT_LED_ROT,0);
					}else if(rfid_Err==ERROR_RFID_COLLISION){
						timer_rfid=1000;
						rfid_state=RFID_ERROR;
						tag_detected=NO_TAG_PRESENT;
						IOE_setOneOutput(IOEXP_OUT_LED_ROT,0);
						TRACE("Mehrere Karten im Feld! \n\r");
					}else if(tag.type==TYPE_ULTRA_LIGHT){
						rfid_Err=RFID_getStatus_UL();
						if (!rfid_Err) {
				if (INITSTRUCT.STARTorFINISH) {//finish statemachine aufrufen
					if ((tag.tag_status & TAG_STATUS_STRECKENVALID)
							&& (tag.tag_status & TAG_STATUS_STARTVALID)
							&& !(tag.tag_status & TAG_STATUS_ENDVALID)
							&& !(tag.tag_status & TAG_STATUS_MANUALCLEARED)) {
						//endzeit und fahrzeit auf karte schreiben, status setzen auf 1110X, zeit in file schreiben und Status setzen damit es per gprs geschickt wird
						rfid_state=RFID_WRITE_ENDTIME;
						//	timer_rfid=1000;
					} else if ((tag.tag_status & TAG_STATUS_STRECKENVALID)
							&& (tag.tag_status & TAG_STATUS_STARTVALID)
							&& (tag.tag_status & TAG_STATUS_ENDVALID)
							&& !(tag.tag_status & TAG_STATUS_MANUALCLEARED)) {
						//fahrzeit anzeigen möglichkeit zum löschen mit ok/delete Taste (Taste 2)
						rfid_state=RFID_SHOW_RACETIME;
						read=0;
						switches=0;
						timer_rfid=500;
					} else {
						rfid_state=RFID_ERROR;
					}
				} else {//start Statemachine aufrufen
					if ((tag.tag_status & TAG_STATUS_STRECKENVALID)
							&& !(tag.tag_status & TAG_STATUS_STARTVALID)
							&& !(tag.tag_status & TAG_STATUS_ENDVALID)
							&& !(tag.tag_status & TAG_STATUS_MANUALCLEARED)) {
						rfid_state=RFID_WRITE_STARTTIME;
						Disp_WriteLine("Karte nicht     ", 1);
						Disp_WriteLine("entfernen       ", 2);
						IOE_setOneOutput(IOEXP_OUT_LED_ROT,1);
						timer_rfid=2000;

					} else { //strecke auswählen lassen
						chooseRace=IN;
						rfid_state=RFID_CHOOSE_RACE;
					}
				}	
						}else{
							rfid_state=RFID_ERROR;
							tag_detected=NO_TAG_PRESENT;
						}
					
					
					}
					break;
				case RFID_CHOOSE_RACE:
					choose_Race();	//race choose dialog öffnen
					
					
					break;
				case RFID_WRITE_STARTTIME:
					rtc_get_time(&date);	//Datum/Zeit vom RTC lesen und in rfid Struktur abfüllen
					startdate_rfid.year=date.jahre+2000;
					startdate_rfid.month=date.monate;
					startdate_rfid.day=date.tage;
					startdate_rfid.hour=date.stunden;
					startdate_rfid.minute=date.minuten;
					startdate_rfid.second=date.sekunden;
											
					rfid_Err=RFID_Operation_UL(&startdate_rfid,WRITE_STARTTIME);	//schreibe Startzeit auf Karte
					if(!rfid_Err){
						tag.tag_status |= TAG_STATUS_STARTVALID;
						RFID_setStatus_UL(tag.tag_status);	//set Status Starttime valid
					
						pwm_start();
						TRACE("timerrfid %d",timer_rfid);
						if(timer_rfid!=0){
							Disp_WriteLine("Zeit gespeichert",1);
							Disp_WriteLine("    STARTEN     ",2);
							Disp_EraseLine("                ",3);
							IOE_setOneOutput(IOEXP_OUT_LED_GRUEN,1);
							IOE_setOneOutput(IOEXP_OUT_LED_ROT,0);
						
						}else{					
							rfid_state=RFID_IDLE; //go to IDLE State	
							IOE_setOneOutput(IOEXP_OUT_LED_GRUEN,0);
							pwm_stop();
						}
					}
					break;
				case RFID_WRITE_ENDTIME:
					//endzeit und fahrzeit auf Karte schreiben, status setzen auf 1110X, zeit in file schreiben und Status setzen damit es per gprs geschickt wird								
					rtc_get_time(&date);	//Datum/Zeit vom RTC lesen und in rfid Struktur abfüllen
					enddate_rfid.year=date.jahre+2000;
					enddate_rfid.month=date.monate;
					enddate_rfid.day=date.tage;
					enddate_rfid.hour=date.stunden;
					enddate_rfid.minute=date.minuten;
					enddate_rfid.second=date.sekunden;
																
					rfid_Err=RFID_Operation_UL(&enddate_rfid,WRITE_ENDTIME);	//schreibe Endzeit auf Karte										
					rfid_Err=RFID_Operation_UL(&startdate_rfid,READ_STARTTIME);	//lese Startzeit von Karte
					//rechne Racetime aus (Racetime kann auch grösser sein als ein Tag...)
					if(startdate_rfid.day == enddate_rfid.day){
						raceseconds = (enddate_rfid.hour-startdate_rfid.hour)*3600;
						raceseconds += (enddate_rfid.minute-startdate_rfid.minute)*60;
						raceseconds += (enddate_rfid.second-startdate_rfid.second);
					}else{
						raceseconds = (enddate_rfid.hour-startdate_rfid.hour)*3600+(3600*24);
						raceseconds += (enddate_rfid.minute-startdate_rfid.minute)*60;
						raceseconds += (enddate_rfid.second-startdate_rfid.second);
					}
					if(raceseconds >= 3600*24){
						racetime.day = 1;
						raceseconds-=3600*24;
					}else{
						racetime.day = 0;
					}
					racetime.hour = raceseconds / 3600;
					raceseconds%=3600;
					racetime.minute = raceseconds /60;
					raceseconds%=60;
					racetime.second = raceseconds;
					//racetime ausgerechnet
					rfid_Err=RFID_Operation_UL(&racetime,WRITE_RACETIME);	//schreibe Fahrzeit auf Karte
					Disp_WriteLine("Zeit gespeichert",1);
					sprintf(dummy,"    %02d:%02d:%02d    ",racetime.hour,racetime.minute,racetime.second);
					Disp_WriteLine(dummy,2);
					Disp_EraseLine("                ",3);
					tag.tag_status |= TAG_STATUS_ENDVALID;
					RFID_setStatus_UL(tag.tag_status);	//set Status Endtime valid 1110X
					//write to logfile on sd Card
					rfid_Err=RFID_Operation_UL(&Streckenkey,READ_STRECKENKEY);	//lese Streckenkey von Karte
					rfid_Err=RFID_Operation_UL(&startdate_rfid,READ_STARTTIME);	//lese Startzeit von Karte
					rfid_Err=RFID_Operation_UL(&enddate_rfid,READ_ENDTIME);	//lese Endzeit von Karte
					sprintf(dummy,"TIMW;%d;%d-%d-%d %02d:%02d:%02d;%d-%d-%d %02d:%02d:%02d;%02d:%02d:%02d;%s;",Streckenkey,
							startdate_rfid.year,startdate_rfid.month,startdate_rfid.day,startdate_rfid.hour,startdate_rfid.minute, startdate_rfid.second,
							enddate_rfid.year,enddate_rfid.month,enddate_rfid.day,enddate_rfid.hour,enddate_rfid.minute, enddate_rfid.second,
							racetime.hour,racetime.minute,racetime.second,tag.serial_number_ascii);
					sd_logfile(dummy); 
					//write to timefile
					sd_timefile(dummy);
					dataTOsend=1;
				//	if(timer_rfid && !AT91F_PIO_IsInputSet(AT91C_BASE_PIOA, PIN_RFID_OUT)){
				//	IOE_setOneOutput(IOEXP_OUT_LED_ROT,0);		//
				//	IOE_setOneOutput(IOEXP_OUT_LED_GRUEN,1);	//
					pwm_start();	
				//	}else{
				//		pwm_stop();
					timer_rfid=1000;
					rfid_state=RFID_BEEP; //go to IDLE State
				//	}
					
					
					break;	
				case RFID_SHOW_RACETIME:
					if(read==0){
						rfid_Err=RFID_Operation_UL(&racetime,READ_RACETIME);
						sprintf(dummy,"    %02d:%02d:%02d    ",racetime.hour,racetime.minute,racetime.second);
						Disp_WriteLine(dummy,1);
						Disp_WriteLine("Zeit loeschen   ",2);
						Disp_WriteLine("mit OK Taste    ",3);	
						read=1;
						IOE_setOneOutput(IOEXP_OUT_LED_ROT,0);
						IOE_setOneOutput(IOEXP_OUT_LED_GRUEN,1);
					}
					if(switches==2){
						IOE_setOneOutput(IOEXP_OUT_LED_GRUEN,0);
						IOE_setOneOutput(IOEXP_OUT_LED_ROT,1);
						rfid_state=RFID_DELETE_RACETIME; //go to DELETE State
						switches=0;
//						//write to logfile on sd Card
//						rfid_Err=RFID_Operation_UL(&Streckenkey,READ_STRECKENKEY);	//lese Streckenkey von Karte
//						rfid_Err=RFID_Operation_UL(&startdate_rfid,READ_STARTTIME);	//lese Startzeit von Karte
//						rfid_Err=RFID_Operation_UL(&enddate_rfid,READ_ENDTIME);	//lese Endzeit von Karte
//						sprintf(dummy,"TIMD;%d;%d-%d-%d %02d:%02d:%02d;%d-%d-%d %02d:%02d:%02d;%02d:%02d:%02d;%s;",Streckenkey,
//								startdate_rfid.year,startdate_rfid.month,startdate_rfid.day,startdate_rfid.hour,startdate_rfid.minute, startdate_rfid.second,
//								enddate_rfid.year,enddate_rfid.month,enddate_rfid.day,enddate_rfid.hour,enddate_rfid.minute, enddate_rfid.second,
//								racetime.hour,racetime.minute,racetime.second,tag.serial_number_ascii);
//						sd_logfile(dummy); 
//						//write to timefile erase befehl
//						sd_timefile(dummy);
//						dataTOsend=1;
						Disp_EraseLine("Bestaetigen mit ",1);	
						Disp_WriteLine("OK, oder Karte  ",2);
						Disp_EraseLine("entfernen       ",3);	
						
						IOE_setOneOutput(IOEXP_OUT_LED_GRUEN,1);
						IOE_setOneOutput(IOEXP_OUT_LED_ROT,0);
						
//						tag.tag_status |= TAG_STATUS_MANUALCLEARED;
//						RFID_setStatus_UL(tag.tag_status);	//set Status Endtime valid 1111X
//						
//						rfid_state=RFID_IDLE; //go to IDLE State
//						
					}else if(timer_rfid==0 && (AT91C_BASE_PIOA->PIO_PDSR) & PIN_RFID_OUT){		//wenn Timer abgelaufen, oder Karte entfernt wird.

						rfid_state=RFID_IDLE; //go to IDLE State
					}
					break;
				case RFID_DELETE_RACETIME:
					if(!((AT91C_BASE_PIOA->PIO_PDSR) & PIN_RFID_OUT)){
						
						if(switches==2){
							IOE_setOneOutput(IOEXP_OUT_LED_ROT,1);
							IOE_setOneOutput(IOEXP_OUT_LED_GRUEN,0);
							switches=0;
							//write to logfile on sd Card
							rfid_Err=RFID_Operation_UL(&Streckenkey,READ_STRECKENKEY);	//lese Streckenkey von Karte
							rfid_Err=RFID_Operation_UL(&startdate_rfid,READ_STARTTIME);	//lese Startzeit von Karte
							rfid_Err=RFID_Operation_UL(&enddate_rfid,READ_ENDTIME);	//lese Endzeit von Karte
							sprintf(dummy,"TIMD;%d;%d-%d-%d %02d:%02d:%02d;%d-%d-%d %02d:%02d:%02d;%02d:%02d:%02d;%s;",Streckenkey,
									startdate_rfid.year,startdate_rfid.month,startdate_rfid.day,startdate_rfid.hour,startdate_rfid.minute, startdate_rfid.second,
									enddate_rfid.year,enddate_rfid.month,enddate_rfid.day,enddate_rfid.hour,enddate_rfid.minute, enddate_rfid.second,
									racetime.hour,racetime.minute,racetime.second,tag.serial_number_ascii);
							sd_logfile(dummy); 
							//write to timefile erase befehl
							sd_timefile(dummy);
							dataTOsend=1;
							Disp_EraseLine("Fahrzeit wurde  ",1);	
							Disp_WriteLine("erfolgreich     ",2);
							Disp_EraseLine("geloescht       ",3);	
							
							tag.tag_status |= TAG_STATUS_MANUALCLEARED;
							RFID_setStatus_UL(tag.tag_status);	//set Status Endtime valid 1111X
							IOE_setOneOutput(IOEXP_OUT_LED_ROT,0);
							IOE_setOneOutput(IOEXP_OUT_LED_GRUEN,1);
							rfid_state=RFID_IDLE; //go to IDLE State
						}
					}else{
						rfid_state=RFID_IDLE; //go to IDLE State
					}
					break;
				
				case RFID_ERROR:
					IOE_setOneOutput(IOEXP_OUT_LED_ROT,0);
					IOE_setOneOutput(IOEXP_OUT_LED_GRUEN,1);
					if(INITSTRUCT.STARTorFINISH){
						Disp_WriteLine("     Karte      ",1);	
						Disp_WriteLine(" ungueltig oder ",2);
						Disp_WriteLine("   geloescht    ",3);
						if(AT91F_PIO_IsInputSet(AT91C_BASE_PIOA, PIN_RFID_OUT)){
							rfid_state=RFID_IDLE;
						}
					}else{
						Disp_WriteLine("     Karte      ",1);	
						Disp_WriteLine("   ungueltig -> ",2);
						Disp_WriteLine(" neue beziehen  ",3);
						if(AT91F_PIO_IsInputSet(AT91C_BASE_PIOA, PIN_RFID_OUT)){
							rfid_state=RFID_IDLE;
						}
					}
					
					break;
				case RFID_BEEP:
					if(!timer_rfid){
						pwm_stop();
						IOE_setOneOutput(IOEXP_OUT_LED_GRUEN,0);
						rfid_state=RFID_IDLE;
					}
												
					break;
				case RFID_SLEEP:
					if(!sleep){
						TRACE("RFID wakes up from sleep\n\r");
						// löschen des IN-Signals (Falling edge wake up from power down mode)
						IOE_setOneOutput(IOEXP_OUT_LED_ROT,0);
						IOE_setOneOutput(IOEXP_OUT_LED_GRUEN,0);
						AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, PIN_RFID_IN);
						rfid_state=RFID_IDLE;
					}
												
					break;
			}//end switch case
			
		
	

}

/**
 * \brief	läst den Benutzer eine Rennkategorie wählen 
 * 
 */
void choose_Race(){
	char dummy[17];
	switch(chooseRace){
		case IN:	
			Disp_EraseLine("                ",1);
			Disp_EraseLine("                ",2);
			Disp_EraseLine("                ",3);
//			tag.tag_status &= ~TAG_STATUS_STRECKENVALID;		//flag 0-3 löschen wenn uncomment dann werden statusbits am anfang gelöscht
//			tag.tag_status &= ~TAG_STATUS_STARTVALID;
//			tag.tag_status &= ~TAG_STATUS_ENDVALID;
//			tag.tag_status &= ~TAG_STATUS_MANUALCLEARED;
//			RFID_setStatus_UL(tag.tag_status);	//set Status Starttime valid
			switches=0;
			chooseRace=KEY1;
			break;
		case KEY1:
			
			Disp_WriteLine("Kategorieauswahl",1);
			sprintf(dummy,"%s",INITSTRUCT.STRECKENSTRING_1);
			Disp_WriteLine(dummy,2);
			if(switches==1 && INITSTRUCT.STRECKENKEY_3==0 && INITSTRUCT.STRECKENKEY_2==0){
				Disp_EraseLine("                ",2); //Zeile 2 löschen
				chooseRace=KEY1;
				switches=0;
			}else if(switches==1 && INITSTRUCT.STRECKENKEY_3==0 && INITSTRUCT.STRECKENKEY_2!=0){
				Disp_EraseLine("                ",2); //Zeile 2 löschen
				chooseRace=KEY2;
				switches=0;
			}else if(switches==1 && INITSTRUCT.STRECKENKEY_3!=0){
				Disp_EraseLine("                ",2); //Zeile 2 löschen
				chooseRace=KEY3;
				switches=0;
			}else if(switches==3 && INITSTRUCT.STRECKENKEY_2!=0){
				Disp_WriteLine("                ",2); //Zeile 2 löschen
				chooseRace=KEY2;
				switches=0;
			}else if(switches==3 && INITSTRUCT.STRECKENKEY_2==0){
				Disp_EraseLine("                ",2); //Zeile 2 löschen
				chooseRace=KEY1;
				switches=0;
			}else if(switches==2){
				switches=0;
				Disp_WriteLine("Gewaehlt:       ",1);
				IOE_setOneOutput(IOEXP_OUT_LED_ROT,0);
				IOE_setOneOutput(IOEXP_OUT_LED_GRUEN,1);
				//sprintf(dummy,"%s",INITSTRUCT.STRECKENSTRING_1);
				//Disp_WriteLine(dummy,2);
				//Hier eintrag machen		
				rfid_Err=RFID_Operation_UL(&INITSTRUCT.STRECKENKEY_1,WRITE_STRECKENKEY);
				if(!rfid_Err){
					//status setzten und zu idle zurückkehren
					tag.tag_status &= ~TAG_STATUS_STARTVALID;
					tag.tag_status &= ~TAG_STATUS_ENDVALID;
					tag.tag_status &= ~TAG_STATUS_MANUALCLEARED;
					tag.tag_status |= TAG_STATUS_STRECKENVALID;
					RFID_setStatus_UL(tag.tag_status);	//set Status Starttime valid
					rfid_state=RFID_IDLE;
				}
			}else if(AT91F_PIO_IsInputSet(AT91C_BASE_PIOA, PIN_RFID_OUT)){
				switches=0;
				rfid_state=RFID_IDLE;
				tag_detected=NO_TAG_PRESENT;
			}
			break;
		case KEY2:
			Disp_WriteLine("Kategorieauswahl",1);
			sprintf(dummy,"%s",INITSTRUCT.STRECKENSTRING_2);
			Disp_WriteLine(dummy,2);
			if(switches==1){
				switches=0;
				Disp_EraseLine("                ",2); //Zeile 2 löschen
				chooseRace=KEY1;
			}else if(switches==3 && INITSTRUCT.STRECKENKEY_3!=0){
				switches=0;
				Disp_EraseLine("                ",2); //Zeile 2 löschen
				chooseRace=KEY3;
			}else if(switches==3 && INITSTRUCT.STRECKENKEY_3==0){
				switches=0;
				Disp_EraseLine("                ",2); //Zeile 2 löschen
				chooseRace=KEY1;
			}else if(switches==2){
				switches=0;
				Disp_WriteLine("Gewaehlt:       ",1);
				IOE_setOneOutput(IOEXP_OUT_LED_ROT,0);
				IOE_setOneOutput(IOEXP_OUT_LED_GRUEN,1);
				//sprintf(dummy,"%s",INITSTRUCT.STRECKENSTRING_2);
				//Disp_WriteLine(dummy,2);
						//Hier eintrag machen
				rfid_Err=RFID_Operation_UL(&INITSTRUCT.STRECKENKEY_2,WRITE_STRECKENKEY);
				if(!rfid_Err){
						//status setzten und zu idle zurückkehren
					tag.tag_status &= ~TAG_STATUS_STARTVALID;
					tag.tag_status &= ~TAG_STATUS_ENDVALID;
					tag.tag_status &= ~TAG_STATUS_MANUALCLEARED;
					tag.tag_status |= TAG_STATUS_STRECKENVALID;
					RFID_setStatus_UL(tag.tag_status);	//set Status Starttime valid
					rfid_state=RFID_IDLE;
				}
			}else if(AT91F_PIO_IsInputSet(AT91C_BASE_PIOA, PIN_RFID_OUT)){
				switches=0;
				rfid_state=RFID_IDLE;
				tag_detected=NO_TAG_PRESENT;
			}
			break;
		case KEY3:
			Disp_WriteLine("Kategorieauswahl",1);
			sprintf(dummy,"%s",INITSTRUCT.STRECKENSTRING_3);
			Disp_WriteLine(dummy,2);
			if(switches==1){
				switches=0;
				Disp_EraseLine("                ",2); //Zeile 2 löschen
				chooseRace=KEY2;
			}else if(switches==3){
				switches=0;
				Disp_EraseLine("                ",2); //Zeile 2 löschen
				chooseRace=KEY1;
			}else if(switches==2){
				switches=0;
						//Hier eintrag machen
				Disp_WriteLine("Gewaehlt:       ",1);
				IOE_setOneOutput(IOEXP_OUT_LED_ROT,0);
				IOE_setOneOutput(IOEXP_OUT_LED_GRUEN,1);
				//sprintf(dummy,"%s",INITSTRUCT.STRECKENSTRING_3);
				//Disp_WriteLine(dummy,2);
				rfid_Err=RFID_Operation_UL(&INITSTRUCT.STRECKENKEY_3,WRITE_STRECKENKEY);
				if(!rfid_Err){
						//status setzten und zu idle zurückkehren
							//flag 0-3 löschen
					tag.tag_status &= ~TAG_STATUS_STARTVALID;
					tag.tag_status &= ~TAG_STATUS_ENDVALID;
					tag.tag_status &= ~TAG_STATUS_MANUALCLEARED;
					tag.tag_status |= TAG_STATUS_STRECKENVALID;
					RFID_setStatus_UL(tag.tag_status);	//set Status Starttime valid
					rfid_state=RFID_IDLE;
				}
			}else if(AT91F_PIO_IsInputSet(AT91C_BASE_PIOA, PIN_RFID_OUT)){
				switches=0;
				rfid_state=RFID_IDLE;
				tag_detected=NO_TAG_PRESENT;
			}
			break;
	}

}

/**
 * \brief	Diese funktion wandelt die Serienummer vom Binären ins Hexadezimale Format um
 * 
 * \param[in] 	 * serial	
 * \param[in]	 * source	
 * \param[in]	 length	
 */
void RFID_binToHex(char* serial, unsigned char * source, int length){
	int i;
	for(i=0;i<length;++i){
		if((source[i]&0xF)<0xA)
			serial[2*i+1]=(source[i]&0xF)+0x30;
		else
			serial[2*i+1]=(source[i]&0xF)+0x37;
		if((source[i]>>4)<0xA)
			serial[2*i]=(source[i]>>4)+0x30;
		else
			serial[2*i]=(source[i]>>4)+0x37;
	}
	serial[14]=0; // Stringabschluss
	
}

/**
 * \brief	Selektiert die Karte im Feld. Diese Funktion stellt Kollisionen fest 
 * 			und aktiviert die Karte.
 * 
 * \return  NO_ERROR on sucess or a error code (see rfid.h) 
 */
int RFID_select(){
	int i2c_Err = 0;
	int i,timeout;
	char command[3] = {1,SELECT_MIFARE_CARD,0};
	int readLength = 0;
	
	
	i2c_Err = i2c_write(SL030ADR,0,0,command,sizeof(command));
	timeout=100;
	do{
		i2c_Err = i2c_read(SL030ADR,0,0,TempBuf,11,&readLength);
		timeout--;
	}while((i2c_Err != 0) && (timeout != 0));
	
	TRACE("Temp Buf nach Select: ");
	
	for (i=0;i<TempBuf[0];i++){
		TRACE("%0X ",TempBuf[i]);
	}
	TRACE("\n\r");
	
	if(TempBuf[2]==STATUS_NO_TAG){
		return ERROR_RFID_NOTAG;
	}else if(TempBuf[2]==STATUS_COLLISION_OCCUR){
		return ERROR_RFID_COLLISION;
	}
	
	if(TempBuf[0]==10){ // 7 Byte Seriennnummer
		for(i=0;i<7;i++){
			// Seriennummer in Struktur kopieren
			tag.serial_number[i]=TempBuf[i+3];
		}
		// String abschliessen
		tag.serial_number[7]=0;
		// Kartentyp in Struktur kopieren
		tag.type=TempBuf[10];
		RFID_binToHex(tag.serial_number_ascii,tag.serial_number,7);
	}else if(TempBuf[0]==7){ // 4 Byte Serienummer
		for(i=0;i<4;i++){
			// Seriennummer in Struktur kopieren
			tag.serial_number[i]=TempBuf[i+3];
		}
		// String abschliessen
		tag.serial_number[4]=0;
		// Kartentyp in Struktur kopieren
		tag.type=TempBuf[7];
		RFID_binToHex(tag.serial_number_ascii,tag.serial_number,4);
	}
	
	return NO_ERROR;
}

/**
 * \brief	Macht das Login in einen Sektor einer Mifare STD 1k / 4k.
 * 
 * \param[in]	sector	Sektornummer, in welchem eingeloggt wird
 * \param[in]	keyA	Schlüssel zum Einloggen
 * 
 * \return	NO_ERROR on sucess or a error code (see rfid.h) 	 
 */
int RFID_Login_STD(int sector, char * keyA){
	int i2c_Err = 0;
	int timeout;
	char command[10] = {9,LOGIN_TO_SECTOR,sector,0xAA,0};
	int readLength = 0;
	
	strncat(command,keyA,6);
	
	i2c_Err = i2c_write(SL030ADR,0,0,command,sizeof(command));
	timeout=100;
	do{
		i2c_Err = i2c_read(SL030ADR,0,0,TempBuf,3,&readLength);
		timeout--;
	}while((i2c_Err != 0) && (timeout != 0));
	
	if(TempBuf[2]==STATUS_NO_TAG){
		return ERROR_RFID_NOTAG;
	}else if(TempBuf[2]==STATUS_LOGIN_FAILED ||
			 TempBuf[2]==STATUS_LOAD_KEY_FAILED){
		return ERROR_RFID_LOGIN_FAIL;
	}
	TRACE("STATUS in Login Antwort: %X \n\r",TempBuf[2]);
	return NO_ERROR;
}

/**
 * \brief	liest einen Block in einem Sektor einer Mifare STD 1k / 4k.
 * 
 * \param[in]	sector	Sektornummer, in welchem eingeloggt ist
 * \param[in]	block	Blocknummer, welcher gelesen werden soll
 * \param[out]	data	gelesene Daten
 * 
 * \return	NO_ERROR on sucess or a error code (see rfid.h) 	 
 */
int RFID_Read_STD(int sector, int block, char * data){
	int i2c_Err = 0;
	int rfid_Err = 0;
	int i,timeout;
	char keyA[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	char command[4] = {2,READ_DATA_BLOCK,block,0};
	int readLength = 0;
	
	rfid_Err = RFID_Login_STD(sector,keyA);
	
	i2c_Err = i2c_write(SL030ADR,0,0,command,sizeof(command));
	timeout=100;
	do{
		i2c_Err = i2c_read(SL030ADR,0,0,TempBuf,19,&readLength);
		timeout--;
	}while((i2c_Err != 0) && (timeout != 0));
	
	if(TempBuf[2]==STATUS_NO_TAG){
		return ERROR_RFID_NOTAG;
	}else if(TempBuf[2]==STATUS_READ_FAILED){
		return ERROR_RFID_READ_FAILED;
	}else if(TempBuf[2]==STATUS_NOT_AUTHENTICATE){
		return ERROR_RFID_NOT_AUTHENTICATE;
	}
		
	for(i=0;i<16;i++){
		// Daten-Bytes kopieren
		*data++ = TempBuf[i+3];
	}
	// Daten String abschliessen
	*data=0;
	
	return NO_ERROR;
}

/**
 * \brief	liest eine Page einer Mifare Ultralight Karte
 * 
 * \param[in]	page	Seite die gelesen wird
 * \param[in]	data	gelesene Daten
 * 
 * \return	NO_ERROR on sucess or a error code (see rfid.h) 	 
 */
int RFID_Read_UL(int page, char * data){
	int i2c_Err = 0;
	int i,timeout;
	char command[4] = {2,UL_READ_DATA_PAGE,page,0};
	int readLength = 0;
	
	i2c_Err = i2c_write(SL030ADR,0,0,command,sizeof(command));
	timeout=100;
	do{
		i2c_Err = i2c_read(SL030ADR,0,0,TempBuf,7,&readLength);
		timeout--;
	}while((i2c_Err != 0) && (timeout != 0));
	
	if(TempBuf[2]==STATUS_NO_TAG){
		return ERROR_RFID_NOTAG;
	}else if(TempBuf[2]==STATUS_READ_FAILED){
		return ERROR_RFID_READ_FAILED;
	}
		
	for(i=0;i<4;i++){
		// Daten-Bytes kopieren
		*data++ = TempBuf[i+3];
	}
	// Daten String abschliessen
	*data=0;
	
	return NO_ERROR;
}

/**
 * \brief	schreibt eine Page einer Mifare Ultralight Karte
 * 
 * \param[in]	page	Seite die geschrieben wird
 * \param[in]	data	zu schreibende Daten
 * 
 * \return	NO_ERROR on sucess or a error code (see rfid.h) 	 
 */
int RFID_Write_UL(int page, char * data){
	int i2c_Err = 0;
	int i,timeout;
	char command[7] = {2,UL_WRITE_DATA_PAGE,page,0};
	int readLength = 0;
	char datacmp[4];
	
	memcpy(command+3,data,4);
	
	i2c_Err = i2c_write(SL030ADR,0,0,command,sizeof(command));
	timeout=100;
	do{
		i2c_Err = i2c_read(SL030ADR,0,0,TempBuf,7,&readLength);
		timeout--;
	}while((i2c_Err != 0) && (timeout != 0));
	
	if(TempBuf[2]==STATUS_NO_TAG){
		return ERROR_RFID_NOTAG;
	}else if(TempBuf[2]==STATUS_WRITE_FAILED ||
			 TempBuf[2]==STATUS_UNABLE_READ_AFTER_WRITE ||
			 TempBuf[2]==STATUS_READ_AFTER_WRITE_ERROR){
		return ERROR_RFID_WRITE_FAILED;
	}
		
	for(i=0;i<4;i++){
		// Daten-Bytes kopieren für Vergleich
		datacmp[i] = TempBuf[i+3];
	}
	
	// Validierung der geschriebenen Daten
	if(strncmp(datacmp,data,4)!=0){
		return ERROR_RFID_WRITE_FAILED;
	}
	
	return NO_ERROR;
}

/**
 * \brief	Schickt das RFID-Modul in den Suspend-Mode
 * 
 * 
 * \return	NO_ERROR	 
 */
int RFID_suspend(){
	//int i2c_Err = 0;
	char command[3] = {1,POWER_DOWN_MODE,0};
	i2c_write(SL030ADR,0,0,command,sizeof(command));
	return NO_ERROR;
}

/**
 * \brief	liest den Status auf der Page 4 (Byte 0) in einer Mifare Ultralight Karte
 * 			und schreibt diesen in die globale Tag-Struktur
 * 
 * \return	NO_ERROR on sucess or a error code (see rfid.h) 	 
 */
int RFID_getStatus_UL(void){
	int rfid_Err = 0;
	char data[4];
	rfid_Err = RFID_Read_UL(RFID_ADR_STATUS,data);
	
	if(rfid_Err == ERROR_RFID_NOTAG)
		return ERROR_RFID_NOTAG;
	else if(rfid_Err == ERROR_RFID_READ_FAILED)
		return ERROR_RFID_READ_FAILED;
	
	if(data[0]	& TAG_STATUS_STRECKENVALID)
		tag.tag_status |= TAG_STATUS_STRECKENVALID;
	else 
		tag.tag_status &= ~TAG_STATUS_STRECKENVALID;
	if(data[0] & TAG_STATUS_STARTVALID)
		tag.tag_status |= TAG_STATUS_STARTVALID;
	else
		tag.tag_status &= ~TAG_STATUS_STARTVALID;
	if(data[0] & TAG_STATUS_ENDVALID)
		tag.tag_status |= TAG_STATUS_ENDVALID;
	else
		tag.tag_status &= ~TAG_STATUS_ENDVALID;
	if(data[0] & TAG_STATUS_MANUALCLEARED)
		tag.tag_status |= TAG_STATUS_MANUALCLEARED;
	else
		tag.tag_status &= ~TAG_STATUS_MANUALCLEARED;
	if(data[0] & TAG_STATUS_REGISTERED)
		tag.tag_status |= TAG_STATUS_REGISTERED;
	else
		tag.tag_status &= ~TAG_STATUS_REGISTERED;
	
	return NO_ERROR;
}

/**
 * \brief	schreibt den Status auf der Page 4 (Byte 0) in einer Mifare Ultralight Karte
 * 			und aktualisiert diesen in der globalen Tag-Struktur
 * 
 * \param[in]	status		neuer Status. Achtung!: muss beim Aufruf mit den bestehenden zusammengeführt werden.
 * 
 * \return	NO_ERROR on sucess or a error code (see rfid.h) 	 
 */
int RFID_setStatus_UL(char status){
	int rfid_Err = 0;
	char data[4]={0,0,0,0};
	
	if(status & TAG_STATUS_STRECKENVALID){
		data[0]|=TAG_STATUS_STRECKENVALID;
		tag.tag_status |= TAG_STATUS_STRECKENVALID;
	}else{
		tag.tag_status &= ~TAG_STATUS_STRECKENVALID;
	}
	if(status & TAG_STATUS_STARTVALID){
		data[0]|=TAG_STATUS_STARTVALID;
		tag.tag_status |= TAG_STATUS_STARTVALID;
	}else{
		tag.tag_status &= ~TAG_STATUS_STARTVALID;
	}
	if(status & TAG_STATUS_ENDVALID){
		data[0]|=TAG_STATUS_ENDVALID;
		tag.tag_status |= TAG_STATUS_ENDVALID;
	}else{
		tag.tag_status &= ~TAG_STATUS_ENDVALID;
	}
	if(status & TAG_STATUS_MANUALCLEARED){
		data[0]|=TAG_STATUS_MANUALCLEARED;
		tag.tag_status |= TAG_STATUS_MANUALCLEARED;
	}else{
		tag.tag_status &= ~TAG_STATUS_MANUALCLEARED;
	}
	if(status & TAG_STATUS_REGISTERED){
		data[0]|=TAG_STATUS_REGISTERED;
		tag.tag_status |= TAG_STATUS_REGISTERED;
	}else{
		tag.tag_status &= ~TAG_STATUS_REGISTERED;
	}
	
	rfid_Err = RFID_Write_UL(RFID_ADR_STATUS,data);
	
	if(rfid_Err == ERROR_RFID_NOTAG)
		return ERROR_RFID_NOTAG;
	else if(rfid_Err == ERROR_RFID_WRITE_FAILED)
		return ERROR_RFID_WRITE_FAILED;
	
	return NO_ERROR;
}

/**
 * \brief	schreibt das Zeiten und Datums auf die Karte in die richtigen Pages (Mifare UL)
 * 
 * \param[in]	pDate 	Pointer auf die Datumsstruktur 
 * \param[in]	mode	RFID-Operation mode		
 * 
 * \return	NO_ERROR on sucess or a error code (see rfid.h) 	 
 */
int RFID_Operation_UL(void * pDate, enum RFID_OPERATION_MODE mode){
	int i = 0,rfid_Err = 0;
	char tempData[20];
	switch(mode){
		case WRITE_STARTTIME:
			memcpy(tempData,(RFIDDATE*)pDate,7);
			do{
				rfid_Err = RFID_Write_UL(RFID_ADR_STARTTIME+i,tempData+i*4);
				i++;
			}while((i<2) && (rfid_Err==0));
		break;
		case READ_STARTTIME:
			do{
			rfid_Err = RFID_Read_UL(RFID_ADR_STARTTIME+i,tempData+i*4);
			i++;
			}while((i<2) && (rfid_Err==0));
			memcpy((RFIDDATE*)pDate,tempData,7);
		break;
		case WRITE_STRECKENKEY:
			memcpy(tempData,pDate,2);
			do{
				rfid_Err = RFID_Write_UL(RFID_ADR_STRECKENKEY+i,tempData+i*4);
				i++;
			}while((i<1) && (rfid_Err==0));
		break;
		case READ_STRECKENKEY:
			do{
				rfid_Err = RFID_Read_UL(RFID_ADR_STRECKENKEY+i,tempData+i*4);
				i++;
			}while((i<1) && (rfid_Err==0));
			memcpy(pDate,tempData,2);
			
		break;
		case WRITE_ENDTIME:
			memcpy(tempData,(RFIDDATE*)pDate,7);
			do{
				rfid_Err = RFID_Write_UL(RFID_ADR_ENDTIME+i,tempData+i*4);
				i++;
			}while((i<2) && (rfid_Err==0));
		break;	
		case READ_ENDTIME:
			do{
				rfid_Err = RFID_Read_UL(RFID_ADR_ENDTIME+i,tempData+i*4);
				i++;
			}while((i<2) && (rfid_Err==0));
			memcpy((RFIDDATE*)pDate,tempData,7);
		break;	
		case WRITE_RACETIME:
			memcpy(tempData,((RFIDTIME*)pDate),4);
			do{
				rfid_Err = RFID_Write_UL(RFID_ADR_RACETIME+i,tempData+i*4);
				i++;
			}while((i<1) && (rfid_Err==0));
		break;
		case READ_RACETIME:
			do{
				rfid_Err = RFID_Read_UL(RFID_ADR_RACETIME+i,tempData+i*4);
				i++;
			}while((i<1) && (rfid_Err!=0));
			memcpy(((RFIDTIME*)pDate),tempData,4);
		break;
	}
	
	return NO_ERROR;
}

/**
 * \brief	Initialisiert das RFID-Modul.
 * 
 * 
 * \return	NO_ERROR on sucess or a error code (see rfid.h) 	 
 */
int RFID_Init(){
	timeout=0;
	rfid_Err=0;
	chooseRace=IN;
	rfid_state=RFID_IDLE;
	dataTOsend=0;
	// Initialisieren der I2C-Schnittstelle
	//i2c_init();
	// Enable Peripheral clock in PMC for PIOA
	AT91F_PIOA_CfgPMC();
	// Configure Pins to direct drive
	AT91F_PIO_CfgDirectDrive(AT91C_BASE_PIOA, PIN_RFID_IN);
	// configure IOs: RS, Reset
	AT91F_PIO_CfgOutput(AT91C_BASE_PIOA, PIN_RFID_IN);
	AT91F_PIO_CfgInput(AT91C_BASE_PIOA, PIN_RFID_OUT);
	// löschen des IN-Signals (Falling edge wake up from power down mode)
	AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, PIN_RFID_IN);
	// detektieren einer Karte
	if(AT91F_PIO_IsInputSet(AT91C_BASE_PIOA, PIN_RFID_OUT)){
		tag_detected = NO_TAG_PRESENT;
		TRACE("NO_TAG_PRESENT \n\r");
	}else{
		tag_detected = TAG_PRESENT;
		TRACE("TAG_PRESENT \n\r");
	}
	return NO_ERROR;
}


