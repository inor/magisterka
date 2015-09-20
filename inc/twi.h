#include <avr/io.h>

#define ACK 1
#define NACK 0

void twi_init(void);
// procedura transmisji sygnału START
void twi_start(void);
// procedura transmisji sygnału STOP
void twi_stop(void);
// procedura transmisji bajtu danych
void twi_write(unsigned char data);
//procedura odczytu bajtu danych
unsigned char twi_read(unsigned char ack);

unsigned char twi_get_status(void);

void twi_send_char(unsigned char ad, unsigned char reg, unsigned char data);
unsigned char twi_receive_char(unsigned char ad, unsigned char reg);
void twi_receive_multichar(unsigned char ad, unsigned char reg, unsigned char size, unsigned char* data);
