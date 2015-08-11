// ***********************************************************
// Project: ACC Data
// Author: Konrad Neneman
// Module description:
// ***********************************************************

#include <avr\io.h>
#include "uart.h"
#include "spi.h"
#include "twi.h"
#include "BMC050.h"
#include "m25px32.h"
//#include "fifo.h"
//#include <string.h>
#include <avr/sleep.h>

#define DEBUG
//definicja pozwalajaca na kompilacje z wydrukami kontrolnymi
//zakomentowac w wersji release

#define sbi(x,y) x |= _BV(y) //set bit - using bitwise OR operator
#define cbi(x,y) x &= ~(_BV(y)) //clear bit - using bitwise AND operator
#define tbi(x,y) x ^= _BV(y) //toggle bit - using bitwise XOR operator
#define is_high(x,y) (x & _BV(y) == _BV(y)) //check if the y'th bit of register 'x' is high ... test if its AND with 1 is 1

#define m25_slave_select() cbi(PORTC, 2)
#define m25_slave_free() sbi(PORTC,2)


volatile unsigned char spi_rdy=0;
volatile unsigned char spi_received_value=0;
volatile unsigned char UART_rdy=0;

char check_user_button(void){
	unsigned char i=255;
	if (!(PIND&0x10)){ //sprawdz czy nacisnal przycisk
		
		while(i--);//poczekaj na koniec zaklocen
		if(!(PIND&0x10)) {//jesli dalej trzyma to poczekaj az pusci
			while(!(PIND&0x10));
			return 1;
		}
	}
	
	return 0;
	
}

void klawisz(void)  //czekaj na nacisnięcie okreslonego klawisza
{
	while(!check_user_button());
}

//liczy bit parzystosci liczby 16 bitowej zlozonej z dwoch jednobajtowych a i b. 1 oznacza nieparzysta liczbe jedynek, 0 parzysta liczbe
unsigned char parity_bit(unsigned char a, unsigned char b){
	a ^= a >> 1;
	a ^= a >> 2;
	a ^= a >> 4;

	b ^= b >> 1;
	b ^= b >> 2;
	b ^= b >> 4;
	
	return (a^b)&0x01;
	
}


unsigned char twi_get_data(unsigned char *buf){

	twi_receive_multichar(ACC_ADDR_I2C, 0x02, 6, buf);
	if(!(buf[0]&0x01)){
		tbi(PORTC,0);
		return 1;
	}
	else return 0;
}

//PRZERWANIE

ISR(SPI_STC_vect)
{
	//UART_send_char(0x50);
	spi_rdy=1;
	spi_received_value=SPDR;
	//UART_send_char(spi_received_value);
	
}


#define wait_for_spi_ready() while(!spi_rdy)
int main(void) {
	//unsigned char _UART_ind=0;
	unsigned char i, tmp;
	unsigned char i2c_buf[6];
	unsigned long addr=0x00001700;
	set_sleep_mode(SLEEP_MODE_IDLE);
	DDRC = 0x07;//jako wyjscia dla diod led ale przedewszystkim SS dla acc i mag. PC5 - acc; PC4 - mag
	PORTC = 0x04;//gasimy diody, SS w stanie wysokim
	PORTD=0x1C;//podciaganie do USER_BUTTON i wejsc przerwan

	UART_init();
	UART_rdy=1;
	SPI_init_master();
	spi_rdy=1;
	twi_init();

	MCUCR=0x0A;	//przerwanie generowane na narastajacym zboczu.
	//GICR=0xC0;	//wlaczamy przerwanie od int0 i int1-> zeby zaczelo dzialac funkcja sei();
	//GIFR=0xC0;	//czyszczenie flag przerwan od int0 i int1 -> zeby nie bylo zerowych przebiegow
	sei();

	
	UART_send_char(0x86);

	twi_acc_config(RANGE_4G, BW_500HZ);
	
	UART_send_char(0x82);
	m25_slave_select();
	UART_send_char(0x83);
	tmp=m25_get_status();
	UART_send_char(0x84);
	m25_slave_free();
	UART_send_char(0x85);
	UART_send_char(tmp);
	
	while(1) {
		//while (!(UCSRA & (1<<UDRE)));
		//UART_send_char(0x7F);
		UART_send_char(0x80);
		m25_slave_select();
		//klawisz();
		tmp=m25_get_status();
		m25_slave_free();
		
		
		UART_send_char(tmp);		
		
		/*m25_slave_select();
		m25_send_command(M25_PAGE_PROGRAM);
		m25_choose_address(addr);
		for(i=0;i<16;i++){
			m25_write_byte(i+11);
			//
			//UART_send_char(tmp);		
		}
		m25_slave_free();
		
		UART_send_char(0x88);
		klawisz();
		*/
		m25_slave_select();
		m25_send_command(M25_READ_IDENTIFICATION1);
		//m25_choose_address(addr);
		for(i=0;i<16;i++){
			tmp=m25_read_byte();
			
			UART_send_char(tmp);		
		}
		m25_slave_free();
		UART_send_char(0x78);
		klawisz();
		//for(i=0; i<5; ++i){
			//twi_get_data(i2c_buf);
		
			/*
			UART_send_char(i2c_buf[0]);
			
			UART_send_char(i2c_buf[1]);
			
			UART_send_char(i2c_buf[2]);
			
			UART_send_char(i2c_buf[3]);
			
			UART_send_char(i2c_buf[4]);
			
			UART_send_char(i2c_buf[5]);*/
		//}
	}
}
/*
	Notki:	Trzeba zadbać o kwarc 3,6864 -> choć do testow z uart, ale przyda sie wieksza czestotliwosc przy komunikacji. ograniczy tez
		pobor pradu.
*/






