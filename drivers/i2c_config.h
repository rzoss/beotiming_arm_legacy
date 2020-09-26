/**
 *******************************************************************************
 * \file    i2c_config.h
 *******************************************************************************
 * \brief    Configuration for I2C driver for AT91SAM7Sxxx.
 * 
 * \version		1.0
 * \date		16.2.2008
 * \author		M.Muehlemann
 *
 *******************************************************************************
 */
#ifndef I2C_CONFIG_H_
#define I2C_CONFIG_H_

/**
 * \brief F_TWI
 */
#define F_TWI	10000


// TWI Clock

/**
 * \brief Clock High Divider
 */
#define CHDIV CLDIV						// CHDIV = CLDIV
/**
 * \brief Clock Divider
 */
#define CKDIV	1						// CKDIV = 1
/**
 * \brief Clock Low Divider
 */
#define CLDIV   (MCK/(4*F_TWI)-3)

/**
 * \brief TWI Clock Waveform Generation Register
 */
#define CWGR_INIT 		((CLDIV&0xff) | ((CHDIV&0xff)<<8) | ((CKDIV&0x07)<<16))

#endif /*I2C_CONFIG_H_*/
