#include "m25px32.h"
#include "spi.h"
#include "uart.h"
#include <util/delay.h>

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

flash_state m25_state;
//inicjalizujemy pamiec flash, czyscimy i zaczynami pisac od poczatku
void m25_init(void){
	SPI_init_master();
	spi_rdy=1;
	sei();
}

void m25_prepare_to_start(void){
	m25_slave_select();
	m25_erase_chip();

	m25_slave_select();
	m25_send_command(M25_WRITE_ENABLE);
	m25_send_command(M25_PAGE_PROGRAM);
	m25_send_command((unsigned char)((m25_get_state()->page_index)>>8));
	m25_send_command((unsigned char)(m25_get_state()->page_index));
	m25_send_command(m25_get_state()->cell_index);
}

void m25_save_byte(unsigned char byte)
{
	m25_send_command(byte);
	if(++(m25_get_state()->cell_index)==0){
		(m25_get_state()->page_index)++;
		if (m25_get_state()->page_index>M25_MAX_PAGE_ADDRESS){
			cbi(PORTC, 1); //zapalamy czerwona diode
			wait_for_spi_ready();
			m25_slave_free();
			while(1);//skonczyla nam sie pamiec. Czekamy na reset
		}
		else {
			wait_for_spi_ready();
			m25_slave_free();
			asm("nop");asm("nop");asm("nop");
			m25_slave_select();
			m25_send_command(M25_WRITE_ENABLE);
			m25_send_command(M25_PAGE_PROGRAM);
			m25_send_command((unsigned char)((m25_get_state()->page_index)>>8));
			m25_send_command((unsigned char)(m25_get_state()->page_index));
			m25_send_command(m25_get_state()->cell_index);
		}
	}
	

}

void m25_save_6bytes(unsigned char* bytes){
	m25_save_byte(bytes[0]);
	m25_save_byte(bytes[1]);
	m25_save_byte(bytes[2]);
	m25_save_byte(bytes[3]);
	m25_save_byte(bytes[4]);
	m25_save_byte(bytes[5]);
}

void m25_load_all_data(void){
	unsigned long i=0;
	unsigned char tmp;
	
	m25_slave_select();
	m25_send_command(M25_READ_DATA_BYTES);
	m25_send_command(0x00);
	m25_send_command(0x00);
	m25_send_command(0x00);
	for(i=0x1FFFFF; i<0x3FFFFF; ++i){
		tmp = m25_read_byte();
		UART_send_char(tmp);		
	}
}

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
		_delay_ms(20);
		m25_slave_select();
		tmp=m25_get_status();
		m25_slave_free();
		UART_send_char(tmp);
		
	}while (tmp&0x01);
}

//------------------------------------PRIVATE--------------------------------------
inline flash_state_p m25_get_state(void){
	return &m25_state;
}




