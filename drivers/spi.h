/**
 *******************************************************************************
 * \file    spi.h
 *******************************************************************************
 * \brief    SPI driver for AT91SAM7Sxxx.
 *
 *
 * \version		1.0
 * \date		22.3.2007
 * \author		M.Muehlemann
 *
 *******************************************************************************
 */
#ifndef SPI_H_
#define SPI_H_


void spi_init();
void spi_write(int c, int cs, int lastTransfer, int waitforSend);
int spi_read(int cs, int lastTransfer);


#endif /*SPI_H_*/
