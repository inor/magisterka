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

static uint8_t m25_read_byte(void);
static uint8_t m25_get_status(void);
static void m25_send_command(unsigned char comm);

extern volatile unsigned char spi_rdy;
volatile unsigned char spi_received_value;

typedef struct flash_state{
	uint32_t flash_adress;
	bool page_enabled;
} flash_state, * flash_state_p;

flash_state m25_state;
//inicjalizujemy pamiec flash, czyscimy i zaczynami pisac od poczatku
void m25_init(void){
	m25_state.flash_adress=0;
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
	m25_send_command((uint8_t) (m25_state.flash_adress>>16));
	m25_send_command((uint8_t) (m25_state.flash_adress>>8));
	m25_send_command((uint8_t) m25_state.flash_adress);
}

bool m25_save_byte(uint8_t byte)
{
	m25_send_command(byte);
	m25_state.flash_adress++;
	if(m25_state.flash_adress%256==0){
		if (m25_state.flash_adress>0x3FFFFF){
			cbi(PORTC, 1); //zapalamy czerwona diode
			wait_for_spi_ready();
			m25_slave_free();
			return false;
		}
		else {
			wait_for_spi_ready();
			m25_slave_free();
			asm("nop");asm("nop");asm("nop");
			m25_slave_select();
			m25_send_command(M25_WRITE_ENABLE);
			m25_send_command(M25_PAGE_PROGRAM);
			m25_send_command((uint8_t) (m25_state.flash_adress>>16));
			m25_send_command((uint8_t) (m25_state.flash_adress>>8));
			m25_send_command((uint8_t) m25_state.flash_adress);
		}
	}
	return true;
	

}

bool m25_save_6bytes(unsigned char* bytes){
	bool ret=true;
	ret &= m25_save_byte(bytes[0]);
	ret &= m25_save_byte(bytes[1]);
	ret &= m25_save_byte(bytes[2]);
	ret &= m25_save_byte(bytes[3]);
	ret &= m25_save_byte(bytes[4]);
	ret &= m25_save_byte(bytes[5]);
	return ret;
}

void m25_load_all_data(void){
	unsigned long i=0;
	unsigned char tmp;
	
	m25_slave_select();
	m25_send_command(M25_READ_DATA_BYTES);
	m25_send_command(0x00);
	m25_send_command(0x00);
	m25_send_command(0x00);
	for(i=0/*0x1FFFFF*/; i<0x3FFFFF; ++i){//#TODO tutaj chyba od 0 jedziemy?
		tmp = m25_read_byte();
		UART_send_char(tmp);		
	}
}

bool m25_test(void){
	uint8_t tmp;
	m25_slave_select();
	m25_send_command(M25_READ_IDENTIFICATION1);
	tmp=m25_read_byte();
	if (tmp!=0x20)
		return false;
	tmp=m25_read_byte();
	if (tmp!=0x71)
		return false;
	tmp=m25_read_byte();
	if (tmp!=0x16)
		return false;

	return true;
}

static void m25_send_command(unsigned char comm){
	wait_for_spi_ready();
	SPI_send_char(comm);
}
static unsigned char m25_read_byte(void){
	wait_for_spi_ready();
	SPI_send_char(RECEIVE);
	wait_for_spi_ready();
	return spi_received_value;
}
static unsigned char m25_get_status(void){
	m25_send_command(M25_READ_STATUS_REGISTER);
	return m25_read_byte();
}
void m25_erase_chip(void){
	unsigned char tmp;	
	m25_send_command(M25_BULK_ERASE);
	m25_slave_free();
	asm("nop");
	do{
		_delay_ms(2000);
		m25_slave_select();
		tmp=m25_get_status();
		m25_slave_free();
		UART_send_char(tmp);
		
	}while (tmp&0x01);
}

