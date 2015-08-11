#define TAB_SIZE 50

typedef struct {
	unsigned char tab[TAB_SIZE];
	unsigned char st;
	unsigned char end;	
} myFIFO;


void push_FIFO(unsigned char a, volatile myFIFO *f);
unsigned char pop_FIFO(volatile myFIFO *f);
unsigned char is_empty_FIFO(volatile myFIFO *f);
