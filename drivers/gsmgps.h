/**
 *******************************************************************************
 * \file    gsmgps.h
 *******************************************************************************
 * \brief   Telit GSM/GPS driver for AT91SAM7Sxxx
 * 			
 *
 * \version	1.0
 * \date	05.05.2008
 * \author  M. Leiser
 *
 *******************************************************************************
 */

#ifndef GSMGPS_H_
#define GSMGPS_H_


/**
 * \brief	Enumeration for GSM/GPS State Machine.
 */
int gsmgps_state;
enum GSM_STATE{
	INIT_GSM,
	IDLE,
	GSM_SLEEP,
	GPRS_CONNECT,
	CONNECT_TRYMAX,
	GPRS_CONNECTED,
	GSM_ERROR_STATE
};

/**
 * \brief	Enumeration for INIT State 
 */
int init_state;
enum INIT_STATE{
	HW_ON,
	SEND1,
	SEND2,
	SEND3,
	SEND4,
	SEND5,
	SEND6,
	SEND7,
	SEND8,
};

/**
 * \brief	Enumeration for IDLE State 
 */
int idle_state;
enum IDLE_{
	GPS_FIX,
	GPS_FIX2,
	GPS_OFF
};

/**
 * \brief	Enumeration for GSM_SLEEP State 
 */
int gsm_sleep_state;
enum GSM_SLEEP{
	GOTO_SLEEP,
	IN_SLEEP,
	WAKEUP
};


/**
 * \brief	Enumeration for GPRS_CONNECT State 
 */
int gprs_connect_state;
enum GPRS_CONNECT{
	CONTEXT_SETTING,
	CONTEXT_SETTING1,
	CONTEXT_SETTING2,
	CHECK_CONNECTION,
	CHECK_CONNECTION2,
	CONNECT1,
	CONNECT2,
	CONNECT3
};

/**
 * \brief	Enumeration for GPRS_CONNECTED State 
 */
int gprs_connected_state;
enum GPRS_CONNECTED{
	CONNECTED_IDLE,
	CONNECTED_IDLE2,
	PUT_SEMA,
	CHECK_DATA,
	CHECK_DATA2,
	PREPARE_DATA,
	SEND_DATA,
	SEND_DATA2,
	SEND_NROFFILES,
	SEND_DELETESEMA,
	GOTO_IDLE,
	PUT_OK,
	GPRS_DISCONNECT,
	GPRS_DISCONNECT2
};

/**
 * \brief	Enumeration for FTP_PUTFILE function
 */
int ftp_putfile_state;
enum FTP_PUTFILE{
	FTP_PUT,
	FTP_PUT1,
	FTP_PUT2a,
	FTP_PUT2b,
	FTP_PUT3,
	FTP_PUT4,
	FTP_PUT5,
	FTP_PUT6,
	FTP_PUT7
};

/**
 * \brief	Enumeration for FTP_GETFILE function
 */
int ftp_getfile_state;
enum FTP_GETFILE{
	FTP_GET,
	FTP_GET1,
	FTP_GET2a,
	FTP_GET2b,
	FTP_GET3,
	FTP_GET4,
	FTP_GET5,
	FTP_GET6,
	FTP_GET7
};

/**
 * \brief	Enumeration for FTP_DELETEFILE function
 */
int ftp_deletefile_state;
enum FTP_DELETEFILE{
	FTP_DELETE,
	FTP_DELETE1,
	FTP_DELETE2a,
	FTP_DELETE2b,
	FTP_DELETE3,
	FTP_DELETE4,
	FTP_DELETE5
};




#define GSM_ERROR	99

// IO pins
/**
 * \brief	TELIT_Reset ist Pin PA23
 */
#define PIN_TELIT_RESET		AT91C_PIO_PA23
/**
 * \brief	TELIT_ON_OFF ist Pin PA19
 */
#define PIN_TELIT_ON_OFF	AT91C_PIO_PA19
/**
 * \brief	TELIT_PWRMON ist Pin PA20
 */
#define PIN_TELIT_PWRMON	AT91C_PIO_PA20



// AT Commands
/**
 * \brief	Definition der AT Kommandos
 */
#define AT 					"AT\r"					//Autobauding
#define AT_SHUTDOWN 		"AT#SHDN\r"				//Software Shutdown
#define AT_IPR 				"AT+IPR=38400\r"		//Fix Baudrate
#define AT_K0 				"AT&K0\r"				//Disable Hardware Flow Control
#define AT_ERROR_VERBOSE 	"AT+CMEE=2\r"			//ERROR Result Codes in Verbose Format
#define AT_ERROR_NUMERIC 	"AT+CMEE=1\r"			//ERROR Result Codes in Numeric Format
#define AT_SIM_PRESENCE 	"AT+CPIN?\r"			//Query SIM presence and status
#define AT_PIN 				"AT+CPIN="				//Provide SIM Pin
#define AT_PUK 				"AT+CPIN=43114140,6238\r"	//Provide SIM Puk and set New Pin
#define AT_NET_STATE 		"AT+CREG?\r"			//Query Network status
#define AT_NET_OPERATOR		"AT+COPS=?\r"			//Query Network Operator
#define AT_SMS_TEXT 		"AT+CMGF=1\r"			//Set SMS Mode to TEXT
#define AT_GPS				"AT$GPSACP\r"			//Abfrage von GPS NMEA Protokoll

// AT Commands
/**
 * \brief	Definition Der Command und Response Längen
 */
#define USART_COMMAND_LEN	80
#define USART_ANSWER_LEN	200 


/**
 * \brief  	STRUCT type for GPS NMEA Type Answer
 **/
typedef struct{
	char time[11];							//Zeit hhmmss.ms UTC
	int hour;								//hh
	int minute;								//mm
	int second;								//ss
	int milisecond;							//ms 
	char date[8];							//Datum dd mm yy
	int day;								//dd
	int month;								//mm
	int year;								//yy
	int fix;								//0 if fix invalid, 1 if 2D-, 3 if 3D-fix
	int NrOfSat;							//Wie viele Satelitten in Sicht
	double precision;						//Präzision der GPS Daten (horizontal diluition of Precision)
	char nmea_sentence[USART_ANSWER_LEN];	//gesamter NMEA Protokollstring
}NMEA;

NMEA NMEA_Protocol;



char strCommand[USART_COMMAND_LEN],strAnswer[USART_ANSWER_LEN];

extern int recieved;
extern int startedup;
extern int ExtSent;
extern int ExtRecieved;
extern int ExtGsmInt;

char answer[USART_ANSWER_LEN];  // response buffer until response is valid

void gsmgps();
int gsmgps_init();
int gsmgps_HW_on();
int gsmgps_HW_off();
int gsmgps_check();
int selfAtoi(char* in);
double selfAtof(char* in);
void timestringTOint(char* in);
void datestringTOint(char* in);
void gsmgps_putcommand(char * command);
void gsmgps_putAndCheck(char* command, char strAnswer[20], int* state);
void checkEndOfResponse();
void gsmgps_to_debug(char chr);
void gsm_sleep();

void gsmgps_recieve_answer(char chr);
void gps_to_debug(char chr);
void ftp_getfile(char * filename, char * data);
void ftp_putfile(char * filename, char * data);
void ftp_deletefile(char * filename);


void init_gsm();
void idle();
void gprs_connect();
void gprs_connected();
//int UART_waitForRX(UART uart, int timeout);
//int UART_waitForTX(UART uart, int timeout);






#endif /*GSMGPS_H_*/
