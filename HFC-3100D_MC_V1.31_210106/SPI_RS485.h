/* 
 * Company      : Seoul Electric Power System co,ltd
 * Application  : HFC-3100D (+125Vdc)
 * File         : SPI_RS485.h
 * Author       : Martin. Kim
 * Date         : Created on 2018. 04. 18
 */

// SPI Pin Set Definition
#define SEL_SPI		(LATDbits.LATD11)//active low
#define SPI_CLK		(LATGbits.LATG6)
#define SPI_DOUT	(LATGbits.LATG8)
#define SPI_DIN		(LATGbits.LATG7)


extern void Init_SPI(void);                   // SPI Initialize for Max3100
extern void Init_Max3100(void);              // MAX3100 Chipset Initialize
extern void Serial_Process_485(void);       // Continuous Data Processing 
extern void Response_Serial_485(void);      // Data Response Select
extern void Receive_Data_485(UINT8 ID);     // Set Data Recieve RX
extern void Continuous_Send(void);          // Continuous Data TX
extern void Request_Send(void);             // Request Data TX
extern void Send_Data_485(UINT8 data);      // RS-485 Data Transfer Mode Fuction        
extern void Set485_Rx_Mode(void);           // RS-485 Data Receive Mode Fuction
extern void Delay_Cnt(UINT16 cnt);          // System Delay

extern UINT8    RxBuf_485[36];               // Recieve Buffer
extern UINT8    TxBuf_485[36];               // Trancieve Buffe

// Module #N Continous Data
extern FLOAT32  Voltage_Out[4];             // Modules Voltage Output
extern FLOAT32  Current_Out[4];             // Modules Current Output
extern FLOAT32  Temperature[4];             // Modules Temperature 
extern FLOAT32  Input_Volt_R[4];            // Modules Input Voltage R
extern FLOAT32  Input_Volt_S[4];            // Modules Input Voltage S
extern FLOAT32  Input_Volt_T[4];            // Modules Input Voltage T
extern UINT8    Rx_Char[4];                 // Modules Status
extern UINT8    Rx_Char2[4];                // Modules Warning Status
extern UINT8    Rx_Char3[4];
extern UINT8    Rx_Spare1[4];

extern bool8    Alram_Sts[5];                  // Modules Operation Status
extern bool8    Warning_Sts[5];                // Modules Operation Status
extern bool8    CHG_Info[5];                   // Module Charge type

// Module # Number
extern UINT8    ID_485;                        // Modules No.

//Operation Mode Variable
extern UINT8    OP_MODE;                       // OP_MODE => '0' : Manual / '1': Auto 
extern UINT8    OP_CMD;                        // Command Change Operation Command Module2
extern UINT8    OP_Type;                       // 0 : Stand-alone or 1 : Load Share Mode  

extern UINT16	tmr1_1;
extern UINT16	tmr1_2;
extern UINT16   tmr2_4;
extern UINT16   tmr2_5;
extern UINT16   tmr2_6;



