/**
 *******************************************************************************
 * \file    gsmgps.c
 *******************************************************************************
 * \brief    Telit GSM/GPS driver and state machine for AT91SAM7Sxxx.
 *
 *
 * \version		1.0
 * \date		06.05.2008
 * \author		M. Leiser
 *
 *******************************************************************************
 */

#include <AT91SAM7S256.h>
#include <lib_AT91SAM7S256.h>

#include "../system/printf.h"
#include "uart.h"
#include "gsmgps.h"
#include "global.h"
#include "timer.h"
#include "string.h"
#include "stdio.h"
#include "rtc.h"
#include "display.h"
#include "rfid.h"
#include "sd-card.h"
#include "../filesystem/diskio.h"
#include "../filesystem/tff.h"
//#include "stdlib.h"			//zu gross! -> kann nicht mehr im RAM debuggen!


/**
 *\brief Global Variables 
 **/
int recieved=0;
int startedup=0;
int ExtRecieved=0;
int ExtSent=0;
int summertime;
static int NoAnswercount=0;
char error[20];
static int trymax=0;
int connections=0;

/**
 * \fn 		void gsmgps())
 * 
 * \brief  	State Machine GSM / GPS
 *  

 **/
void gsmgps(){
//	TRACE("AUFRUF: GSMGPS STATE MACHINE\n\r");
	FTRACE("AUFRUF: GSMGPS STATE MACHINE\r\n");
	switch(gsmgps_state){
				
				case INIT_GSM: 
					init_gsm();
					
					
					break;
				case IDLE:
					//warten auf GPS FIX -> time eintragen in rtc
					idle();
					break;
				case GPRS_CONNECT:
					
					gprs_connect();
					break;		
				case CONNECT_TRYMAX:
					
					if(sleep){
						gsmgps_state=GSM_SLEEP;
					}
					
					break;	
				case GPRS_CONNECTED:
					gprs_connected();
					break;
				case GSM_SLEEP:
					gsm_sleep();
								
					break;

				case GSM_ERROR_STATE:							
					if(strstr(error,"NO_NET")){
						FTRACE("ERROR: Kein GSM Empfang -> bitte Anlage verschieben\r\n");
						TRACE("ERROR: Kein GSM Empfang -> bitte Anlage verschieben\n\r");
						Disp_WriteLine("Kein GSM Empfang",1);
						Disp_WriteLine("Anlage umstellen",2);
						if(timer2000==0){
							gsmgps_state=INIT_GSM;
							init_state=HW_ON;
						}
					}
					if(strstr(error,"NO_GPS")){
						
						FTRACE("ERROR: Kein GPS Empfang -> bitte Anlage verschieben\r\n");
						TRACE("ERROR: Kein GPS Empfang -> bitte Anlage verschieben\n\r");
						Disp_WriteLine("Kein GPS Empfang",1);
						Disp_WriteLine("Anlage umstellen",2);
						if(timer2000==0){
							gsmgps_state=INIT_GSM;
							init_state=HW_ON;
						}
					}
//					if(strstr(error,"GPRS_MAX")){
//						//Send SMS with warning ausserdem vll fehler ändern!
//						TRACE("ERROR: Zu viele GPRS Verbindungen pro Tag\n\r");	
//						FTRACE("ERROR: Zu viele GPRS Verbindungen pro Tag\r\n");	
//					}
					if(strstr(error,"TELIT_OFF")){
						
						gsmgps_state=INIT_GSM;
						init_state=HW_ON;
						
						
						//Disp_WriteLine("GSM Modul nicht ",1);
						//Disp_WriteLine("gestartet       ",2);
						TRACE("ERROR: TELIT Kann nicht gestartet werden\n\r");	
						FTRACE("ERROR: TELIT OFF Kann nicht gestartet werden\r\n");
					}
				
					if(strstr(error,"NoAnswerMax")){
						//Shut down and restart
						gsmgps_state=INIT_GSM;
						init_state=HW_ON;
						TRACE("ERROR: Keine Antwort vom Telit Modul\n\r");	
						FTRACE("ERROR: Keine Antwort vom Telit Modul\r\n");	
					}
				
					if(strstr(error,"WrongAnswerMax")){
						//Shut down and restart
						gsmgps_state=INIT_GSM;
						init_state=HW_ON;
						TRACE("ERROR: Falsche Antwort vom Telit Modul\n\r");	
						FTRACE("ERROR: Falsche Antwort vom Telit Modul\r\n");
					}
					break;
				
			}//end switch case
			
	
}

/**
 * \fn 		void init_gsm())
 * 
 * \brief  	State Machine GSM / GPS -> INITSTATE
 *  
 * 
 **/
void init_gsm(){
	static int trymax;
	static int set=0;
	static char dummy[40];
	TRACE("STATE: INIT_GSM\n\r");
	FTRACE("STATE: INIT_GSM\r\n");
	if(startedup==0){
	Disp_WriteLine("Geraet startet  ",1);
	}
	switch(init_state){
				
				case HW_ON: 
				if(set==0){
					hw_on_timer=1100;
					set=1;
				}
				//setze Output = 1 und Warte 1.5 Sekunden
					//GSM Power ON (3,8 V)
					AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, AT91C_PIO_PA7);
					delay(8);
					AT91F_PIO_SetOutput(AT91C_BASE_PIOA, PIN_TELIT_ON_OFF);
				if(hw_on_timer==0){
					AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, PIN_TELIT_ON_OFF);
					//prüft Pin PWRMON
					
					if(gsmgps_check()){
						i_Char=0;
						init_state=SEND1;
						trymax=0;
						set=0;
					}else{
						trymax++;
						set=0;
					}
					if(trymax>=3){
						trymax=0;
						set=0;
						sprintf(error,"TELIT_OFF");
						gsmgps_state=GSM_ERROR_STATE;
					}
				}
					
					
					
					
					
				
					
					
					break;
				case SEND1:
					TRACE("Putandcheck AT\n\r");
					FTRACE("Putandcheck AT\r\n");
					gsmgps_putAndCheck(AT,"OK",&init_state);
					
					break;
				case SEND2:
					TRACE("Putandcheck AT_IPR\n\r");
					FTRACE("Putandcheck AT_IPR\r\n");
					gsmgps_putAndCheck(AT_IPR,"OK",&init_state);
								
					break;
				case SEND3:
					TRACE("Putandcheck AT_K0\n\r");
					FTRACE("Putandcheck AT_K0\r\n");
					gsmgps_putAndCheck(AT_K0,"OK",&init_state);
											
					break;
				case SEND4:
					TRACE("Putandcheck AT_ERROR\n\r");
					FTRACE("Putandcheck AT_ERROR\r\n");
					gsmgps_putAndCheck(AT_ERROR_VERBOSE,"OK",&init_state);					
															
					break;
				case SEND5:
					TRACE("Putandcheck AT_SIM_PRES.\n\r");
					FTRACE("Putandcheck AT_SIM_PRES.\r\n");
					gsmgps_putAndCheck(AT_SIM_PRESENCE,"OK",&init_state);					
																			
					break;
				case SEND6:
					trymax=0;
					if(strstr(answer,"SIM PIN")){			//provide SIM Pin
						sprintf(dummy,"%s%s\r",AT_PIN,INITSTRUCT.SIM_PIN);
						TRACE("DUMMY: %s\n\r",dummy);
						TRACE("Putandcheck AT_PIN\n\r");
						FTRACE("Putandcheck AT_PIN\r\n");
						gsmgps_putAndCheck(dummy,"OK",&init_state);	
					}else if(strstr(answer,"SIM PUK")){		//provide SIM Puk
						sprintf(dummy,"%s%s,%s\r",AT_PIN,INITSTRUCT.SIM_PUK,INITSTRUCT.SIM_PIN);
						TRACE("DUMMY: %s",dummy);
						TRACE("Putandcheck AT_PIN\n\r");
						FTRACE("Putandcheck AT_PIN\r\n");
						gsmgps_putAndCheck(dummy,"OK",&init_state);	
					}else if(strstr(answer,"READY")){		//no PIN required
						init_state=SEND7;
						TRACE("No Pin required\n\r");
						FTRACE("No Pin required\n\r");
					}else{
						gsmgps_putAndCheck(dummy,"OK",&init_state);		
						
					}
																			
					break;
				case SEND7:
					if((strstr(answer,"OK")||strstr(answer,"READY"))&& timer2000==0){
						gsmgps_putAndCheck(AT_NET_STATE,"OK",&init_state);
					
					}else if(strstr(answer,"ERROR")){
						init_state=SEND5;	//go back and ask wether PIN or PUK ist required
					}
								
							
					
					break;
				case SEND8:
					if(strstr(answer,"+CREG: 0,1")||strstr(answer,"+CREG: 1,1")||strstr(answer,"+CREG: 0,5")||strstr(answer,"+CREG: 5,5")){
						TRACE("Registered on GSM Network\n\r");
						trymax=0;
						
						gsmgps_state=IDLE;
					
					}else if(trymax>=40){
						TRACE("Trymax=%d\n\r",trymax);
						TRACE("NO GSM Network found\n\r");
						if(INITSTRUCT.STARTorFINISH){
							TRACE("Fehler - Kein Netz\n\r");
							timer2000=10000;
							sprintf(error,"NO_NET");
							gsmgps_state=GSM_ERROR_STATE;
						}else{
							TRACE("Go to IDLE - No net but Startmachine doesn't need\n\r");
							gsmgps_state=IDLE;
						}
					}else{
						TRACE("Trying to find GSM Net\n\r");
						timer2000=2000;
						init_state=SEND7;
						trymax++;
					}
					break;
			}//end switch case
			
	
}










/**
 * \fn 		void idle())
 * 
 * \brief  	State Machine GSM / GPS -> IDLE_START_STATE
 * 
 **/
void idle(){
	TRACE("STATE: IDLE\n\r");
	FTRACE("STATE: IDLE\r\n");
	char dummy[17];
	//char dummy2[17];
	TIME date;
	switch(idle_state){
				
				case GPS_FIX:
					TRACE("STATE: GPS_FIX_START\n\r");
					FTRACE("STATE: GPS_FIX_START\r\n");
					TRACE("atTimeout= %d\n\r",atTimeout);
					TRACE("NrofSat= %d\n\r",NMEA_Protocol.NrOfSat);
					if(atTimeout==0){	//jede sekunde solange kein satelitt gefunden
						atTimeout=1000;
						TRACE("Putandcheck AT$GPSACP\n\r");
						FTRACE("Putandcheck AT$GPSACP\r\n");
						if(startedup==0){
						Disp_WriteLine("Warten auf GPS- ",1);
						Disp_WriteLine("Empfang         ",2);
						Disp_WriteLine("Empfang         ",2);
						}
						TRACE("TRYMAX=%d\n\r",trymax);
						if(trymax<300){	//während 5 minuten
							gsmgps_putAndCheck("AT$GPSACP\r", "OK", &idle_state);
							trymax++;
						}else{
							sprintf(error,"NO_GPS");
							timer2000=10000;
							gsmgps_state=GSM_ERROR_STATE;
							
						}
					}
					
					break;
				case GPS_FIX2:
					TRACE("STATE: GPS_FIX2\n\r");
					if(!NMEA_Protocol.NrOfSat){
						idle_state=GPS_FIX;
					}else{
						trymax=0;
						if(startedup==0){
						Disp_WriteLine("GPS Empfang OK  ",1);
						Disp_EraseLine("                ",2);
						}
						TRACE("GPS Time Fix OK\n\r");
						FTRACE("GPS Time Fix OK\r\n");
						//zeit im RTC setzen
						rtc_set_time_from_NMEA();
						time_valid=1;
						TRACE("Switch GPS Off\n\r");
						FTRACE("Switch GPS Off\r\n");
						gsmgps_putAndCheck("AT$GPSACP\r", "OK", &idle_state);
						timer2000=2000;
					}
					
					break;
				case GPS_OFF:
					//GPS OFF - GSM ON - 
					if(timer2000!=0){
						if(startedup==0){
							Disp_WriteLine("Erfolgreich     ",1);
							Disp_WriteLine("aufgestartet    ",2);
						}
						rtc_get_time(&date);
						sprintf(dummy,"%02d.%02d - %02d:%02d:%02d",NMEA_Protocol.day,NMEA_Protocol.month, NMEA_Protocol.hour,NMEA_Protocol.minute,NMEA_Protocol.second);
						Disp_WriteLine(dummy,3);
						time_valid=1;
					}else if(timer2000==0 && (!INITSTRUCT.STARTorFINISH || !INITSTRUCT.USE_GPRS)){
						startedup=1;
					}
					TRACE("GPS off Idle\n\r");
					FTRACE("GPS off Idle\r\n");
//					if(INITSTRUCT.STARTorFINISH && INITSTRUCT.USE_GPRS && timer2000==0){
//						startedup=1;
//						gsmgps_state=GPRS_CONNECT;
//					}else{
//						if(sleep){
					startedup=1;
							gsmgps_state=GPRS_CONNECT;
							gprs_connect_state=CONTEXT_SETTING;
//						}
//					}							
					break;	
				
				
			}//end switch case
			
	
}


/**
 * \fn 		void gsm_sleep())
 * 
 * \brief  	State Machine GSM / GPS -> SLEEP_START_STATE
 * 
 **/
void gsm_sleep(){
	TRACE("STATE: SLEEP\n\r");
	FTRACE("STATE: SLEEP\r\n");
	TIME date;
	int leastwakeuptime=0;
	//TODO: GO To sleep and make error secure
	switch(gsm_sleep_state){
				
				case GOTO_SLEEP:
					TRACE("STATE: GOTO_SLEEP\n\r");
					FTRACE("GOTO_SLEEP\r\n");
					gsmgps_HW_off();
					gsm_sleep_state=IN_SLEEP;
					break;
				case IN_SLEEP:
					//TRACE("STATE: IN_SLEEP\n\r");
					//FTRACE("STATE: IN_SLEEP\r\n");
					rtc_get_time(&date);
					leastwakeuptime=(INITSTRUCT.GOODNIGHT_TIME-1);
					if(leastwakeuptime<0){
						leastwakeuptime=23;
					}
					if(!sleep && INITSTRUCT.STARTorFINISH){	//zielanlage wakeup
						gsm_sleep_state=WAKEUP;
					}else if(!INITSTRUCT.STARTorFINISH && start_sync){ //startanlage wakeup
						gsm_sleep_state=WAKEUP;
					}else if(sleep && INITSTRUCT.STARTorFINISH && date.stunden == leastwakeuptime){ //falls ein ganzer tag kein fahrer gekommen ist, uss die anlage trotzdem aufwachen und synchronisieren
						gsm_sleep_state=WAKEUP;
						sleep=0;
					}
					
					break;
				case WAKEUP:
					TRACE("STATE: WAKEUP\n\r");
					FTRACE("STATE: WAKEUP\r\n");
					//wakeup from sleep
					//setze verbindungscounter auf 0
					connections=0;
					
						gsmgps_state=INIT_GSM;
						init_state=HW_ON;
						idle_state=GPS_FIX;
						gprs_connect_state=CONTEXT_SETTING;
						gprs_connected_state=CONNECTED_IDLE;
						NMEA_Protocol.NrOfSat=0;
						
					break;	
				
				
			}//end switch case

}


/**
 * \fn 		void gprs_connect())
 * 
 * \brief  	State Machine GSM / GPS -> GPRS_CONNECT
 * 
 **/
void gprs_connect(){
	
	static char dummy[60];
	TRACE("STATE: GPRS_CONNECT\n\r");
	FTRACE("STATE: GPRS_CONNECT\r\n");
	switch(gprs_connect_state){
				
				case CONTEXT_SETTING: 
					TRACE("CONEXT_SETTING\n\r");
					FTRACE("CONEXT_SETTING\r\n");
					sprintf(dummy,"AT+CGDCONT=1,\"IP\",\"%s\",\"0.0.0.0\",0,0\r",INITSTRUCT.WAPAPN); //set gprs context
					gsmgps_putAndCheck(dummy,"OK",&gprs_connect_state);
					//sprintf(dummy,"AT#USERID=\"%s\"\r",INITSTRUCT.GPRS_USERNAME); //set gprs username
					//gsmgps_putAndCheck(dummy,"OK",&gprs_connect_state);
					break;
				case CONTEXT_SETTING1: 
					TRACE("CONEXT_SETTING1\n\r");
					FTRACE("CONEXT_SETTING1\r\n");
					sprintf(dummy,"AT#USERID=\"%s\"\r",INITSTRUCT.GPRS_USERNAME); //set gprs username
					gsmgps_putAndCheck(dummy,"OK",&gprs_connect_state);
				break;
				case CONTEXT_SETTING2: 
					TRACE("CONEXT_SETTING2\n\r");
					FTRACE("CONEXT_SETTING2\r\n");
					sprintf(dummy,"AT#PASSW=\"%s\"\r",INITSTRUCT.GPRS_PW); //set gprs PW
					gsmgps_putAndCheck(dummy,"OK",&gprs_connect_state);
				break;
				case CHECK_CONNECTION:
					TRACE("CHECK_CONNECTION\n\r");
					FTRACE("CHECK_CONNECTION\r\n");
					gsmgps_putAndCheck("AT#GPRS?\r","OK",&gprs_connect_state);
				break;
				case CHECK_CONNECTION2:
					TRACE("CHECK_CONNECTION2\n\r");
					FTRACE("CHECK_CONNECTION2\r\n");
					if(strstr(answer,"#GPRS: 1")){
						gprs_connect_state=CONNECT2;
					}else{
						gprs_connect_state=CONNECT1;
					}
				break;
				case CONNECT1:
					TRACE("CONNECT1\n\r");
					FTRACE("CONNECT1\r\n");
					
					if(strstr(answer,"already activated")){
						gsmgps_state=CONNECT2;
					}
					
					if(connections<=3){
						TRACE("activate connection\n\r");
						FTRACE("activate connection\r\n");
						gsmgps_putAndCheck("AT#GPRS=1\r","OK",&gprs_connect_state);
						//TRACE("Connection ANSWER: %s\n\r",answer); //testzweck
					}else{
						TRACE("To much connections per Day\n\r");
						FTRACE("To much connections per Day\r\n");
						
						gsmgps_state=CONNECT_TRYMAX;
					}	
					
					break;
				case CONNECT2:
					TRACE("CONNECT2\n\r");
					FTRACE("CONNECT2\r\n");
					TRACE("GPRS VERBINDUNG OK\n\r");
					TRACE("Connection ANSWER: %s\n\r",answer);
					FTRACE("Connection ANSWER: %s\n\r",answer);
					gsmgps_state=GPRS_CONNECTED;
					gprs_connected_state=CONNECTED_IDLE;
					break;
				
	}
}


/**
 * \fn 		void gprs_connected())
 * 
 * \brief  	State Machine GSM / GPS -> GPRS_CONNECTED
 * 
 **/
void gprs_connected(){
	static int rowNr,res;
	static int NrOfFiles;
	//static char dummy[104];
	static char row[100];
	static char data[100];
	static char filename[30];
	TRACE("STATE: GPRS_CONNECTED\n\r");
	FTRACE("STATE: GPRS_CONNECTED\r\n");
	switch(gprs_connected_state){
				
				case CONNECTED_IDLE: 
					
					if(sleep==1 || !INITSTRUCT.STARTorFINISH){
						gprs_connected_state=PUT_OK;
					}else if (timer2000==0){
						gsmgps_putAndCheck("AT#GPRS?\r","OK",&gprs_connected_state); //check if gprs connected
					}
					break;
				case CONNECTED_IDLE2: 
					TRACE("ANSWER:%s",answer);
					if(strstr(answer,"#GPRS: 1")){	//wenn GPRS immernoch verbunden
						if(dataTOsend){
							gprs_connected_state=PUT_SEMA;
						}else{
							gprs_connected_state=CONNECTED_IDLE;
							timer2000=2000;
						}
					}else{
						gsmgps_state=GPRS_CONNECT;	//falls verbindung abgebrochen	
						gprs_connect_state=CONNECT1;
						connections++;				//wird beim wakeup auf 0 gestellt!
					}
					break;					
				case PUT_SEMA:
					//writes a semaphore file on the server, so that the script doesn't run.
					TRACE("put sema file\n\r");
					ftp_putfile("ftp_sema", " ");
					break;
				case CHECK_DATA:			
					TRACE("get file time.dat\n\r");
					ftp_getfile("time.dat", data);
					//NrOfFiles=selfAtoi(data);
					break;
				case CHECK_DATA2:
					NrOfFiles=selfAtoi(data);
					//TRACE("CHECK_DATA2->ANSWER:%s\n\r",dummy);
					TRACE("CHECK_DATA2->NrOfFiles:%d\n\r",NrOfFiles);
					rowNr=1;	//rownumber to get
					gprs_connected_state=PREPARE_DATA;
					break;
				case PREPARE_DATA:
			
					res=getFileRow(rowNr,row);			//get row from file		
					if(res==NO_ERROR){
						rowNr++;
						NrOfFiles++;
						gprs_connected_state=SEND_DATA;
					}else if(res==END_OF_FILE){
						gprs_connected_state=SEND_NROFFILES;
						f_unlink("time.txt");		//löscht file nach verarbeitung
						dataTOsend=0;
					}else{
						//TODO: go to error state (file could not be opened)
					}	
					break;
				
				case SEND_DATA:
						sprintf(filename,"time%d.txt",NrOfFiles);
						ftp_putfile(filename,row);
					break;
				case SEND_DATA2:
					gprs_connected_state=PREPARE_DATA;
					break;
//				case SEND_NROFFILES:
//					ftp_deletefile("time.dat");			
//					break;
				case SEND_NROFFILES:
					sprintf(data,"%d",NrOfFiles);
					ftp_putfile("time.dat",data);			
					break;
				case SEND_DELETESEMA:
					//deletes the semaphore file
					ftp_deletefile("ftp_sema");		
					break;	
				case GOTO_IDLE:
					gprs_connected_state=CONNECTED_IDLE;		
					break;
				case PUT_OK:
					//send security file (new state)
					ftp_putfile("ok.dat"," ");
					break;
				case GPRS_DISCONNECT:
					//Disconnect GPRS CONNECTION
					gsmgps_putAndCheck("AT#GPRS=0\r","OK",&gprs_connected_state);
					
					break;
				case GPRS_DISCONNECT2:
					start_sync=0;
					gsmgps_state=GSM_SLEEP; 
					gsm_sleep_state=GOTO_SLEEP;
					break;
	}
}

/**
 * \fn 			void ftp_putfile())
 * 
 * \brief  		erstellt eine Datei auf dem Server. GPRS Verbindung muss bestehen
 * \param in	Filename und Daten
 * 
 * 
 * 
 **/
void ftp_putfile(char * filename, char * data){
	static char dummy[104];

	
	switch(ftp_putfile_state){
									
					case FTP_PUT:
						i_Char=0;
						gsmgps_putcommand("AT#FTPCLOSE=?\r");
						ftp_putfile_state=FTP_PUT1;
						break;
					case FTP_PUT1:
						if(ExtRecieved && strstr(answer,"OK")){
							ExtRecieved=0;
							ExtSent=0;
							i_Char=0;
							ftp_putfile_state=FTP_PUT2a;
							
						}else if(ExtRecieved && !strstr(answer,"OK")){
							ftp_putfile_state=FTP_PUT2b;
						}
						break;
					case FTP_PUT2a:

						sprintf(dummy,"AT#FTPOPEN=\"%s:21\",\"%s\",\"%s\",0\r",INITSTRUCT.FTP_SERVER,INITSTRUCT.FTP_USERNAME,INITSTRUCT.FTP_PW);
						gsmgps_putAndCheck(dummy,"OK",&ftp_putfile_state);
						if(strstr(answer,"Already connected")){
							ftp_putfile_state=FTP_PUT2b;
						}
						break;
					case FTP_PUT2b:
						TRACE("SET FTP MODE to ASCII");	
						gsmgps_putAndCheck("AT#FTPTYPE=1\r","OK",&ftp_putfile_state); //ftptype=1=ASCII Mode
						//TRACE("       ANSWER: %s ", answer);
						
						break;
					case FTP_PUT3:
						sprintf(dummy,"AT#FTPPUT=\"%s\"\r",filename);
						gsmgps_putAndCheck(dummy,"CONNECT",&ftp_putfile_state);
						//TRACE("       ANSWER: %s ", answer);	
						break;
					case FTP_PUT4:
						gsmgps_putcommand(data);
						timer2000=2000;
						ftp_putfile_state=FTP_PUT5;	
						break;
					case FTP_PUT5:		
						if(!timer2000){			//wait 2s till end
							gsmgps_putAndCheck("+++","NO CARRIER",&ftp_putfile_state);
							//TRACE("       ANSWER: %s ", answer);
							timer2000=2000;
						}
					case FTP_PUT6:
						if(!timer2000){			//wait 2s till end
							gsmgps_putAndCheck("AT#FTPCLOSE\r","OK",&ftp_putfile_state);
							//TRACE("       ANSWER: %s ", answer);
						}
						break;
					case FTP_PUT7:
						TRACE("DATEI ERFOLGREICH GESCHRIEBEN\n\r");
						TRACE("FTP VERBINDUNG GESCHLOSSEN\n\r");
						ftp_putfile_state=FTP_PUT;
						gprs_connected_state++;
						break;
		}
	

}

/**
 * \fn 			void ftp_getfile())
 * 
 * \brief  		holt eine Datei vom Server. GPRS Verbindung muss bestehen
 * \param in	filename und pointer zu String für daten
 * 
 * 
 * 
 **/
void ftp_getfile(char * filename, char * data){
	static char dummy[104];
	int i=0;
	switch(ftp_getfile_state){
									
					case FTP_GET:
						i_Char=0;
						gsmgps_putcommand("AT#FTPCLOSE=?\r");
						ftp_getfile_state=FTP_GET1;
						break;
					case FTP_GET1:
						if(ExtRecieved && strstr(answer,"OK")){
							ExtRecieved=0;
							ExtSent=0;
							i_Char=0;
							ftp_getfile_state=FTP_GET2a;
						}else if(ExtRecieved && !strstr(answer,"OK")){
							ftp_getfile_state=FTP_GET2b;
						}
						break;
					case FTP_GET2a:
						sprintf(dummy,"AT#FTPOPEN=\"%s:21\",\"%s\",\"%s\",0\r",INITSTRUCT.FTP_SERVER,INITSTRUCT.FTP_USERNAME,INITSTRUCT.FTP_PW);
						gsmgps_putAndCheck(dummy,"OK",&ftp_getfile_state);
						//TRACE("       ANSWER: %s ", answer);
						if(strstr(answer,"Already connected")){
							ftp_putfile_state=FTP_GET2b;
						}
						break;
					case FTP_GET2b:
						TRACE("SET FTP MODE to ASCII");	
						gsmgps_putAndCheck("AT#FTPTYPE=1\r","OK",&ftp_getfile_state); //ftptype=1=>ASCII Mode
						//TRACE("       ANSWER: %s ", answer);
						break;
					case FTP_GET3:
							sprintf(dummy,"AT#FTPGET=\"%s\"\r",filename);
							gsmgps_putAndCheck(dummy,"CONNECT",&ftp_getfile_state);
							//TRACE("       ANSWER: %s ", answer);
						break;
					case FTP_GET4:
						if(ExtRecieved && strstr(answer,"NO CARRIER")){
						ExtRecieved=0;
						ExtSent=0;
						while(answer[i]>='0' && answer[i]<='9'){
						data[i]=answer[i];
						i++;
						data[i]='\0';
						}
						ftp_getfile_state=FTP_GET6;
						}	
						break;
					case FTP_GET5:		
						
							ftp_getfile_state=FTP_GET6;
					case FTP_GET6:
						gsmgps_putAndCheck("AT#FTPCLOSE\r","OK",&ftp_getfile_state);		
						//TRACE("       ANSWER: %s ", answer);
						break;
					case FTP_GET7:
						TRACE("FTP VERBINDUNG GESCHLOSSEN");
						ftp_getfile_state=FTP_GET;
						gprs_connected_state++;
						break;
		}
	

}


/**
 * \fn 			int ftp_deletefile())
 * 
 * \brief  		löscht eine Datei auf dem Server. GPRS Verbindung muss bestehen
 * \param in	Filename
 * 
 * 
 * 
 **/
void ftp_deletefile(char * filename){
	static char dummy[104];

	
	switch(ftp_deletefile_state){
									
					case FTP_DELETE:
						i_Char=0;
						gsmgps_putcommand("AT#FTPCLOSE=?\r");
						ftp_deletefile_state=FTP_PUT1;
						break;
					case FTP_DELETE1:
						if(ExtRecieved && strstr(answer,"OK")){
							ExtRecieved=0;
							ExtSent=0;
							i_Char=0;
							ftp_deletefile_state=FTP_DELETE2a;
							
						}else if(ExtRecieved && !strstr(answer,"OK")){
							ftp_deletefile_state=FTP_DELETE2b;
						}
						break;
					case FTP_DELETE2a:
						sprintf(dummy,"AT#FTPOPEN=\"%s:21\",\"%s\",\"%s\",0\r",INITSTRUCT.FTP_SERVER,INITSTRUCT.FTP_USERNAME,INITSTRUCT.FTP_PW);
						gsmgps_putAndCheck(dummy,"OK",&ftp_deletefile_state);
						if(strstr(answer,"Already connected")){
							ftp_deletefile_state=FTP_DELETE2b;
						}
						break;
					case FTP_DELETE2b:
						TRACE("SET FTP MODE to ASCII");	
						gsmgps_putAndCheck("AT#FTPTYPE=1\r","OK",&ftp_deletefile_state); //ftptype=1=ASCII Mode
						//TRACE("       ANSWER: %s ", answer);
						
						break;
					case FTP_DELETE3:
						sprintf(dummy,"AT#FTPDELE=\"%s\"\r",filename);
						gsmgps_putAndCheck(dummy,"OK",&ftp_deletefile_state);
						TRACE("       ANSWER: %s ", answer);	
						break;
					case FTP_DELETE4:
						if(!timer2000){			//wait 2s till end
							gsmgps_putAndCheck("AT#FTPCLOSE\r","OK",&ftp_deletefile_state);
							//TRACE("       ANSWER: %s ", answer);
						}
						break;
					case FTP_DELETE5:
						TRACE("DATEI ERFOLGREICH Gelöscht");
						TRACE("FTP VERBINDUNG GESCHLOSSEN");
						ftp_deletefile_state=FTP_DELETE;
						gprs_connected_state++;
						break;
		}
	

}


/**
 * \fn 		int gsmgps_init())
 * 
 * \brief  	initialisiert die UART Schnittstellen für das Telit-Modul
 *  
 * \return NO_ERROR    Fehlerfreie Übertragung
 * 
 * 
 **/
int gsmgps_init(){
	gsmgps_state=INIT_GSM;
	init_state=HW_ON;
	idle_state=GPS_FIX;
	gprs_connect_state=CONTEXT_SETTING;
	gprs_connected_state=CONNECTED_IDLE;
	NMEA_Protocol.NrOfSat=0;
	ftp_getfile_state=FTP_GET;
	ftp_putfile_state=FTP_PUT;
	ftp_deletefile_state=FTP_DELETE;
	connections=0;
	AT91C_BASE_PIOA->PIO_PPUDR = AT91C_PIO_PA20; //disable Pullup on TELIT_POWERMON
	AT91C_BASE_PIOA->PIO_PPUDR = AT91C_PIO_PA7;	//disable Pullup on 3.8V ON/OFF


	ExtGsmInt=0;
	// configure IOs: Reset, on-off, powermonitor	
	AT91F_PIO_CfgOutput(AT91C_BASE_PIOA, PIN_TELIT_RESET);
		AT91F_PIO_CfgOutput(AT91C_BASE_PIOA, PIN_TELIT_ON_OFF);
		AT91F_PIO_CfgOutput(AT91C_BASE_PIOA, AT91C_PIO_PA7);
	//setze Output = 0
	AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, PIN_TELIT_RESET | PIN_TELIT_ON_OFF);

	AT91F_PIO_CfgInput(AT91C_BASE_PIOA, PIN_TELIT_PWRMON);
	
	
	return NO_ERROR;
}


/**
 * \fn 		int gsmgps_HW_on())
 * 
 * \brief  	Telit Hardware ON (Achtung dauert > 1 Sekunde)
 *  
 * \return NO_ERROR = Telit ON || ERROR = Telit not ON
 * 
 * 
 **/
int gsmgps_HW_on(){
		
	//setze Output = 1 und Warte 1.5 Sekunden
	//GSM Power ON (3,8 V)
	AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, AT91C_PIO_PA7);
	delay(10);
	if((AT91C_BASE_PIOA->PIO_PDSR) & PIN_TELIT_PWRMON){
		gsmgps_HW_off();
	}
	AT91F_PIO_SetOutput(AT91C_BASE_PIOA, PIN_TELIT_ON_OFF);
	delay(1500);
	AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, PIN_TELIT_ON_OFF);
	//prüft Pin PWRMON
	if((AT91C_BASE_PIOA->PIO_PDSR) & PIN_TELIT_PWRMON){			
			TRACE("HW_ON Telit ON \n\r");
			return NO_ERROR;
		}else{
			TRACE("Telit not ON \n\r");
			return ERROR;
		}
	
}

/**
 * \fn 		int gsmgps_HW_off())
 * 
 * \brief  	Telit Hardware OFF (Achtung dauert > 1 Sekunde)
 *  
 * \return NO_ERROR = Telit ON || ERROR = Telit not ON
 * 
 * 
 **/
int gsmgps_HW_off(){
		
	//setze Output = 1 und Warte 1 Sekunde
	AT91F_PIO_SetOutput(AT91C_BASE_PIOA, PIN_TELIT_ON_OFF);
	delay(1050);
	AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, PIN_TELIT_ON_OFF);
	delay(500);
	AT91F_PIO_SetOutput(AT91C_BASE_PIOA, AT91C_PIO_PA7);
	
	//prüft Pin PWRMON
	if((AT91C_BASE_PIOA->PIO_PDSR) & PIN_TELIT_PWRMON){			
			TRACE("Telit not OFF \n\r");
			return ERROR;
		}else{
			TRACE("Telit OFF \n\r");
			return NO_ERROR;
		}
	
}

/**
 * \fn 		int gsmgps_check())
 * 
 * \brief  	prüft ob Telit ein- oder ausgeschaltet ist.
 *  
 * \return 1 wenn Telit ON; 0 wenn Telit off
 * 
 * 
 **/
int gsmgps_check(){
		
	//prüft Pin PWRMON
	if((AT91C_BASE_PIOA->PIO_PDSR) & PIN_TELIT_PWRMON){			
			//TRACE("CHECK:Telit ON \n\r");
			return 1;
		}else{
			TRACE("Telit OFF \n\r");
			return 0;
		}
	
}

/**
 * \fn 		void gsmgps_putcommand(char chr))
 * 
 * \brief  	Diese Funktion sendet das übergebene Argument über USART0 zum Telit
 *  
 * \param[in]  
 * 
 * 
 *  **/
void gsmgps_putcommand(char * command){
	int i=0;
		
	while(command[i]!='\0'){
			UART_putc(UART0, command[i] );
			i++;
	}
}

/**
 * \fn 		void checkIfEnd())
 * 
 * \brief  	Diese Funktion testet, ob das Ende der Antwort erreicht ist
 *  
 * 
 *  **/
void checkEndOfResponse(){
	char *pChar;		//, *pChar1, *pChar2, *pChar3; //only used for sms 
	
	ExtGsmInt = 0;
	
	// end of response detection for successful-response 
	pChar = strstr(answer, "\r\nOK\r\n");
	if (pChar){ 						
		strcpy((pChar+6),"\0");
		ExtRecieved = 1;
		//gsm_response_edit();  
	}
	// end of response detection for CONNECT-response 
	pChar = strstr(answer, "\r\nCONNECT\r\n");
	if (pChar){ 						
		strcpy((pChar+11),"\0");
		ExtRecieved = 1;
		//gsm_response_edit();  
	}
	// end of response detection for NO CARRIER-response 
	pChar = strstr(answer, "\r\nNO CARRIER\r\n");
	if (pChar){ 	
		strcpy((pChar+14),"\0");
		ExtRecieved = 1;
		//gsm_response_edit();
	}
	// end of response detection for ERROR-response
	pChar = strstr(answer, "ERROR"); 
	if (pChar){
		pChar = strstr(pChar, "\n");
		if (pChar){
			strcpy(pChar,"\0");
			ExtRecieved = 1;
			//gsm_response_edit();
		}  
	}
//	// end of response detection for config.ini-response	
//	pChar = strstr(answer, "[END]"); 
//	if (pChar){
//		strcpy(strAnswer,answer);
//		pChar = strstr(strAnswer, "[END]");
//		strcpy(pChar+5,"\0");
//		i_Char = 0;
//		ExtRecieved = 1;
//	}
//	// "end of response" detection for sms
//	// ex. [\r\n\r\n+CMT: "+41XXXXXXXXX",,"07/10/05,13:47:44+8"\r\nSMS_TEXT\r\n]
//	// +CMT: indicates incomming sms, "+41XXXXXXXXX" sender number
//	// 07/10/05,13:47:44+8 yy/mm/dd,hh:mm:ss+?
//	pChar1 = strstr(answer, "send data");
//	pChar2 = strstr(answer, "get config");
//	pChar3 = strstr(answer, "get status");
//	pChar = 0;
//	if (pChar1){
//		pChar = pChar1;
//	}
//	if (pChar2){
//		pChar = pChar2;
//	}
//	if (pChar3){
//		pChar = pChar3;
//	}
//	if (pChar){ 				
//		pChar = strstr(pChar, "\0");  
//		if (pChar){ 							//"end of response" detected 				 
//			strcpy(strAnswer,answer);
//			pChar1 = strstr(strAnswer,"\""); // get sender number
//			if (pChar1){
//				pChar2 = strstr(pChar1+1,"\"");
//				*(pChar2) = '\0';
//				strcpy(receivedSMS.senderNo,pChar1+1); 
//			}
//			strcpy(strAnswer,answer);		// ok
//			pChar1 = strstr(strAnswer,"/"); // get date/time
//			if (pChar1){
//				receivedSMS.timeStamp.jahre = (((int)*(pChar1-2)-0x30)*10 + ((int)*(pChar1-1)-0x30));
//				receivedSMS.timeStamp.monate = (((int)*(pChar1+1)-0x30)*10 + ((int)*(pChar1+2)-0x30));
//				receivedSMS.timeStamp.tage = (((int)*(pChar1+4)-0x30)*10 + ((int)*(pChar1+5)-0x30));
//				receivedSMS.timeStamp.stunden = (((int)*(pChar1+7)-0x30)*10 + ((int)*(pChar1+8)-0x30));
//				receivedSMS.timeStamp.minuten = (((int)*(pChar1+10)-0x30)*10 + ((int)*(pChar1+11)-0x30));
//				receivedSMS.timeStamp.sekunden = (((int)*(pChar1+13)-0x30)*10 + ((int)*(pChar1+14)-0x30));
//				rtc_calc_timestring(&receivedSMS.timeStamp);
//			}
//			pChar = strstr(answer, "send data");
//			if (pChar){
//				strcpy(receivedSMS.text,"send data");
//				receivedSMS.valid = 1;
//			}
//			pChar = strstr(answer, "get config");
//			if (pChar){
//				strcpy(receivedSMS.text,"get config");
//				receivedSMS.valid = 1;
//			}
//			pChar = strstr(answer, "get status");
//			if (pChar){
//				strcpy(receivedSMS.text,"get status");
//				receivedSMS.valid = 1;
//			}
//			strcpy(strAnswer,answer);
//			ExtRecieved = 1;
//			i_Char = 0;  
//		}
//	}
//	// end of response detection for start send sms text
//	if ((strstr(answer, "AT+CMGS=") != 0) & (strstr(answer, ">") != 0)){
//		strcpy(strAnswer, ">"); 
//		ExtRecieved = 1;
//		i_Char = 0;
//	}
}




/**
 * \fn 		void gsmgps_putAndCheck(char* command, char answer[20], int* state))
 * 
 * \brief  	Diese Funktion sendet das übergebene Argument über USART0 zum Telit und Testet die Erhaltene
 * Antwort auf Richtigkeit
 *  
 * \param[in] zu sendender Befehl, Erhoffte Antwort, Pointer auf Momentanen State 
 * 
 * 
 *  **/
void gsmgps_putAndCheck(char* command, char strAnswer[60], int* state){
	
	static int try;
	static int maxNoAnswer=10;
	
	int i=0;
	static int trymax;
	char* ptr;
	char answercpy[USART_ANSWER_LEN];
	
	if (!ExtSent){//if(!ExtSent)
		if (gsmgps_check()){ // if module is on -> send command
			strcpy(strCommand,command); 
			gsmgps_putcommand(strCommand);
			if(strstr(strAnswer,"CONNECT")||strstr(strAnswer,"NO CARRIER")){
				atTimeout=6000;	//atTimeout für FTP 6s
			}else if (strstr(strCommand,"AT#FTPCLOSE")||strstr(strCommand,"AT#FTPOPEN")){
				atTimeout=10000;	//atTimeout ist standartmässig 10s
			}else{
				atTimeout=1000;	//atTimeout ist standartmässig 1s
			}
			//TRACE("%s",strCommand);
			ExtSent = 1;
			//try=0;
		}
		else { // Stopp if module is off
			sprintf(error,"TELIT_OFF");
			gsmgps_state = GSM_ERROR_STATE;
	
			try = 0;
		}
	}
	if (ExtRecieved == 1){
		ExtRecieved = 0;
		i_Char=0;
		
		
		if (!strstr(answer,strAnswer)){
			TRACE("Wrong Answer from Module\n\r %s\n\r",answer);
			ExtSent = 0;	// set variables to send request again
			try++; 	// stop sending equal request if answer failed 40 times
			
			//if (strstr(strAnswer,"#GPRS=1")){
				//trymax = 1000;
			//}else {
				trymax = 30;
			//}
			if (try > trymax){
				//sprintf(error,"WRONG_ANSWER");
				sprintf(error,"WrongAnswerMax");
				gsmgps_state = GSM_ERROR_STATE;
				try = 0;
			}
		}
		else{			// string in Answer is in strAnswer
			TRACE("Answer OK\n\r");
			TRACE("%s\n\r",answer);
			strcpy(answercpy,answer);
			if(strstr(answercpy,"$GPSACP:")){	//if Answer is in MNEA Protocol
				strcpy(NMEA_Protocol.nmea_sentence,answercpy);
				ptr = strtok(answercpy, " ,");
				 while(ptr != NULL) {
				     i++; 
					 
				     ptr = strtok(NULL, ",");
				     if(i==1){		//1. Element ist die Zeit hhmmss.ms
				    	 strcpy(NMEA_Protocol.time, ptr);
				    	 TRACE("Zeit: %s \n\r",NMEA_Protocol.time);
				     }else if(i==4){	//precision
				    	 NMEA_Protocol.precision=selfAtof(ptr);
				    	 TRACE("Präzision: %f \n\r",NMEA_Protocol.precision);
				     }else if(i==6){	//fix
				    	 NMEA_Protocol.fix=selfAtoi(ptr);
				    	 TRACE("Fix: %d \n\r",NMEA_Protocol.fix);
				     }else if(i==10){	//date
				    	 strcpy(NMEA_Protocol.date, ptr);
				    	 TRACE("Datum: %s \n\r",NMEA_Protocol.date);		   
				     }else if(i==11){	//NrOfSat
				    	 NMEA_Protocol.NrOfSat=selfAtoi(ptr);
				    	 TRACE("Anzahl Satelliten: %d \n\r\n\r",NMEA_Protocol.NrOfSat);
				     }
				    
				      
				      
				   }
				 datestringTOint(NMEA_Protocol.date);
				 timestringTOint(NMEA_Protocol.time);
				 TRACE("%d Uhr %d Minuten %d Sekunden %d Tausendstel\n\r",NMEA_Protocol.hour,NMEA_Protocol.minute,NMEA_Protocol.second,NMEA_Protocol.milisecond);

			}
			*state = *state+1; // go over to next state
			ExtSent = 0;
			try = 0;

		}
	}else if(atTimeout==0){		//no Answer recieved on request
		NoAnswercount++;
		
		TRACE("No Answer from Module\n\r %s\n\r",answer);
		ExtSent = 0;	// set variables to send request again
		if (NoAnswercount > maxNoAnswer){
			sprintf(error,"NoAnswerMax");
			gsmgps_state = GSM_ERROR_STATE;
			NoAnswercount = 0;
		}
	}
}

/**
 * \fn 		int SummerOrWinterTime())
 * 
 * \brief  	Diese Funktion prüft mit den aus dem config.ini file erhaltenen Daten und dem heutigen Datum, ob 
 * 			Sommer- oder Winterzeit herscht und setzt die variable summertime (1 wenn Sommerzeit)
 * 
 * 
 **/
int SummerOrWinterTime(){
	if(NMEA_Protocol.month==3){	//falls März und schon der SOmmerzeittag durch
		if(NMEA_Protocol.day >= INITSTRUCT.SUMMERTIME_DAY){
			summertime=1;
		}
	}else if(NMEA_Protocol.month==10){//falls Oktober und noch nicht der Winterzeittag durch
			if(NMEA_Protocol.day < INITSTRUCT.WINTERTIME_DAY){
			summertime=1;
		}
	}else if(NMEA_Protocol.month > 3 && NMEA_Protocol.month < 11){	//Falls nach März und vor Oktober
			if(NMEA_Protocol.day >= INITSTRUCT.SUMMERTIME_DAY){
			summertime=1;
		}
	}else{					//sonst Winterzeit!
		summertime=0;
	}
	
	return NO_ERROR;
}

/**
 * \fn 		void datestringTOint(char* a))
 * 
 * \brief  	Diese Funktion konvertiert den Datumsstring aus dem NMEA Protokol von GPS zu Integern hh mm ss ms
 *  
 * \param[in]  a	pointer to string
 * 
 * \param[out] 4 x integer
 * 
 * 
 **/
void datestringTOint(char* in){
	int dummy=0;
	//Day
	dummy=(in[0]-48);		
	dummy*=10;
	dummy+=(in[1]-48);
	NMEA_Protocol.day=dummy;			
	dummy=0;
	//Month
	dummy=(in[2]-48);
	dummy*=10;
	dummy+=(in[3]-48);
	NMEA_Protocol.month=dummy;
	dummy=0;
	//Year
	dummy=(in[4]-48);
	dummy*=10;
	dummy+=(in[5]-48);
	NMEA_Protocol.year=dummy;
	dummy=0;
	
}

/**
 * \fn 		void timestringTOint(char* a))
 * 
 * \brief  	Diese Funktion konvertiert den Zeitstring aus dem NMEA Protokol von GPS zu Integern hh mm ss ms
 *  
 * \param[in]  a	pointer to string
 * 
 * \param[out] 4 x integer
 * 
 * 
 **/
void timestringTOint(char* in){
	int dummy=0;
	//Hour
	dummy=(in[0]-48);		
	dummy*=10;
	dummy+=(in[1]-48);
	SummerOrWinterTime();
	if(summertime){
	NMEA_Protocol.hour=(dummy+2)%24;//ergibt MEZ (Sommer) aus UTC +2 Stunden Modulo 24
	}else{
		NMEA_Protocol.hour=(dummy+2)%24;//ergibt MEZ (Winter) aus UTC 1 Stunden Modulo 24	
	}
	dummy=0;
	//Minute
	dummy=(in[2]-48);
	dummy*=10;
	dummy+=(in[3]-48);
	NMEA_Protocol.minute=dummy;
	dummy=0;
	//Second
	dummy=(in[4]-48);
	dummy*=10;
	dummy+=(in[5]-48);
	NMEA_Protocol.second=dummy;
	dummy=0;
	//Milisecond
	dummy=(in[7]-48);
	dummy*=10;
	dummy+=(in[8]-48);
	dummy*=10;
	dummy+=(in[9]-48);
	NMEA_Protocol.milisecond=dummy;
}

/**
 * \fn 		int selfAtoi(char* a))
 * 
 * \brief  	Diese Funktion konvertiert einen String zu Integer
 *  
 * \param[in]  a	pointer to string
 * 
 * \param[out] integer
 * 
 * 
 **/
int selfAtoi(char* in){
	int i=0;
	int out=0;
	while(in[i]!='\0' && i<9){
		if('0'<=in[i] && in[i]<='9'){
		out*=10;
		out+=(in[i]-48);		
		
		}
		i++;
	}
	return out;
}



/**
 * \fn 		double selAtof(char* a))
 * 
 * \brief  	Diese Funktion konvertiert einen String zu einer double Zahl
 *   
 * \param[in]  a	pointer to string
 * 
 * \param[out] double
 * 
 **/
double selfAtof(char* in){
	int i=0;
	double j=1;
	double out=0;
	while(in[i]!='.' && i<9 && in[i]!='\0'){	//die zehnerpotenzen
		if('0'<=in[i] && in[i]<='9'){
			out*=10;
			out+=(in[i]-48);		
		}
		i++;
	}
	if(in[i]=='.'){
		i++;
	}
	while(i<9 && in[i]!='\0'){					//Komastellen
		if('0'<=in[i] && in[i]<='9'){
			j=j*10;
			out+=((in[i]-48)/j);
			i++;
		}
	}
	
	return out;
}

///**
// * \fn 		void gsmgps_to_debug(char chr))
// * 
// * \brief  	Diese Funktion sendet eine vom Telit erhaltene Antwort auf die Debuggschnittstelle
// *  
// * \param[in]  chr   1 Buchstabe der Antwort
// * 
// * 
// **/
//void gsmgps_to_debug(char chr){
//	static int i,j;
//	static char answergsm[50];
//	
//	
//	if(chr != '\r' && i<48 && chr !='\0'){
//		answergsm[i]=chr;
//		i++;
//		
//	}else if(chr=='\r'){
//		
//		answergsm[i]='\r';
//		//TRACE("\n\r\n\r Debug ");
//		//if(strstr(answergsm, "OK")){
//			for(j=0;j<i;j++){
//				UART_putc(UART_DBG, answergsm[j]);
//			}//end for
//		//}//end if
//		memset(answergsm,0,50);
//		i=0;
//		j=0;
//		
//	}else if(i==49){
//		//TRACE("\n\r GSM ANTWORT zu gross \n\r");
//		for(j=0;j<i;j++){
//			UART_putc(UART_DBG, answergsm[j]);
//		}//end for
//		memset(answergsm,0,50);	
//		i=0;
//		j=0;
//	}//end if...else
//}
//
///**
// * \fn 		void gsmgps_recieve_answer(char chr))
// * 
// * \brief  	Diese Funktion speichert eine vom GSM Modul erhaltene Antwort in einem String
// *  
// * \param[in]  chr   1 Buchstabe der Antwort
// * 
// * 
// **/
//void gsmgps_recieve_answer(char chr){
//	static int i;
//	static char answer[100];
//	
//	if(chr != '\r' && i<98){
//		answer[i]=chr;
//		TRACE("Teilantwort: %s",answer);
//		i++;
//		
//	}else if(chr=='\r'){
//		answer[i]='\r';	
//		TRACE("\n\r recieved answer: %s\n\r",answer);
//		if(strstr(answer, "\r\nOK\r\n")){
//			ExtRecieved=1;
//			memset(answer,0,100);
//			i=0;
//		}//end if
//
//	}else if(i==98){
//		TRACE("Antwort zu gross: %s",answer);
//		if(strstr(answer, "\r\nOK")){
//			ExtRecieved=1;
//		}
//		memset(answer,0,100);	
//		i=0;
//	}//end if...else
//}
//
//
///**
// * \fn 		void gps_to_debug(char chr))
// * 
// * \brief  	Diese Funktion sendet das vom Telit erhaltene NMEA Protokoll auf die Debuggschnittstelle
// *  
// * \param[in]  chr   1 Buchstabe der Antwort
// * 
// * 
// **/
//void gps_to_debug(char chr){
//	static int i,j;
//	static char answergps[100];
//	
//	if(chr != '\r' && i<98){
//		answergps[i+1]=chr;
//		i++;
//		
//	}else if(chr=='\r'){
//		answergps[i+2]='\r';
//		TRACE("\n\r GPS NMEA auf RXD1 \n\r");
//		for(j=0;j<i;j++){
//			UART_putc(UART_DBG, answergps[j+1]);
//		}//end for
//		memset(answergps,0,100);
//		i=0;
//		j=0;
//	}else{
//		TRACE("\n\r GPS NMEA zu gross \n\r");
//		for(j=0;j<i;j++){
//			
//			UART_putc(UART_DBG, answergps[j+1]);
//		}//end for
//		memset(answergps,0,100);	
//		i=0;
//		j=0;
//	}//end if...else
//}
