/**
 *******************************************************************************
 * \file    spi.c
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

#include <AT91SAM7S256.h>
#include <lib_AT91SAM7S256.h>

#include <printf.h>
#include "spi_config.h"

/**
 * \brief	Initialisiert die SPI-Schnittstelle
 * 
 * \return 	no return
 */
void spi_init(){
	AT91PS_SPI pSPI  = AT91C_BASE_SPI;
	
	// enable SPI Clock
	AT91F_SPI_CfgPMC();
		
	// configure SPI inputs and Outputs
	// configure IOs: SPCK, MOSI, MISO (all on peripheral A)
	AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA, SPI_PIN_MISO | SPI_PIN_MOSI | SPI_PIN_SPCK,0);

	#ifdef SPI_USE_CS0
		// allways on peripheral A
		AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA, SPI_PIN_CS0, 0);
	#endif

	#ifdef SPI_USE_CS1
		// can be on Periheral A or B!!!
		AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA, SPI_PIN_CS1, 0);
	#endif

	#ifdef SPI_USE_CS2
		// allways on peripheral B
		AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA, 0, SPI_PIN_CS2);
	#endif

	#ifdef SPI_USE_CS3
		// allways on peripheral B
		AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA, 0, SPI_PIN_CS3);
	#endif

	// reset SPI
	AT91F_SPI_Reset(pSPI);
	
	// enable SPI
	AT91F_SPI_Enable( pSPI );
	
	// configure SPI-Mode
	pSPI->SPI_MR = AT91C_SPI_MSTR |			// Master-Mode
				   AT91C_SPI_PS |			// Variable Peripheral Select
				   AT91C_SPI_MODFDIS |		// Mode fault detection disabled
				   ((DELAY_BETWEEN_CS<<24) & AT91C_SPI_DLYBCS);
	
	
	// configure SPI for CS0
	#ifdef SPI_USE_CS0
		pSPI->SPI_CSR[0] = CS0_CONFIG;
	#endif
		
	#ifdef SPI_USE_CS1
		pSPI->SPI_CSR[1] = CS1_CONFIG;
	#endif
	
	#ifdef SPI_USE_CS2
			pSPI->SPI_CSR[2] = CS2_CONFIG;
	#endif
	
	#ifdef SPI_USE_CS3
			pSPI->SPI_CSR[3] = CS3_CONFIG;
	#endif
	
}

/**
 * \brief	sendet Daten an ein Gerät auf dem SPI-Bus
 * 
 * \param[in]	c 				zu sendende Daten
 * \param[in]	cs				Gerätenummer
 * \param[in]	lastTransfer	letzter Transfer?
 * \param[in]	waitforSend		warten bis versendet?
 * 
 * \return		no return
 */	
void spi_write(int c, int cs, int lastTransfer, int waitforSend){
	AT91PS_SPI pSPI  = AT91C_BASE_SPI;
	
	// prepare tdr
	int tdr = c & 0xffff;
	tdr |= ((~(1 << cs)) & 0x0f) <<16;
	if(lastTransfer){
		tdr |= AT91C_SPI_LASTXFER;
	}
			
	// wait until last transfer is finished
	while( !( pSPI->SPI_SR & AT91C_SPI_TDRE ) ); // wait until transferbuffer is empty
	
	
	
	
	//send byte
	 pSPI->SPI_TDR = tdr;
	 
	 if(waitforSend){
		while( !( pSPI->SPI_SR & AT91C_SPI_TXEMPTY ) ); // wait until transfer is finished 
	 }
}

/**
 * \brief	empfängt Daten von einem Gerät auf dem SPI-Bus
 * 
 * \param[in]	cs				Gerätenummer
 * \param[in]	lastTransfer	letzter Transfer?
 * 
 * \return		Empfangene Daten
 */	
int spi_read(int cs, int lastTransfer){
	AT91PS_SPI pSPI  = AT91C_BASE_SPI;
	
	// wait until transferbuffer is empty
	while( !( pSPI->SPI_SR & AT91C_SPI_TXEMPTY ) );

	// empty receive Buffer
	pSPI->SPI_RDR;

	spi_write(0xffff,cs,lastTransfer,0);
	
	// wait until receivebuffer is full
	while( !( pSPI->SPI_SR & AT91C_SPI_RDRF ) );
	
	// read data
	return ( pSPI->SPI_RDR ) & 0xffff;

}





