// ***********************************************************
// Project: ACC Data 
// Author: Konrad Neneman
// Module description:
// ***********************************************************

#include <avr\io.h>
#include "uart.h"
#include "spi.h"
#include "BMC050.h"
#include "fifo.h"
//#include <string.h>
#include <avr/sleep.h>

#define DEBUG
//definicja pozwalajaca na kompilacje z wydrukami kontrolnymi
//zakomentowac w wersji release

#define sbi(x,y) x |= _BV(y) //set bit - using bitwise OR operator
#define cbi(x,y) x &= ~(_BV(y)) //clear bit - using bitwise AND operator
#define tbi(x,y) x ^= _BV(y) //toggle bit - using bitwise XOR operator
#define is_high(x,y) (x & _BV(y) == _BV(y)) //check if the y'th bit of register 'x' is high ... test if its AND with 1 is 1

volatile unsigned char UART_received_data;
volatile unsigned char UART_receive_flag=0;
volatile unsigned char UART_rdy=0;
volatile unsigned char acc_newdata=0;
volatile unsigned char acc_ind=0;
volatile unsigned char acc_rdy=0;
volatile unsigned char mag_newdata=0;
volatile unsigned char mag_ind=0;
volatile unsigned char mag_rdy=0;
//volatile unsigned char acc_tab[7];

volatile myFIFO UART_FIFO;
//volatile myFIFO SPI_FIFO;

char check_user_button(void){
	char i=255;
	if (!(PIND&0x10)){ //sprawdz czy nacisnal przycisk
		
		while(i--);//poczekaj na koniec zaklocen
		if(!(PIND&0x10)) {//jesli dalej trzyma to poczekaj az pusci
			while(!(PIND&0x10));
			return 1;
		}
	}
	
	return 0;
	
}

void klawisz(void)  //czekaj na nacisnięcie okreslonego klawisza
{
	while(!check_user_button());
}

//----------------------------------------------PRZERWANIA------------------------------------
ISR(INT0_vect) //przerwanie od akcelerometru NEW_DATA
{
#ifdef DEBUG
	//while (!(UCSRA & (1<<UDRE)));
	//UART_send_char('I');
#endif
	cbi(PORTC,0);
	sbi(PORTC,1);
	cbi(PORTC,5);//wlacz SS dla acc, wyslij adres
	SPI_send_char(READ_MULT_DATA);
	acc_newdata=1;
	GICR=0x00;	//wylaczamy przerwanie od int0 i int1-> zeby odebrac dana w calosci bez zaklocen
	//GIFR=0xC0;	//nie czyscimy rejestru GIFR, zeby nie zgubic przerwania -> przedewszystkim od magnetometru, bo tak to malo prawdopodobne
}


ISR(INT1_vect) //przerwanie od magnetometru NEW_DATA
{
	/*cbi(PORTC,4);//wlacz SS dla mag, wyslij adres
	SPI_send_char(READ_MULT_DATA);
	acc_newdata=1;
	GICR=0x00;*/	
}

ISR(SPI_STC_vect)
{
	sbi(PORTC,0);
	cbi(PORTC,1);
	if(acc_newdata==1){
		if(acc_rdy==0) {
			SPI_send_char(RECIEVE_MULT_DATA);
			acc_rdy=1;
		}
		else{		
			push_FIFO(SPDR, &UART_FIFO);
			++acc_ind;
			if(acc_ind==7){ 
				acc_ind=0;
				acc_newdata=0;
				acc_rdy=0;
				sbi(PORTC,5);//podnosimy SS, nie zachodzi komunikacja
				GICR=0xC0;//włączamy przerwania. UWAGA, mogą być obsłużone od razu po wyjściu z przerwania!!!
			}
			else SPI_send_char(RECIEVE);		
		}
	}
		
}
ISR(TIMER0_OVF_vect) //SIGNAL(SIG_OVERFLOW0)
{
	//ustawienia do timera jak bedzie potrzebny
	/*TIMSK=0x01;//wlaczenie przerwania od  timera 0
	TIFR=0xff;//wyczyszczenie flagi przerwania moze byc 0x01
	TCCR0=0x02;//ustawiamy na 1MHz taktowanie timera -> 1 tick to 1µs*/
}
//EMPTY_INTERRUPT(TIMER1_OVF_vect);


int main(void) {
	//unsigned char i;
	set_sleep_mode(SLEEP_MODE_IDLE);
	DDRC = 0x3F;//jako wyjscia dla diod led ale przedewszystkim SS dla acc i mag. PC5 - acc; PC4 - mag
	PORTC = 0x18;//zapalamy diody, SS w stanie wysokim
	PORTD=0x10;//podciaganie do USER_BUTTON
	
	UART_FIFO.st=0;
	UART_FIFO.end=0;
	UART_init();
	SPI_init_master();

	//acc_config(0,0);

	MCUCR=0x0F;	//przerwanie generowane na narastajacym zboczu.
	GICR=0xC0;	//wlaczamy przerwanie od int0 i int1-> zeby zaczelo dzialac funkcja sei();
	GIFR=0xC0;	//czyszczenie flag przerwan od int0 i int1 -> zeby nie bylo zerowych przebiegow

  	
#ifdef DEBUG
	while (!(UCSRA & (1<<UDRE)));
	UART_send_char('S');
#endif
	//klawisz();

	//acc_config(RANGE_2G, BW_1000HZ);

	acc_test();
	while (!(UCSRA & (1<<UDRE)));
	UART_send_char(SPDR);
	klawisz();

	acc_config(RANGE_2G, BW_1000HZ);

#ifdef DEBUG
	while (!(UCSRA & (1<<UDRE)));
	UART_send_char('W');
#endif
	klawisz();
	sei();//globalne wlaczenie przerwan
	
	while(1) {
		//while (!(UCSRA & (1<<UDRE)));
		cbi(PORTC,5);
		SPI_send_char(0x82);
		while(!bit_is_set(SPSR, SPIF));
		SPI_send_char(RECIEVE);
		while(!bit_is_set(SPSR, SPIF));
		SPI_send_char(RECIEVE);
		while(!bit_is_set(SPSR, SPIF));
		sbi(PORTC,5);
		while (!(UCSRA & (1<<UDRE)));
		UART_send_char(SPDR);
		klawisz();
		
		//for(i=6;i!=0;--i) UART_send_char('A');
		/*sleep_enable();
		sleep_cpu();
		sleep_disable();*/
		
		/*if(is_empty_FIFO(&UART_FIFO)!=1 && UART_rdy!=0){
			UART_send_char(pop_FIFO(&UART_FIFO));
			UART_rdy=1;
		}*/
		/*if(is_empty_FIFO(SPI_FIFO){

		}
		if(UART_receive_flag){
			//obsluga sterowania z zewnatrz
		}*/
		//while (!(UCSRA & (1<<UDRE)));
		//UART_send_char('A');


	}

}






