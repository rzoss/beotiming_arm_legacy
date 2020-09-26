/**
 *******************************************************************************
 * \file    spi_config.h
 *******************************************************************************
 * \brief   Application and Hardware specific configurations for SPI driver.
 *
 *
 * \version		1.0
 * \date		22.3.2007
 * \author		M.Muehlemann
 *
 *******************************************************************************
 */

#ifndef SPI_CONFIG_H_
#define SPI_CONFIG_H_

/** \brief	Benutze SPI CS 0*/	
#define SPI_USE_CS0
/** \brief	Benutze SPI CS 1*/	
#define SPI_USE_CS1
///** brief	Benutze SPI CS 2*/	
//#define SPI_USE_CS2
///** brief	Benutze SPI CS 3*/	
//#define SPI_USE_CS3


// Clock and data IOs
/** \brief	Pin für MISO ist PA12*/
#define SPI_PIN_MISO		AT91C_PIO_PA12		// on Peripheral A
/** \brief	Pin für MOSI ist PA13*/
#define SPI_PIN_MOSI		AT91C_PIO_PA13		// on Peripheral A
/** \brief	Pin für SPCK ist PA14*/
#define SPI_PIN_SPCK		AT91C_PIO_PA14		// on Peripheral A

/** \brief	Verzögerung zwischen zwei ChipSelect (20 * 1/48MHz = ~ 400ns)*/
#define DELAY_BETWEEN_CS		20	// 20 * 1/48MHz = ~ 400ns

// Chip-Selects
#ifdef SPI_USE_CS0
	/** \brief  Pin für CS0 ist PA11*/
	#define SPI_PIN_CS0		AT91C_PIO_PA11		// on Peripheral A
	/** \brief  Konfiguration für das Gerät 0*/
	#define CS0_CONFIG 	 AT91C_SPI_CPOL | \
						 AT91C_SPI_BITS_8 | \
						 ((200<<8) & AT91C_SPI_SCBR) | \
						 ((1<<24) & AT91C_SPI_DLYBCT);
#endif

#ifdef SPI_USE_CS1
	/** \brief  Pin für CS1 ist PA31*/
	#define SPI_PIN_CS1		AT91C_PIO_PA31		// on Peripheral A
	/** \brief  Konfiguration für das Gerät 1*/
	#define CS1_CONFIG 	 AT91C_SPI_CPOL | \
						 AT91C_SPI_BITS_8 | \
						 ((200<<8) & AT91C_SPI_SCBR) | \
						 ((1<<24) & AT91C_SPI_DLYBCT);

#endif

// CAN Controller
#ifdef SPI_USE_CS2
	/** \brief  Pin für CS2 ist PA30*/
	#define SPI_PIN_CS2		AT91C_PIO_PA30		// on Peripheral B
	/** \brief  Konfiguration für das Gerät 2*/
	#define CS2_CONFIG 	 AT91C_SPI_CPOL |  		 	 	/* CS low active */ \
						 AT91C_SPI_CSAAT |				/* hold CS low betwen transfers */ \
						 AT91C_SPI_BITS_8 | 			/* 8 Bits */ \
						 ((5<<8) & AT91C_SPI_SCBR) | 	/* Baud Rate = 48MHz/5 = 9.6MHz */ \
						 ((3<<24) & AT91C_SPI_DLYBS) |	/* Delay Before SBCK:  ~ 60ns */ \
						 ((0<<24) & AT91C_SPI_DLYBCT);	/* Delay between transfers:  ~ 60ns */
#endif

#ifdef SPI_USE_CS3
	/** \brief   Pin für CS3 ist PA3*/
	#define SPI_PIN_CS3		AT91C_PIO_PA3		// on Peripheral B
	/** \brief  Konfiguration für das Gerät 3*/
	#define CS3_CONFIG 	 AT91C_SPI_CPOL | \
						 AT91C_SPI_CSAAT |				/* hold CS low betwen transfers */ \
						 AT91C_SPI_BITS_8 | \
						 ((48<<8) & AT91C_SPI_SCBR) | 	/* Baud Rate = 48MHz/48 =  1 MHz */ \
						 ((1<<24) & AT91C_SPI_DLYBS) |	/* Delay Before SBCK: */ \
						 ((0<<24) & AT91C_SPI_DLYBCT);	/* Delay between transfers:*/

#endif




#endif /*SPI_CONFIG_H_*/
