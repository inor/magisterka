#include "m25px32.h"
#include "spi.h"

#define RECEIVE 0x00

#define sbi(x,y) x |= _BV(y) //set bit - using bitwise OR operator
#define cbi(x,y) x &= ~(_BV(y)) //clear bit - using bitwise AND operator
#define tbi(x,y) x ^= _BV(y) //toggle bit - using bitwise XOR operator
#define is_high(x,y) (x & _BV(y) == _BV(y)) //check if the y'th bit of register 'x' is high ... test if its AND with 1 is 1


#define m25_slave_select() cbi(PORTC, 2)
#define m25_slave_free() sbi(PORTC,2)
#define wait_for_spi() while(!bit_is_set(SPSR,SPIF))
#define wait_for_spi_ready() while(!spi_rdy)
#define spi_busy()	spi_rdy=0

extern volatile unsigned char spi_rdy;
volatile unsigned char spi_received_value;

void m25_send_command(unsigned char comm){
	wait_for_spi_ready();
	SPI_send_char(comm);
}
void m25_choose_address(unsigned long addr){
	wait_for_spi_ready();
	SPI_send_char((unsigned char)(addr>>16));
	wait_for_spi_ready();
	SPI_send_char((unsigned char)(addr>>8));
	wait_for_spi_ready();	
	SPI_send_char((unsigned char)addr);
}
unsigned char m25_read_byte(void){
	wait_for_spi_ready();
	SPI_send_char(RECEIVE);
	wait_for_spi_ready();
	return spi_received_value;
}
void m25_write_byte(unsigned char data){
	wait_for_spi_ready();
	SPI_send_char(data);
}
unsigned char m25_get_status(void){
	m25_send_command(M25_READ_STATUS_REGISTER);
	return m25_read_byte();
}
void m25_erase_chip(void){
	unsigned char tmp;	
	m25_send_command(M25_BULK_ERASE);
	m25_slave_free();
	asm("nop");
	do{
		m25_slave_select();
		tmp=m25_get_status();
		m25_slave_free();
	}while (tmp&0x01);
}
