#define BLE_OGF 0x0800
//--------------------------------------KOMENDY------------------------------------------------------

#define HCI_LE_Set_Event_Mask ((0x0001<<6)|BLE_OGF) //zwraca 0x00 jak ok reszta zlo (maski s.1052)
#define HCI_LE_Read_Buffer_Size ((0x0002<<6)|BLE_OGF) //zwraca 0x00 ok, maximum data length 0x0000 (2bajty), ile komend moze zmiescic
#define HCI_LE_Set_Random_Address ((0x0005<<6)|BLE_OGF) //
#define HCI_LE_Set_Advertising_Parameters ((0x0006<<6)|BLE_OGF)
#define HCI_LE_Read_Advertising_Channel_Tx_Power ((0x0007<<6)|BLE_OGF)
#define HCI_LE_Set_Advertising_Data ((0x0008<<6)|BLE_OGF)
#define HCI_LE_Set_Scan_Response_Data ((0x0009<<6)|BLE_OGF)
#define HCI_LE_Set_Advertise_Enable ((0x000A<<6)|BLE_OGF)
#define HCI_LE_Set_Scan_Parameters ((0x000B<<6)|BLE_OGF)
#define HCI_LE_Set_Scan_Enable ((0x000C<<6)|BLE_OGF)
#define HCI_LE_Create_Connection ((0x000D<<6)|BLE_OGF)
#define HCI_LE_Create_Connection_Cancel ((0x000E<<6)|BLE_OGF)
#define HCI_LE_Read_White_List_Size ((0x000F<<6)|BLE_OGF)
#define HCI_LE_Clear_White_List ((0x0010<<6)|BLE_OGF)
#define HCI_LE_Add_Device_To_White_List ((0x0011<<6)|BLE_OGF)
#define HCI_LE_Connection_Update ((0x0013<<6)|BLE_OGF)

//-------------------------------------------ZDARZENIA------------------------------------------------------------
#define LE_Event 0x3E
#define LE_Connection_Complete_SubEvent 0x01
#define LE_Advertising_Report_SubEvent 0x02


#define STBLC_SET_PUBLIC_ADDRESS 0x02 //Set public address
#define STBLC_SET_POWER_MODE 0x03 //Select power mode
#define STBLC_SVLD_MEASUREMENT 0x04 //Run SVLD measurement
#define STBLC_SET_RF_POWER_LEVEL 0x05 //Select TX power level
#define STBLC_POWER_MODE_CONFIGURATION 0x06 // Enable/disable transition to BLE Sleep mode
#define STBLC_SET_UART_BAUD_RATE 0x07 // Set UART baud rate
/*
Ogólnie wszystko polega na wysylaniu po prostu ramek - czesto dlugich, ale jednak tylko ramek
*/
#define SPLIT(x) (unsigned char)(x)<<8, (unsigned char)(x)
/*Komenda w ogolnosci OPCODE [zdefiniowane u gory], potem dlugosc wszystkich parametrow w oktetach(bajtach) potem parametry*/
static unsigned char set_adv_params_buf[]={	SPLIT(HCI_LE_Set_Advertising_Parameters), 0x0F, SPLIT(0x0020), SPLIT(0x4000),
						0x00, 0x00, 0x00, /* 6 bajtow ignorowanych adres bezposredniego*/, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00,0x07,0x00}; /*Cały ciąg na potrzeby wyslania ramki
						ta akurat ma wszystkie potrzebne parametry do najprostszej komunikacji*/
/*Zdecydowanie nie konieczne, mozna jechac na domyslnych ustawieniach */
static unsigned char set_adv_data_buf[]={	SPLIT(HCI_LE_Set_Advertising_Data), 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; /*Cały ciąg na potrzeby wyslania ramki
						ta akurat ma wszystkie potrzebne parametry do najprostszej komunikacji*/
static unsigned char set_adv_enable_buf[]={	SPLIT(HCI_LE_Set_Scan_Enable), 0x01, 0x01};

static unsigned char set_adv_disable_buf[]={	SPLIT(HCI_LE_Set_Scan_Enable), 0x01, 0x00};

void master_advertise(void){
//set advertising params
//set advertising data
//set advertise enable	
}

void slave_create_connection(void){
//set scan params
//set scan response data
//set scan enable
}

void add_to_white_list(void){
	//add device to white list
	//simplify connecion
}

void set_random_adress(void){
	//setting random adress for device
}



