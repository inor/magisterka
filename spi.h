#include<avr/io.h>
//#include<util/delay.h>

void SPI_init_master(void);
void SPI_init_slave(void);
unsigned char SPI_recive_char(void);
void SPI_send_char(unsigned char z);
