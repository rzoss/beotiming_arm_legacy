/**
 *******************************************************************************
 * \file    global.h
 *******************************************************************************
 * \brief    Enthält globale Definitionen und kann in allen Headerdateien included werden
 *
 *
 * \version		1.0
 * \date		26.04.2008
 * \author		R.Zoss
 *
 *******************************************************************************
 */

#ifndef _GLOBAL
#define _GLOBAL

#define __packed	__attribute((packed,aligned(1)))

/**
 * \brief	Integer (32 bit, plattform dependend)
 */
typedef int				INT;
/**
 * \brief	unsigned Integer (32 bit, plattform dependend)
 */
typedef unsigned int	UINT;

/**
 * \brief	Character (8 bit)
 */
typedef char			CHAR;
/**
 * \brief	unsigned Character (8 bit)
 */
typedef unsigned char	UCHAR;
/**
 * \brief	unsigned Character (8 bit)
 */
typedef unsigned char	BYTE;

/**
 * \brief	Word (16 bit)
 */
typedef short			SHORT;
/**
 * \brief	unsigned Word (16 bit)
 */
typedef unsigned short	USHORT;
/**
 * \brief	unsigned Word (16 bit)
 */
typedef unsigned short	WORD;

/**
 * \brief	Long (32 bit)
 */
typedef long			LONG;
/**
 * \brief	unsigned Long (32 bit)
 */
typedef unsigned long	ULONG;
/**
 * \brief	unsigned Long (32 bit)
 */
typedef unsigned long	DWORD;

/**
 * \brief	Boolean
 */
typedef unsigned char	BOOL;
/**
 * \brief	FALSE
 */
#define FALSE	0
/**
 * \brief	TRUE
 */
#define TRUE	1

/**
 * \brief	Allgemeine Definition für Fehlerfrei
 */
#define NO_ERROR	0

/**
 * \brief	Allgemeine Definition für Fehler -> Aktion wiederholen
 */
#define ERROR	1

/**
 * \brief	Masterclock des Prozessors
 */
#define 	MCK 	48054857			// Masterclock

/**
 * \brief	END_OF_FILE
 */
#define END_OF_FILE 5


/**
 * \brief  	STRUCT type for INIT_STRUCT (das init file wird von der SD Karte ausgelesen und die Daten in die Struktur gefüllt)
 **/
typedef struct{
	int STARTorFINISH;						//Variable bestimmt ob Start(0) oder Ziel(1) 
	int SUMMERTIME_DAY;						//Tag im März wo der Zeitwechsel stattfindet
	int WINTERTIME_DAY;						//Tag im Oktober wo der Zeitwechsel stattfindet
	char SIM_PIN[5];						//String mit PIN für SIM Karte (4 Zeichen)
	char SIM_PUK[9];						//String mit PUK für SIM Karte (8 Zeichen)
	char SMS_NR[11];						//Empfängernr. für Status- und Warnungs-SMS
	char SMS_CENTER[13];					//NR. der SMS Zentrale
	char WAPAPN[30];						//Adresse des GPRS-Servers
	char SERVER_IP[16];						//IP Adresse des GPRS-Servers
	char SERVER_PORT[6];					//Verwendeter Port des GPRS-Servers
	char GPRS_USERNAME[15];					//GPRS Benutzername			
	char GPRS_PW[15];						//GPRS Passwort	
	char FTP_SERVER[40];					//Adresse des ftp Servers (um die Daten raufzuladen)
	char FTP_USERNAME[25];					//Benutzername für ftp Server
	char FTP_PW[20];						//Passwort für ftp Server
	int GOODNIGHT_TIME;						//Wann soll die Anlage in sleep mode gehen? Format [hh]
	int WAKEUP_TIME;						//Wann soll die Anlage aufwachen? Format [hh]
	int USE_GPRS;							//sollen die daten per GPRS übertragen, oder nur auf SD-Karte gespeichert werden.
	short STRECKENKEY_1;						//Mehrere Strecken z.B. Laufen Velo und MTB möglich
	char STRECKENSTRING_1[17];
	short STRECKENKEY_2;
	char STRECKENSTRING_2[17];
	short STRECKENKEY_3;
	char STRECKENSTRING_3[17];
}INITS;

INITS INITSTRUCT;


#endif
