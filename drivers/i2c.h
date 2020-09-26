/**
 *******************************************************************************
 * \file    i2c.h
 *******************************************************************************
 * \brief    I2C driver for AT91SAM7Sxxx.
 * 
 * \version		2.0
 * \date		13.03.2008
 * \author		M.Muehlemann
 *
 *******************************************************************************
 */
#ifndef I2C_H_
#define I2C_H_

/**
 * \brief	Wert für Timeout
 */
#define MAX_TO_COUNT						10000

/**
 * \brief I2C-Bus Timeout
 */
#define ERROR_I2C_TIMEOUT					1
/**
 * \brief	Not Acknowledge
 */
#define ERROR_I2C_NACK						2
/**
 * \brief	Falsche Parameter
 */
#define ERROR_I2C_INVALID_PARAMETER			3

// function prototypes
int i2c_init();
int i2c_read (int slave_address,char* intAddr, int numIntAddr,char* data, int len, int* readBytes);
int i2c_write(int slave_address,char* intAddr, int numIntAddr, char* data, int len);

#endif /*I2C_H_*/
