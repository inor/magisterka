#include <avr\interrupt.h>
#define F_CPU 9216000
#define BAUD 115200
#include <util/setbaud.h>


void UART_init(void);//funkcja inicjujaca UART
void UART_send(void);//funkcja wysylajaca zawartosc bufora z pamieci globalnej
void UART_send_char(unsigned char);//funkcja wysylająca pojedynczy znak, bez wykorzystania przerwan
unsigned char UART_recive_char(void);//odebranie pojedynczego znaku -> funkcja blokujaca, bez obslugi przerwan
//------------przerwania do obslugo UART-------------------------//

ISR(USART_RXC_vect);  //przerwanie odebrania bajtu

ISR(USART_UDRE_vect); //przerwanie wolnego bufora nadajnika
