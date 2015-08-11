#include "BMC050.h"
#include "spi.h"
#include "twi.h"
#include <avr/io.h>

#define sbi(x,y) x |= _BV(y) //set bit - using bitwise OR operator
#define cbi(x,y) x &= ~(_BV(y)) //clear bit - using bitwise AND operator
#define tbi(x,y) x ^= _BV(y) //toggle bit - using bitwise XOR operator
#define is_high(x,y) (x & _BV(y) == _BV(y)) //check if the y'th bit of register 'x' is high ... test if its AND with 1 is 1

void range_config(unsigned char range){
	//
	SPI_send_char(RANGE_SELECT);
	while(!bit_is_set(SPSR, SPIF));
	SPI_send_char(range);
	while(!bit_is_set(SPSR, SPIF));
}
void twi_range_config(unsigned char range){	
	twi_send_char(ACC_ADDR_I2C, RANGE_SELECT, range);
}

void bw_config(unsigned char bw){//uzywac stalych zdefiniowanych w BMC050.h
	SPI_send_char(BW_SELECT);
	while(!bit_is_set(SPSR, SPIF));
	SPI_send_char(bw);
	while(!bit_is_set(SPSR, SPIF));	
}
void twi_bw_config(unsigned char bw){//uzywac stalych zdefiniowanych w BMC050.h
	twi_send_char(ACC_ADDR_I2C, BW_SELECT, bw);
}

void int_config(){
	
	SPI_send_char(MAP_SELECT);
	while(!bit_is_set(SPSR, SPIF));
	SPI_send_char(MAP_INT1);
	while(!bit_is_set(SPSR, SPIF));
	sbi(PORTC,5);
	cbi(PORTC,5);
	SPI_send_char(UNFILT_SELECT);
	while(!bit_is_set(SPSR, SPIF));
	SPI_send_char(UNFILT_DATA);
	while(!bit_is_set(SPSR, SPIF));
	sbi(PORTC,5);
	cbi(PORTC,5);
	SPI_send_char(POLARITY_SELECT);
	while(!bit_is_set(SPSR, SPIF));
	SPI_send_char(POLARITY_0_ACTIVE);
	while(!bit_is_set(SPSR, SPIF));
	sbi(PORTC,5);
	cbi(PORTC,5);
	SPI_send_char(INT_SELECT);
	while(!bit_is_set(SPSR, SPIF));
	SPI_send_char(NEW_DATA_INT);
	while(!bit_is_set(SPSR, SPIF));
	sbi(PORTC,5);
	//cbi(PORTC,5);
}
void twi_int_config(){
	twi_send_char(ACC_ADDR_I2C, MAP_SELECT, MAP_INT1);
	twi_send_char(ACC_ADDR_I2C, UNFILT_SELECT, UNFILT_DATA);
	twi_send_char(ACC_ADDR_I2C, POLARITY_SELECT, POLARITY_0_ACTIVE);
	twi_send_char(ACC_ADDR_I2C, INT_SELECT, NEW_DATA_INT);
}
void acc_config(unsigned char range, unsigned char bw){
	char tmp;
	if(range!=0) {
		do{
			cbi(PORTC,5);
			range_config(range); //jesli range=0 pozostawiamy wartosc domyslna (16g)
			sbi(PORTC,5);
		}
		while(acc_range_check(range));
	}
	if(bw!=0){ //jesli bw=0 pozostawiamy wartosc domyslna (1000Hz)
		do{
			cbi(PORTC,5);
			bw_config(bw);
			sbi(PORTC,5);
		}
		while(acc_bw_check(bw));//powtarzamy, az poprawnie zaincjalizujemy czujnik
	}
	do{
		cbi(PORTC,5);
		int_config();
		sbi(PORTC,5);
		tmp=acc_int_check();
		sbi(PORTC,5);
	}
	while(tmp);

}
void twi_acc_config(unsigned char range, unsigned char bw){
	unsigned char tmp;
	if(range!=0) {
		do{
			
			twi_range_config(range); //jesli range=0 pozostawiamy wartosc domyslna (16g)
		}
		while(twi_acc_range_check(range));
	}
	if(bw!=0){ //jesli bw=0 pozostawiamy wartosc domyslna (1000Hz)
		do{
			twi_bw_config(bw);
		}
		while(twi_acc_bw_check(bw));//powtarzamy, az poprawnie zaincjalizujemy czujnik
	}
	do{
		twi_int_config();
		tmp=twi_acc_int_check();
	}
	while(tmp);
}

void acc_test(){
	cbi(PORTC,5);
	SPI_send_char(TEST_NAME);
	while(!bit_is_set(SPSR, SPIF));
	SPI_send_char(RECEIVE);
	while(!bit_is_set(SPSR, SPIF));
	sbi(PORTC,5);
}
unsigned char twi_acc_test(){
	unsigned char tmp;
	tmp = twi_receive_char(ACC_ADDR_I2C, 0x00);
	return tmp;
}
void acc_reset(){
	cbi(PORTC,5);
	SPI_send_char(ACC_SOFT_RESET_SELECT);
	while(!bit_is_set(SPSR, SPIF));
	SPI_send_char(ACC_SOFT_RESET);
	while(!bit_is_set(SPSR, SPIF));
	sbi(PORTC,5);
}
void twi_acc_reset(){
	twi_send_char(ACC_ADDR_I2C, ACC_SOFT_RESET_SELECT, ACC_SOFT_RESET); 
}

unsigned char acc_int_check(){ //sprawdzenie czy wszystkie rejestry zostaly poprawnie zainicjowane
	
	unsigned char tmp;//, ret=0;
	
	cbi(PORTC,5);

	SPI_send_char(MAP_SELECT|0x80);
	while(!bit_is_set(SPSR, SPIF));
	SPI_send_char(RECEIVE);
	while(!bit_is_set(SPSR, SPIF));
	tmp=SPDR;
	if((tmp&0x81)!=MAP_INT1) return 1;
	
	sbi(PORTC,5);
	cbi(PORTC,5);

	SPI_send_char(UNFILT_SELECT|0x80);
	while(!bit_is_set(SPSR, SPIF));
	SPI_send_char(RECEIVE);
	while(!bit_is_set(SPSR, SPIF));
	tmp=SPDR;
	if((tmp&0xC0)!=UNFILT_DATA) return (tmp|0x03);	

	sbi(PORTC,5);
	cbi(PORTC,5);

	SPI_send_char(POLARITY_SELECT|0x80);
	while(!bit_is_set(SPSR, SPIF));
	SPI_send_char(RECEIVE);
	while(!bit_is_set(SPSR, SPIF));
	tmp=SPDR;
	if((tmp&0x0F)!=POLARITY_0_ACTIVE) return (tmp|0xF0);
	
	sbi(PORTC,5);
	cbi(PORTC,5);

	SPI_send_char(INT_SELECT|0x80);
	while(!bit_is_set(SPSR, SPIF));
	SPI_send_char(RECEIVE);
	while(!bit_is_set(SPSR, SPIF));
	tmp=SPDR;
	if((tmp&0x1F)!=NEW_DATA_INT) return (tmp|0xC0);

	

	return 0;
}

unsigned char twi_acc_int_check(){
	unsigned char tmp;
	tmp=twi_receive_char(ACC_ADDR_I2C, MAP_SELECT);
	if((tmp&0x81)!=MAP_INT1) return 1;
	tmp=twi_receive_char(ACC_ADDR_I2C, UNFILT_SELECT);
	if((tmp&0xC0)!=UNFILT_DATA) return (tmp|0x03);
	tmp=twi_receive_char(ACC_ADDR_I2C, POLARITY_SELECT);
	if((tmp&0x0F)!=POLARITY_0_ACTIVE) return (tmp|0xF0);
	tmp=twi_receive_char(ACC_ADDR_I2C, INT_SELECT);
	if((tmp&0x1F)!=NEW_DATA_INT) return (tmp|0xC0);
	
	return 0;
}

unsigned char acc_bw_check(unsigned char bw){
	cbi(PORTC,5);
	SPI_send_char(BW_SELECT|0x80);
	while(!bit_is_set(SPSR, SPIF));
	SPI_send_char(RECEIVE);
	while(!bit_is_set(SPSR, SPIF));
	sbi(PORTC,5);
	if((SPDR&0x1F)!=bw) return 1;
	return 0;
}

unsigned char twi_acc_bw_check(unsigned char bw){

	unsigned char tmp;
	tmp=twi_receive_char(ACC_ADDR_I2C, BW_SELECT);
	if((tmp&0x1F)!=bw) return 1;
	return 0;
}

unsigned char acc_range_check(unsigned char range){
	cbi(PORTC,5);	
	SPI_send_char(RANGE_SELECT|0x80);
	while(!bit_is_set(SPSR, SPIF));
	SPI_send_char(RECEIVE);
	while(!bit_is_set(SPSR, SPIF));
	sbi(PORTC,5);
	if((SPDR&0x0F)!=range) return 1;
	return 0;
}

unsigned char twi_acc_range_check(unsigned char range){
	unsigned char tmp;
	tmp=twi_receive_char(ACC_ADDR_I2C, RANGE_SELECT);
	if((tmp&0x0F)!=range) return 1;
	return 0;
}

/*unsigned char acc_get_x(){//funkcje blokujace
	cbi(PORTC,5);
	SPI_send_char(TEST_NAME);
	while(!bit_is_set(SPSR, SPIF));
	SPI_send_char(RECEIVE);
	while(!bit_is_set(SPSR, SPIF));
	sbi(PORTC,5);
}

unsigned char acc_get_y(){

}

unsigned char acc_get_z(){

}*/

void mag_test(){
	cbi(PORTC,4);
	SPI_send_char(0x80|MAG_POWER_CTRL_REG);
	while(!bit_is_set(SPSR, SPIF));
	SPI_send_char(RECEIVE);
	while(!bit_is_set(SPSR, SPIF));
	/*SPI_send_char(MAG_POWER_CTRL_BIT_EN);
	while(!bit_is_set(SPSR, SPIF));
	SPI_send_char(MAG_TEST_NAME_REG);
	while(!bit_is_set(SPSR, SPIF));
	SPI_send_char(RECEIVE);
	while(!bit_is_set(SPSR, SPIF));*/
	sbi(PORTC,4);
}
void mag_test2(){
	cbi(PORTC,4);
	SPI_send_char(MAG_POWER_CTRL_REG);
	while(!bit_is_set(SPSR, SPIF));
	SPI_send_char(MAG_POWER_CTRL_BIT_EN);
	while(!bit_is_set(SPSR, SPIF));
	sbi(PORTC,4);
	asm("nop");
	asm("nop");
	asm("nop");
	cbi(PORTC,4);
	SPI_send_char(0x80|MAG_POWER_CTRL_REG);
	while(!bit_is_set(SPSR, SPIF));
	SPI_send_char(RECEIVE);
	while(!bit_is_set(SPSR, SPIF));
	sbi(PORTC,4);
}
void mag_test3(){
	cbi(PORTC,4);
	SPI_send_char(MAG_TEST_NAME_REG);
	while(!bit_is_set(SPSR, SPIF));
	SPI_send_char(RECEIVE);
	while(!bit_is_set(SPSR, SPIF));
	sbi(PORTC,4);
}



