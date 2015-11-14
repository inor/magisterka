#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

//#include<util/delay.h>

void SPI_init_master(void);
void SPI_init_slave(void);
unsigned char SPI_recive_char(void);
void SPI_send_char(uint8_t z);
