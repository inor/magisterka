#include "spi.h"

extern volatile unsigned char spi_rdy;

void SPI_init_master()
{
	DDRB = ( 1 << PB5 ) | ( 1 << PB3 ) | ( 1 << PB2 );              //MOSI, SCK, SS' jako wyjścia
	SPCR = (1<<SPIE) | ( 1 << SPE ) | ( 1 << MSTR ) /*| ( 1 << SPR1 )*/ | ( 1 << SPR0 );   //komentarz -> usun 4x wolniejsza komunikacja
	//Włączamy SPI,    
}	//układ Master,  częstotliwość SCK fosc / 16 || aby wlaczyc przerwanie dodac na poczatku SPCR (1<<SPIE) | 

void SPI_init_slave()
{
	DDRB = ( 1 << PB4 );	//MISO jako wyjście   
	SPCR = ( 1 << SPE );	//Włączamy SPI
}

unsigned char SPI_recive_char()
{                            //Czekamy na koniec transmisji danych
	while ( ! bit_is_set( SPSR, SPIF ) );        // ( aż do ustawienie flagi SPIF    )
	return SPDR;                    //Zwracamy to co dostaliśmy do SPDR
}                             

inline void SPI_send_char(unsigned char z)
{
	spi_rdy=0;  
	SPDR = z;
	                  //Wysyłamy zawartość zmiennej bajt   
	//while( ! bit_is_set( SPSR, SPIF ) );        //Oczekujemy na zakończenie transmisji ( do ustawienia SPIF ) przez sprzęt
}                           

