/**
 *******************************************************************************
 * \file    rtc.h
 *******************************************************************************
 * \brief    RTC driver for Maxim DS1338Z
 * 
 * \version		2.0
 * \date		24.04.2008
 * \author		R.Zoss
 *
 *******************************************************************************
 */

#ifndef RTC_H_
#define RTC_H_

/**\brief	Adresse des RTC's*/
#define RTC_ADR 			0x68

// RTC Registers
/**\brief	Register Adresse der Sekunde*/
#define RTC_REG_SEC			0x00	
/**\brief	Register Adresse der Minute*/
#define RTC_REG_MIN			0x01	
/**\brief	Register Adresse der Stunde*/
#define RTC_REG_HOUR		0x02	
/**\brief	Register Adresse der Wochentag*/
#define RTC_REG_DAY			0x03	
/**\brief	Register Adresse der Tag*/
#define RTC_REG_DATE		0x04	
/**\brief	Register Adresse der Monat*/
#define RTC_REG_MONTH		0x05	
/**\brief	Register Adresse der Jahr (0-99)*/
#define RTC_REG_YEAR		0x06	
/**\brief	Register Adresse der Kontolle*/
#define RTC_REG_CONTROL		0x07	

// RTC control register
/**\brief	Output Control*/
#define RTC_CTRL_OUT	(1 << 7)
/**\brief	Oscillator Stop Flag*/
#define RTC_CTRL_OSF	(1 << 5)
/**\brief	Square-Wave Enable*/
#define RTC_CTRL_SQWE	(1 << 4)
/**\brief	Rate Select*/
#define RTC_CTRL_RS1	(1 << 1)
/**\brief	Rate Select*/
#define RTC_CTRL_RS0	(1 << 0)


extern int start_sync;
extern int sleep;
extern int time_valid;


/**\brief Zustände der RTC Zustandsmaschine */
int rtc_state;
enum RTC_STATE{
	START_RTC,
	SLEEP,
	WAKE
};

//#define BCD_TO_DEC(_bcd_) ((_bcd_&0x0f) + ((_bcd_>>4)&0x0f)*10)
//#define DEC_TO_BCD(_dec_) ((_dec_/10)<< 4)+(_dec_%10))

/**\brief	Zeitstrukur für das RTC*/
typedef struct _TIME {
	unsigned short	milisekunden;	/**< Milisekunden */
	unsigned short 	sekunden;		/**< Sekunden */
	unsigned short 	minuten;		/**< Minuten */
	unsigned short 	stunden;		/**< Stunden */
	unsigned short  wochentag;		/**< Wochentag */
	unsigned short 	tage;			/**< Tage */
	unsigned short 	monate;			/**< Monate */
	unsigned short  jahre;			/**< Jahre (0-99)*/
	char			string[25]; 	/**< Datum und Zeit als ASCII-Zeichenkette */
} TIME;

/**\brief	Aktuelle Uhrzeit*/
TIME actTime;

/**
 * \brief 		konvertiert Dezimal zu BCD-Format
 * \param[in]	dec		Dezimalzahl
 * \return		BCD-Zahl
 */
inline static BYTE DEC_TO_BCD(BYTE dec) {
	return (((dec/10) << 4)+(dec%10));
}

/**
 * \brief 		konvertiert BCD-Format zu Dezimal
 * \param[in]	bcd		BCD-Zahl
 * \return		Dezimalzahl
 */
inline static BYTE BCD_TO_DEC(BYTE bcd) {
	return ((bcd & 0x0f) + ((bcd >> 4) & 0x0f)*10);
}

void rtc();
int rtc_init(TIME* actTime);
int rtc_set_time(TIME* date);
int rtc_get_time(TIME* date);
int rtc_set_time_from_NMEA();
int rtc_read_reg(char * regAdr ,char *data, char length);
int rtc_write_reg(char *data, char length);

#endif /*RTC_H_*/
