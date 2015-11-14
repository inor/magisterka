#include "spi.h"

volatile bool spi_rdy;

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

inline void SPI_send_char(uint8_t z)
{
	while (spi_rdy==false);
	spi_rdy=false;
	SPDR = z;
	                  //Wysyłamy zawartość zmiennej bajt   
	//while( ! bit_is_set( SPSR, SPIF ) );        //Oczekujemy na zakończenie transmisji ( do ustawienia SPIF ) przez sprzęt
}                           

//PRZERWANIE

ISR(SPI_STC_vect)
{
	spi_rdy=true;
	spi_received_value=SPDR;

}
