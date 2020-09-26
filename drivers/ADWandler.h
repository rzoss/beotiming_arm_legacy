/**
 *******************************************************************************
 * \file    ADWandler.h
 *******************************************************************************
 * \brief    Driver für AD Wandler auf Connector Board
 * 
 * \version		1.0
 * \date		17.08.2008
 * \author		M. Leiser
 *
 *******************************************************************************
 */

#ifndef ADWANDLER_H_
#define ADWANDLER_H_

/**
 * \brief Adresse des AD Wandlers
 */
#define ADWADR 		0x48


/**
 * \brief control bytes
 */
#define ADW_CONTROL1 		0x0		//ILast
#define ADW_CONTROL2 		0x01	//USolar
#define ADW_CONTROL3 		0x02	//ISolar
#define ADW_CONTROL4 		0x03	//ULast
#define ADW_CONTROL_INC		0x04	//Auto Increment

//Structs
/**
 * \brief  	STRUCT type für Werte des AD Wandlers (werden vorher umgerechnet)
 **/
typedef struct{
	int ULast;						//Spannung an der Last 
	int ILast;						//Strom an der Last
	int USolar;						//Spannung an der Solarzelle
	int ISolar;						//Strom an der Solarzelle
	
}ADW_S;

ADW_S ADWerte;


// Variables


// Prototypes

//int ADW_Init();
int ADW_read();

int ADW_read_inc();

#endif /* ADWANDLER_H_ */


