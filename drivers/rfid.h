/**
 *******************************************************************************
 * \file    rfid.h
 *******************************************************************************
 * \brief    RFID driver for I2C-RFID-Reader/Writer.
 *  
 * \version		1.0
 * \date		20.03.2008
 * \author		R.Zoss
 *
 *******************************************************************************
 */

#ifndef RFID_H_
#define RFID_H_

/**
 * \brief  Tag im Feld
 */
#define TAG_PRESENT     1
/**
 * \brief	Kein Tag im Feld
 */
#define NO_TAG_PRESENT  0
/**
 * \brief Adresse des RFID-Moduls 
 */
#define SL030ADR 		0x50

/**
 * \brief Diese Struktur beinhaltet alle wichtigen Informationen einer Karte 
 */
typedef struct _TAG{
	/**\brief	Seriennummer der aktuellen Karte*/
	unsigned char serial_number[7];
	/**\brief	Seriennummer der aktuellen Karte*/
	char serial_number_ascii[15];
	/**\brief	Typ der aktuellen Karte*/
	unsigned char type;
	/**\brief	Status der aktuellen Karte*/
	unsigned char tag_status;
} TAG;

/**
 * \brief Enumeration für verschiedene Operationen für die RFID_Operation_UL-Funktion
 */
enum RFID_OPERATION_MODE {
	WRITE_STARTTIME,
	READ_STARTTIME,
	WRITE_ENDTIME,
	READ_ENDTIME,
	WRITE_RACETIME,
	READ_RACETIME,
	WRITE_STRECKENKEY,
	READ_STRECKENKEY
};


/**
 * \brief Enumeration für die verschiedenen Zustände der RFID Zustandsmaschine 
 */
int rfid_state;
enum RFID_STATE{
	RFID_IDLE,
	RFID_READ_CARD,
	RFID_CHOOSE_RACE,
	RFID_WRITE_STARTTIME,
	RFID_WRITE_ENDTIME,
	RFID_SHOW_RACETIME,
	RFID_DELETE_RACETIME,
	RFID_SLEEP,
	RFID_BEEP,
	RFID_ERROR
};
/**
 * \brief Enumeration für den Zustand CHOOSE_RACE der RFID Zustandsmaschine 
 */
int chooseRace;
enum CHOOSE_RACE{
	IN,
	KEY1,
	KEY2,
	KEY3
};


/**
 * \brief	RFID-Datumsstruktur (Grösse: 7 Byte)
 */ 
typedef  struct _RFIDDATE{
	/**\brief	Jahr*/
	short year;
	/**\brief	Monat*/
	unsigned char month;
	/**\brief	Tag*/
	unsigned char day;
	/**\brief	Stunde*/
	unsigned char hour;
	/**\brief	Minute*/
	unsigned char minute;
	/**\brief	Sekunde*/
	unsigned char second;
} __packed RFIDDATE;

/**
 * \brief	RFID-Zeitstruktur (Grösse: 4 Byte)
 */ 
typedef struct _RFIDTIME{
	/**\brief	Tag*/
	unsigned char day;
	/**\brief	Stunde*/
	unsigned char hour;
	/**\brief	Minute*/
	unsigned char minute;
	/**\brief	Sekunde*/
	unsigned char second;
} RFIDTIME;

// Tag-Status Masks
/**\brief	Gültige Streckennr. aud der Karte*/
#define TAG_STATUS_STRECKENVALID 	(1<<0)
/**\brief	Gültige Startzeit auf der Karte*/
#define TAG_STATUS_STARTVALID		(1<<1)
/**\brief	Gültige Endzeit auf der Karte*/
#define TAG_STATUS_ENDVALID			(1<<2)
/**\brief	Karte ist Manuel gelöscht worden*/
#define TAG_STATUS_MANUALCLEARED	(1<<3)
/**\brief	Karte ist Persönlich*/
#define TAG_STATUS_REGISTERED		(1<<4)

// Adressen in der RFID-Karte
/**\brief	Pagenummer des Status*/
#define RFID_ADR_STATUS			4
/**\brief	Pagenummer der Streckennummer*/
#define RFID_ADR_STRECKENKEY	5
/**\brief	Pagenummer der Startzeit*/
#define RFID_ADR_STARTTIME		6
/**\brief	Pagenummer der Endzeit*/
#define RFID_ADR_ENDTIME		8
/**\brief	Pagenummer der Fahrzeit*/
#define RFID_ADR_RACETIME		10

// Error
/**\brief	Kein Tag im Feld*/
#define ERROR_RFID_NOTAG					1
/**\brief	Mehrere Karten im Feld*/
#define ERROR_RFID_COLLISION				2
/**\brief	Lesefehler*/
#define ERROR_RFID_READ_FAILED				3
/**\brief	Schreibfehler*/
#define ERROR_RFID_WRITE_FAILED				4
/**\brief	Loginfehler*/
#define ERROR_RFID_LOGIN_FAIL				5
/**\brief	Authentifizierungsfehler*/
#define ERROR_RFID_NOT_AUTHENTICATE			6

// IO pins
/**\brief	RFID Tag Detection Pin auf PA27*/
#define PIN_RFID_IN			AT91C_PIO_PA27
/**\brief	RFID Wake Up Pin auf PA15*/
#define PIN_RFID_OUT		AT91C_PIO_PA15

// Commands for RFM_030_i2c
/**\brief	Auswählen einer Mifare Karte*/
#define SELECT_MIFARE_CARD	0x01
/**\brief	Einloggen in einen Sektor mit dem Schlüssel*/
#define LOGIN_TO_SECTOR		0x02
/**\brief	Lesen eines Datenblocks*/
#define READ_DATA_BLOCK		0x03
/**\brief	Schreiben eines Datenblocks*/
#define WRITE_DATA_BLOCK 	0x04
/**\brief	Lesen eines Wertes*/
#define READ_VALUE_BLOCK 	0x05
/**\brief	Schreiben eines Wertes*/
#define WRITE_VALUE_BLOCK 	0x06
/**\brief	Schreiben des Schlüssels*/
#define WRITE_MASTER_KEY 	0x07
/**\brief	Inkrementieren eines Wertes*/
#define INCREMENT_VALUE 	0x08
/**\brief	Dekrementieren eines Wertes*/
#define DECREMENT_VALUE 	0x09
/**\brief	Kopieren eines Wertes*/
#define COPY_VALUE 			0x0A
/**\brief	Ultralight: Lesen einer Seite*/
#define UL_READ_DATA_PAGE 	0x10
/**\brief	Ultralight: Schreiben einer Seite*/
#define UL_WRITE_DATA_PAGE 	0x11
/**\brief	Suspend*/
#define POWER_DOWN_MODE		0x50

// Status
/**\brief	Operation erfolgreich ausgeführt*/
#define STATUS_OPERATION_SUCCESS 		0x00
/**\brief	Kein Tag im Feld*/
#define STATUS_NO_TAG					0x01
/**\brief	Login erfolgreich*/
#define STATUS_LOGIN_SUCCESS			0x02
/**\brief	Login fehlgeschlagen*/
#define STATUS_LOGIN_FAILED				0x03
/**\brief	Lesen fehlgeschlagen*/
#define STATUS_READ_FAILED				0x04
/**\brief	Schreiben fehlgeschlagen*/
#define STATUS_WRITE_FAILED				0x05
/**\brief	Lesen nach dem schreiben nicht möglich*/
#define STATUS_UNABLE_READ_AFTER_WRITE	0x06
/**\brief	Lesen nach dem schreiben fehlgeschlagen*/
#define STATUS_READ_AFTER_WRITE_ERROR	0x07
/**\brief	Kollision*/
#define STATUS_COLLISION_OCCUR			0x0A
/**\brief	Laden des Schlüssels fehlgeschlagen*/
#define STATUS_LOAD_KEY_FAILED			0x0C
/**\brief	Nicht Berechtigt*/
#define STATUS_NOT_AUTHENTICATE			0x0D

// Kartentypen
/**\brief	Mifare STD 1 Kilobyte*/
#define TYPE_STANDARD_1K		0x01
/**\brief	Mifare UltraLight 512 Byte*/
#define TYPE_ULTRA_LIGHT		0x03
/**\brief	Mifare STD 4 Kilobyte*/
#define TYPE_STANDARD_4K		0x04

// Variables
extern unsigned int tag_detected;
extern TAG tag;
extern int dataTOsend;

// Prototypes
void rfid();
int RFID_Init();
int RFID_select();
int RFID_suspend();
int RFID_Read_UL(int page, char * data);
int RFID_Write_UL(int page, char * data);
int RFID_Read_STD(int sector, int block, char * data);
int RFID_getStatus_UL(void);
int RFID_setStatus_UL(char status);
int RFID_Operation_UL(void * pDate, enum RFID_OPERATION_MODE mode);
void choose_Race();

#endif /* RFID_H_ */


