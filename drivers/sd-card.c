/**
 *******************************************************************************
 * \file    sd-card.c
 *******************************************************************************
 * \brief    SD Kartentreiber
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
#include "rtc.h"
#include "string.h"
#include "stdio.h"
#include "sd-card.h"
#include "../filesystem/diskio.h"
#include "../filesystem/tff.h"


/**
 * \brief  	Global Vars
 **/
/**
 * \brief  	Work area (file system object) for logical drive
 **/
FATFS fs;            // Work area (file system object) for logical drive


/**
 * \fn 		int sd_init()
 * 
 * \brief  	initialize sd-card
 * \return 	NO_ERROR
 **/
int sd_init(){
		
	// Register a work area for logical drive 0
	f_mount(0, &fs);

	return NO_ERROR;
}


/**
 * \fn 		int sd_unmount()
 * 
 * \brief  	unmount sd-card
 * 
 * \return 	NO_ERROR
 **/
int sd_unmount(){
		
	// Unregister a work area before discard it
	f_mount(0, NULL);
	//now safe to remove SD - Card
	TRACE("NOW IT IS SAFE TO REMOVE SD CARD");
	return NO_ERROR;
}



/**
 * \fn 		int sd_timefile(char * tim)
 * 
 * \brief  	prints to timefile
 * 
 * \param[in] char * tim
 * \return 	NO_ERROR
 **/
int sd_timefile(char * tim){

	FIL ftim; 		 					// file object
	UINT bw;       						// File R/W count
	FRESULT res;         				// FatFs function common result code
	char buffer2[3]="\r\n";

	
	// Open source file
	res = f_open(&ftim, "time.txt", FA_OPEN_ALWAYS | FA_WRITE);
	// Move to end of the file to append data
    res = f_lseek(&ftim, ftim.fsize);
	//write string to file
	res = f_write(&ftim, tim ,strlen(tim), &bw);
	//\r\n
	res = f_write(&ftim, buffer2, 2, &bw);
	
	f_close(&ftim);
	return NO_ERROR;
}




/**
 * \fn 		int sd_logfile(char * log)
 * 
 * \brief  	prints to logfile
 * 
 * \param[in] int rowNr, char * log
 * \return 	NO_ERROR
 **/
int sd_logfile(char * log){
	TIME datum;
	char buffer[22];
	FIL flog; 		 					// file object
	UINT bw;       						// File R/W count
	FRESULT res;         				// FatFs function common result code
	char buffer2[3]="\r\n";

	
	// Open source file
	res = f_open(&flog, "logfile.log", FA_OPEN_ALWAYS | FA_WRITE);
	// Move to end of the file to append data
    res = f_lseek(&flog, flog.fsize);
    //get time
	rtc_get_time(&datum);
	//write time
	sprintf(buffer,"<%s>",datum.string);
	res = f_write(&flog, buffer,strlen(buffer), &bw);
	//write string to file
	res = f_write(&flog, log ,strlen(log), &bw);
	//\r\n
	res = f_write(&flog, buffer2, 2, &bw);
	
	f_close(&flog);
	return NO_ERROR;
}

/**
 * \fn 		int sd_dbg(char * dbg)
 * 
 * \brief  	prints to debug file
 * 
 * \param[in] char * dbg
 * \return 	NO_ERROR
 **/
int sd_dbg(char * dbg){

	TIME datum;
	char buffer[23];
	FIL fdbg; 		 					// file object
	UINT bw;       						// File R/W count
	FRESULT res;         				// FatFs function common result code
	char buffer2[3]="\r\n";

	
	// Open source file
	res = f_open(&fdbg, "debug.log", FA_OPEN_ALWAYS | FA_WRITE);
	// Move to end of the file to append data
    res = f_lseek(&fdbg, fdbg.fsize);
    //get time
	rtc_get_time(&datum);
	sprintf(buffer,"<%s> ",datum.string);
	//write time
	res = f_write(&fdbg, buffer,22, &bw);
	//write string to file
	res = f_write(&fdbg, dbg ,strlen(dbg), &bw);
	//\r\n
	res = f_write(&fdbg, buffer2, 2, &bw);

	f_close(&fdbg);
	return NO_ERROR;
}


/**
 * \fn 		int sd_read_config()
 * 
 * \brief  	read config file
 *  \return 	NO_ERROR or ERROR
 **/
int sd_read_config(){
	int i=0;
	int j=0;
	FIL fconfig; 		 // file object
	char buffer;  	 	 // file copy buffer
	UINT br;        	 // File R/W count
	FRESULT res;         // FatFs function common result code
	char dummy[41];		 //string to copy in
	
	// Open source file
	res = f_open(&fconfig, "config.ini", FA_OPEN_EXISTING | FA_READ);
	if(res){ 
		TRACE("Fehler bei f_open -> config.ini\r\n");
		FTRACE("Fehler bei f_open -> config.ini\r\n");
		return ERROR;
	}else{
		while(!res && br!=0){									//while !EOF
			i++;										//zeilencounter
			while(!res && buffer!='=' && br!=0)
				res = f_read(&fconfig,&buffer, 1, &br);
			
			while(!res && br!=0){			//lesen.	
				res = f_read(&fconfig,&buffer, 1, &br);
				if(buffer == '\r'){
					break;
				}
				dummy[j++]=buffer;
				dummy[j]='\0';
			}
			//jede zeile in ein anderes Element des INITSTRUCT reinschreiben.
			switch(i){
				case STARTorFINISH:		INITSTRUCT.STARTorFINISH=selfAtoi(dummy); 	break;
				case SUMMERTIME_DAY: 	INITSTRUCT.SUMMERTIME_DAY=selfAtoi(dummy); 	break;	
				case WINTERTIME_DAY: 	INITSTRUCT.WINTERTIME_DAY=selfAtoi(dummy); 	break;
				case SIM_PIN:			strcpy(INITSTRUCT.SIM_PIN, dummy);			break;
				case SIM_PUK:			strcpy(INITSTRUCT.SIM_PUK, dummy);			break;
				case SMS_NR:			strcpy(INITSTRUCT.SMS_NR, dummy);			break;
				case SMS_CENTER:		strcpy(INITSTRUCT.SMS_CENTER, dummy);		break;
				case WAPAPN:			strcpy(INITSTRUCT.WAPAPN, dummy);			break;
				case SERVER_IP:			strcpy(INITSTRUCT.SERVER_IP, dummy);		break;
				case SERVER_PORT:		strcpy(INITSTRUCT.SERVER_PORT, dummy);		break;
				case GPRS_USERNAME:		strcpy(INITSTRUCT.GPRS_USERNAME, dummy);	break;
				case GPRS_PW:			strcpy(INITSTRUCT.GPRS_PW, dummy);			break;
				case FTP_SERVER:		strcpy(INITSTRUCT.FTP_SERVER, dummy);		break;
				case FTP_USERNAME:		strcpy(INITSTRUCT.FTP_USERNAME, dummy);		break;
				case FTP_PW:			strcpy(INITSTRUCT.FTP_PW, dummy);			break;
				case GOODNIGHT_TIME:	INITSTRUCT.GOODNIGHT_TIME=selfAtoi(dummy); 	break;	
				case WAKEUP_TIME:		INITSTRUCT.WAKEUP_TIME=selfAtoi(dummy); 	break;
				case USE_GPRS: 			INITSTRUCT.USE_GPRS=selfAtoi(dummy);		break;
				case STRECKENKEY_1: 	INITSTRUCT.STRECKENKEY_1=selfAtoi(dummy);	break;
				case STRECKENSTRING_1: 	strcpy(INITSTRUCT.STRECKENSTRING_1, dummy);	break;
				case STRECKENKEY_2: 	INITSTRUCT.STRECKENKEY_2=selfAtoi(dummy);	break;
				case STRECKENSTRING_2: 	strcpy(INITSTRUCT.STRECKENSTRING_2, dummy);	break;
				case STRECKENKEY_3: 	INITSTRUCT.STRECKENKEY_3=selfAtoi(dummy);	break;
				case STRECKENSTRING_3: 	strcpy(INITSTRUCT.STRECKENSTRING_3, dummy);	break;
			}
			j=0;
		}
		f_close(&fconfig);
		return NO_ERROR;
	}
}

/**
 * \fn 		int getFileRow()
 * 
 * \brief  	gets Row from file time.txt
 * 
 * \param[in] int rowNr, char * row
 *  \return 	NO_ERROR, END_OF_FILE or ERROR
 **/
int getFileRow(int rowNr, char * row){
	int i=0;
	int j=0;
	FIL ftim; 		 // file object
	char buffer;  	 	 // file copy buffer
	UINT br;        	 // File R/W count
	FRESULT res;         // FatFs function common result code
	//char dummy[50];		 //string to copy in
	
	// Open source file
	res = f_open(&ftim, "time.txt", FA_OPEN_EXISTING | FA_READ);
	if (res) {
		TRACE("Fehler bei f_open -> time.txt\r\n");
		FTRACE("Fehler bei f_open -> time.txt\r\n");
		return ERROR;
	} else {
		res = f_read(&ftim, &buffer, 1, &br);
		while (!res && br!=0) { //while !EOF
			i++; //zeilencounter
			
			while (!res && br!=0) { //Zeile lesen.	
				
				if (buffer == '\n') {
					row[j++]='\n';
					row[j]='\0';
					break;
				}

				row[j++]=buffer;
				row[j]='\0';
				res = f_read(&ftim, &buffer, 1, &br);
			}
			if (i==rowNr) {
				f_close(&ftim);
				return NO_ERROR;
				break;
			}
			j=0;
			res = f_read(&ftim, &buffer, 1, &br);
		}
		if (br==0) { //if EOF
			f_close(&ftim);
			return END_OF_FILE;
		}else{
			f_close(&ftim);
			return ERROR;
		}
		
	}
}

