//BMC050 ADRESSES FOR I2C
#define ACC_ADDR_I2C 0x18
//----------------------------ACCELEROMETER----------------------------
#define READ_MULT_DATA 0x82
#define READ_ACC_X 0x82
#define READ_ACC_Y 0x84
#define READ_ACC_Z 0x86
#define READ_ACC_TEMP 0x88
#define RECEIVE 0x00
#define RANGE_SELECT 0x0F
#define RANGE_2G 0x03
#define RANGE_4G 0x05
#define RANGE_8G 0x08
#define RANGE_16G 0x0C

#define BW_SELECT 0x10
#define BW_8HZ 0x08
#define BW_16HZ 0x09
#define BW_31HZ 0x0A
#define BW_62HZ 0x0B
#define BW_125HZ 0x0C
#define BW_250HZ 0x0D
#define BW_500HZ 0x0E
#define BW_1000HZ 0x0F

#define INT_SELECT 0x17
#define NEW_DATA_INT 0x10
#define MAP_SELECT 0x1A
#define MAP_INT1 0x01
#define POLARITY_SELECT 0x20
#define POLARITY_0_ACTIVE 0x00

#define UNFILT_SELECT 0x13
#define UNFILT_DATA 0x80

#define TEST_NAME 0x80
#define ACC_SOFT_RESET_SELECT 0x14
#define ACC_SOFT_RESET 0xB6


//----------------------------MAGNETOMETER-----------------------------
#define MAG_POWER_CTRL_REG 0x4B		//przeniesienie magnetometru do sleep mode
#define MAG_POWER_CTRL_BIT_EN 0x01
#define MAG_POWER_CTRL_BIT_DIS 0x00

#define MAG_TEST_NAME_REG 0xC0	//powinno zwrocic 0x32

#define MAG_DRDY_REG 0x4E	//wlaczenie przerwania na pinie DRDY
#define MAG_DRDY_EN 0x80

#define MAG_OP_MODE_REG 0x4C	//rejestr odpowiadajacy za tryb pracy oraz datarate
				//mozna razem wybrac DR i OPMODE -> trzeba zrobic or
#define MAG_DR_10HZ	0x00<<3
#define MAG_DR_2HZ	0x01<<3
#define MAG_DR_6HZ	0x02<<3
#define MAG_DR_8HZ	0x03<<3
#define MAG_DR_15HZ	0x04<<3
#define MAG_DR_20HZ	0x05<<3
#define MAG_DR_25HZ	0x06<<3
#define MAG_DR_30HZ	0x07<<3

#define MAG_NORMAL_MODE 0x00<<1
#define MAG_FORCED_MODE 0x01<<1
#define MAG_SLEEP_MODE	0x03<<1

#define MAG_READ_MULT_DATA 0xC2 //czytanie pierwszego rejestru z danymi -> potem 8 kolejnych



void range_config(unsigned char range);
void twi_range_config(unsigned char range);

void bw_config(unsigned char bw);
void twi_bw_config(unsigned char bw);

void int_config(void);
void twi_int_config(void);

void acc_config(unsigned char range, unsigned char bw);
void twi_acc_config(unsigned char range, unsigned char bw);

void acc_test(void);
void acc_reset(void);
unsigned char twi_acc_test(void);
void twi_acc_reset(void);
unsigned char acc_int_check(void);
unsigned char twi_acc_int_check(void);
unsigned char acc_bw_check(unsigned char bw);
unsigned char twi_acc_bw_check(unsigned char bw);
unsigned char acc_range_check(unsigned char range);
unsigned char twi_acc_range_check(unsigned char range);
//unsigned char acc_get_x(void);
//unsigned char acc_get_y(void);
//unsigned char acc_get_z(void);

void mag_test(void);
void mag_test2(void);
void mag_test3(void);

