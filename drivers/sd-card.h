/**
 *******************************************************************************
 * \file    sd-card.h
 *******************************************************************************
 * \brief   SD Kartentreiber
 * 			
 *
 * \version	1.0
 * \date	05.05.2008
 * \author  M. Leiser
 *
 *******************************************************************************
 */


#ifndef SDCARD_H_
#define SDCARD_H_

#include <AT91SAM7S256.h>
#include <lib_AT91SAM7S256.h>

/**
 * \brief Enumeration für INIT Struktur (config.ini) 
 **/
enum{DUMMY,STARTorFINISH, WINTERTIME_DAY, SUMMERTIME_DAY, SIM_PIN, SIM_PUK, SMS_NR, SMS_CENTER, WAPAPN, SERVER_IP, SERVER_PORT,
	GPRS_USERNAME, GPRS_PW, FTP_SERVER, FTP_USERNAME, FTP_PW, GOODNIGHT_TIME, WAKEUP_TIME, USE_GPRS, STRECKENKEY_1, STRECKENSTRING_1,STRECKENKEY_2, STRECKENSTRING_2,STRECKENKEY_3, STRECKENSTRING_3};

/*
*********************************************************************************************************
*                                              DEBUGGING
**********************************************************************************************************
*/
#ifdef FILEDEBUG
	/** \brief	FILE- Debug Funktionen werden mitkompiliert*/
	#define FTRACE(...) sd_dbg(__VA_ARGS__)
#else
	/** \brief	FILE- Debug Funktionen werden nicht mitkompiliert*/
	#define FTRACE(...)
#endif


/**
 * \brief	SD-Karte: Pin für WriteProtect Detektion ist PA16
 */
#define MMC_WRITEPROTECT	AT91C_PIO_PA16	
/**
 * \brief	SD-Karte: Pin für Kartendetektion ist PA29
 */
#define MMC_CARDDETECT		AT91C_PIO_PA29

/**
 *	\brief	   Check if the SD-Card is write protected 
 * 
 *	\return    TRUE/FALSE
 ******************************************************************************
 */
static inline int isWriteProtect(){
	return ((AT91C_BASE_PIOA->PIO_PDSR) & MMC_WRITEPROTECT) != 0;	
}

/**
 *	\brief	   Check if a SD-Card is inserted 
 * 
 *	\return    TRUE/FALSE
 ******************************************************************************
 */
static inline int isCardDetect(){
	return ((AT91C_BASE_PIOA->PIO_PDSR) & MMC_CARDDETECT) == 0;	
}

	


int sd_init();
int sd_unmount();
int sd_read_config();
int sd_logfile(char * log);
int sd_timefile(char * tim);
int sd_dbg(char * dbg);
int getFileRow(int rowNr, char * dummy);

#endif /*SDCARD_H_*/
