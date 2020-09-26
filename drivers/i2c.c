/**
 *******************************************************************************
 * \file    i2c.c
 *******************************************************************************
 * \brief    I2C driver for AT91SAM7Sxxx.
 * 
 * \version		2.0
 * \date		13.03.2008
 * \author		M.Muehlemann, R.Zoss
 *
 *******************************************************************************
 */

#include <AT91SAM7S256.h>
#include <lib_AT91SAM7S256.h>

#include "string.h"
#include "stdio.h"
#include "uart.h"
#include "global.h"
#include "i2c.h"
#include "i2c_config.h"

/**
 * \brief	Timeout Counter für i2c
 */
int toCntr=0;

/**
 * \brief	Initialize TWI controller.
 * 
 * \return 	NO_ERROR
 */
int i2c_init(void)
 {	 // set I/O pins for I2C
	 AT91F_TWI_CfgPIO(); 

	 //	Power management  
	 AT91F_TWI_CfgPMC(); 

	 //	Disable TWI-interrupts, soft-reset, enable TWI
	 AT91F_TWI_Configure(AT91C_BASE_TWI); 

	 // Set TWI Clock
	 *AT91C_TWI_CWGR = CWGR_INIT;

	 // Set the device address 0x00 (7 bits), addressable space device 0 bits, read
	 *AT91C_TWI_MMR= 0x00;
	 return NO_ERROR;
 }
 /**
  * \brief		Write one or more bytes to the I2C-Bus.
  * 
  * \param[in]		slave_address		Slave Address (7 bits)
  * \param[in]		intAddr				Array of bytes with internal address. Length from 0 to 4 bytes.
  * \param[in]		numIntAddr			Number of elements in intAddr array
  * \param[in]		data				Pointer to send buffer
  * \param[in]		len					Number of bytes in send buffer
  * 
  * \return			NO_ERROR on sucess or a error code (see i2c.h)
  * 
  * */
 int i2c_write(int slave_address,char* intAddr, int numIntAddr, char* data, int len){
	 int toSend = len;
	// int toCntr;
	 
	 AT91PS_TWI pTWI = AT91C_BASE_TWI;

	 // set master mode register
	 pTWI->TWI_MMR = ((slave_address&0x7f)<<16) | (numIntAddr%0x03)<<8;
	 
	 // set internal address
	 if(numIntAddr){
		 unsigned int iadr = 0;
		 int i;
		 // check parameter
		 if(numIntAddr > 3){
			 return ERROR_I2C_INVALID_PARAMETER;
		 }
		 for(i=0; i<numIntAddr; i++){
			 iadr |= intAddr[i]<<(i*8);
		 }
		 pTWI->TWI_IADR = iadr;
	 }

     toCntr = MAX_TO_COUNT;
     // send data
     while(toSend) {
         // load transmit holding register
    	 pTWI->TWI_THR = *data++;
    	 toSend--;
    	 
    	 // wait until transmit-hold-register is empty
         while(--toCntr >0 && !(pTWI->TWI_SR & AT91C_TWI_TXRDY));
         if(toCntr <= 0){
        	 return ERROR_I2C_TIMEOUT;
         }
         
         if(pTWI->TWI_SR & AT91C_TWI_NACK){
        	 return ERROR_I2C_NACK;
         }    
     }
    
     toCntr = MAX_TO_COUNT;
	 // wait until transmit is complete
     while((toCntr--) && !(pTWI->TWI_SR & AT91C_TWI_TXCOMP));
     if(!toCntr){
    	 return ERROR_I2C_TIMEOUT;
     }
     
     return NO_ERROR;	 
 }
/**
 * \brief		Read one or more bytes from the I2C-Bus.
 * 
 * \param[in]		slave_address		Slave Address (7 bits)
 * \param[in]		intAddr				Array of bytes with internal address. Length from 0 to 4 bytes.
 * \param[in]		numIntAddr			Number of elements in intAddr array
 * \param[out]		data				Pointer to send buffer
 * \param[in]		len					Number of bytes in send buffer
 * \param[out]		readBytes			Pointer ti a varaiable that will contain the number of read bytes
 * 
 * \return		NO_ERROR on sucess or a error code (see i2c.h)
 * 
 * */
 int i2c_read(int slave_address,char* intAddr, int numIntAddr,char* data, int len, int* readBytes){
	 //int toCntr;
	 AT91PS_TWI pTWI = AT91C_BASE_TWI;
 
	 *readBytes = 0;
 
	 // set master mode register
	 pTWI->TWI_MMR = ((slave_address&0x7f)<<16) | AT91C_TWI_MREAD | (numIntAddr%0x03)<<8;
	 
	 // set internal address
	 if(numIntAddr){
		 unsigned int iadr=0;
		 int i;
		 // check parameter
		 if(numIntAddr > 3){
			 return ERROR_I2C_INVALID_PARAMETER;
		 }
		 for(i=0; i<numIntAddr; i++){
			 iadr |= intAddr[i]<<(i*8);
		 }
		 pTWI->TWI_IADR = iadr;
	 }
	 
	 // empty read buffer
	 pTWI->TWI_RHR;

	  pTWI->TWI_CR = AT91C_TWI_START;		 
	 
     // wait until first data byte was received
     int sr;
     toCntr = MAX_TO_COUNT;
     while(--toCntr >0 && ((*readBytes) < len)){    	 
    	 
    	 if((len - (*readBytes)) == 1){
    		 pTWI->TWI_CR = AT91C_TWI_STOP;
    	 }
    	 
    	 // check status
         sr = pTWI->TWI_SR;
         // watch for address NACK
         if(sr & AT91C_TWI_NACK){
             return ERROR_I2C_NACK;
         }

         if(sr & AT91C_TWI_RXRDY){
             // read transmit holding register
              *data++ = pTWI->TWI_RHR;
              (*readBytes)++;
              toCntr = MAX_TO_COUNT;
              
         }
         
   
     }
     
     if(!toCntr){
    	 return ERROR_I2C_TIMEOUT;
    	 TRACE("ERROR_I2C_TIMEOUT");
    	 //pTWI->TWI_CR &= AT91C_TWI_SWRST; //1 auf 7. bit
     }
   
	 return NO_ERROR;
 }
 
 


