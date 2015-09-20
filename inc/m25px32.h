#define	M25_WRITE_ENABLE		0x06
#define	M25_WRITE_DISABLE		0x04
#define M25_READ_STATUS_REGISTER	0x05
#define	M25_WRITE_STATUS_REGISTER	0x01
#define	M25_READ_DATA_BYTES		0x03
#define M25_PAGE_PROGRAM		0x02
#define	M25_SUBSECTOR_ERASE		0x20
#define	M25_SECTOR_ERASE		0xD8
#define M25_BULK_ERASE			0xC7
#define	M25_DEEP_POWER_DOWN		0xB9
#define	M25_RELEASE_DEEP_POWER_DOWN	0xAB
#define	M25_READ_IDENTIFICATION1	0x9F
#define	M25_READ_IDENTIFICATION2	0x9E

#define M25_MAX_PAGE_ADDRESS		0x3FFFUL
#define M25_PAGE_SIZE			256



typedef struct flash_state{
	unsigned char cell_index;
	unsigned int page_index;
} flash_state, * flash_state_p;

void m25_init(void);
void m25_prepare_to_start(void);
void m25_save_byte(unsigned char byte);
void m25_save_6bytes(unsigned char* bytes);
void m25_load_all_data(void);


unsigned char m25_read_byte(void);
//void m25_write_enable(void);
void m25_start_writing_page(unsigned int addr);
void m25_write_byte(unsigned char data);
unsigned char m25_get_status(void);
void m25_send_command(unsigned char comm);
void m25_choose_address(unsigned long addr);
void m25_erase_chip(void);
flash_state_p m25_get_state(void);

