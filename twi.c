#include "twi.h"
#include "uart.h"

void twi_init(void){
	TWBR=0x0A;
	TWSR=0x00;//razem dostajemy ok 230kHz bit rate:)
	TWCR = (1<<TWEN);
}

// procedura transmisji sygnału START
void twi_start(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
}

// procedura transmisji sygnału STOP
void twi_stop(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	while ((TWCR & (1<<TWSTO)));
}

// procedura transmisji bajtu danych
void twi_write(unsigned char data)
{
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
}

//procedura odczytu bajtu danych
unsigned char twi_read(unsigned char ack)
{
	TWCR = ack ? ((1 << TWINT) | (1 << TWEN) | (1 << TWEA)) : ((1 << TWINT) | (1 << TWEN)) ;
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}

void twi_send_char(unsigned char ad, unsigned char reg, unsigned char data){
	twi_start();
	twi_write((ad<<1)&0xFE);
	twi_write(reg);
	twi_write(data);
	twi_stop();
}

unsigned char twi_receive_char(unsigned char ad, unsigned char reg){
	unsigned char data;
	//while (!(UCSRA & (1<<UDRE)));
	//UART_send_char((unsigned char)0x10);

	twi_start();
	//while (!(UCSRA & (1<<UDRE)));
	//UART_send_char((unsigned char)0x11);
	//if (twi_get_status()!=0x08){
	//	twi_stop();		
	//	return 0x80;
	//}
	twi_write((ad<<1)&0xFE);
	//while (!(UCSRA & (1<<UDRE)));
	//UART_send_char((unsigned char)0x12);
	twi_write(reg);
	//while (!(UCSRA & (1<<UDRE)));
	//UART_send_char((unsigned char)0x13);
	//twi_stop();
	twi_start();
	//while (!(UCSRA & (1<<UDRE)));
	//UART_send_char((unsigned char)0x14);
	twi_write((ad<<1)|0x01);
	//while (!(UCSRA & (1<<UDRE)));
	//UART_send_char((unsigned char)0x15);
	data = twi_read(NACK);//chyba juz nie musi byc stop, ale do przetestowania
	//while (!(UCSRA & (1<<UDRE)));
	//UART_send_char((unsigned char)0x16);
	twi_stop();
	//while (!(UCSRA & (1<<UDRE)));
	//UART_send_char((unsigned char)0x17);
	return data;
}

void twi_receive_multichar(unsigned char ad, unsigned char reg, unsigned char size, unsigned char* data){
	unsigned char i;
	twi_start();
	twi_write((ad<<1)&0xFE);
	twi_write(reg);
	twi_stop();
	twi_start();
	twi_write((ad<<1)|0x01);
	for (i=0; i<(size-1); ++i){
		data[i] = twi_read(ACK);
	}
	data[i]=twi_read(NACK);
}

unsigned char twi_get_status(void)
{
    unsigned char status;
    //mask status
    status = TWSR & 0xF8;
    return status;
}





