#include "uart.h"



//extern volatile unsigned char UART_received_data;
//extern volatile unsigned char UART_receive_flag;
extern volatile unsigned char UART_rdy;


void UART_init(void){ //funkcja inicjujaca UART

	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;

	//UBRRH = 0;	//makra z setbaud.h
	//UBRRL = 12;	//
	#if USE_2X  		//
		UCSRA |=  (1<<U2X);//
	#else			//	
		UCSRA &= ~(1<<U2X);//
	#endif			//

	//dalsza konfiguracja
	UCSRB=(1<<UDRIE)|(1<<RXCIE)|(1<<RXEN)|(1<<TXEN); 	// wlaczamy: przerwanie od odebrania danej,
						// odbiornik i nadajnik UART
	//UCSRB=(1<<RXEN)|(1<<TXEN);//odbiornik smiga caly czas, nadajnik tylko jak potrzeba

	UCSRC=(1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0)|(1<<UPM1);
	// bity danych:8
	// bity stopu:1
	// bit parzystosci: even
}


void UART_send_char(unsigned char z){
	while(!UART_rdy);
	UDR = z;
	UART_rdy=0;
}


ISR(USART_RXC_vect)  //przerwanie odebrania bajtu
{
/*	unsigned char tmp = UDR;
	//UART_received_data = UDR;	//zapamiętaj odebraną liczbę
	if (tmp>='0' && tmp<='9'){
		UART_receive_flag = 1;		//ustaw flagę odbioru liczby dla main()
		UART_received_data =tmp;
	}
*/
}

ISR(USART_UDRE_vect){
	UART_rdy=1;
}

