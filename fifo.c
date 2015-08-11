#include "fifo.h"


void push_FIFO(unsigned char a, volatile myFIFO *f){
	f->tab[f->end]=a;
	f->end=(f->end+1)%TAB_SIZE;
}
unsigned char pop_FIFO(volatile myFIFO *f){
	unsigned char ret = f->tab[f->st];
	f->st=(f->st+1)%TAB_SIZE;
	return ret;
}
unsigned char is_empty_FIFO(volatile myFIFO *f){
	return f->st==f->end;
}
