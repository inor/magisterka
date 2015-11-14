#include "uart.h"


volatile uint8_t UART_received_data=0;
volatile bool UART_receive_flag=false;
volatile bool UART_rdy = false;


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

	UART_received_data=0;
	UART_receive_flag=false;
	UART_rdy = false;
}


void UART_send_char(uint8_t z){
	while(!UART_rdy);
	UART_rdy=false;
	UDR = z;	
}
bool UART_receive_char_irq(uint8_t * received_data){
	*received_data = UART_received_data;
	return UART_receive_flag;
}
void UART_unlock_recive_buf(){
	UART_receive_flag=false;
}


ISR(USART_RXC_vect)  //przerwanie odebrania bajtu
{
	if (UART_receive_flag==false){
		UART_received_data = UDR;
		UART_receive_flag=true;
	}

}

ISR(USART_UDRE_vect){
	UART_rdy=true;
}

