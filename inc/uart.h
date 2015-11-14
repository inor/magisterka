#include <avr/interrupt.h>
#define F_CPU 9216000UL
#define BAUD 115200
#include <util/setbaud.h>
#include "stdbool.h"
#include "stdint.h"


void UART_init(void);//funkcja inicjujaca UART
void UART_send(void);//funkcja wysylajaca zawartosc bufora z pamieci globalnej
void UART_send_char(unsigned char);//funkcja wysylająca pojedynczy znak, bez wykorzystania przerwan
//unsigned char UART_recive_char(void);//odebranie pojedynczego znaku -> funkcja blokujaca, bez obslugi przerwan
bool UART_receive_char_irq(uint8_t * received_data);
void UART_unlock_recive_buf(void);
//------------przerwania do obslugo UART-------------------------//

ISR(USART_RXC_vect);  //przerwanie odebrania bajtu

ISR(USART_UDRE_vect); //przerwanie wolnego bufora nadajnika
