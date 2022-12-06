/*
**********************************************************
* PROJECT : HFC3100D MAIN DISPLAY
* FILE NAME: RS232.h
* DEVICE : dsPIC30F6012A
* COMPILER : MPLAB-C
* REVISION HISTORY
* 1) 2018.02.28 (V1.0)
* 
********************************************************** */

//====================================================================================//
//                          Function Declaration                                      //
//====================================================================================//
//-----------------LCD Comm(RS-232)---------------------------//
extern void Init_Uart1(void);						// for RS-232
extern void Init_Uart2(void);						// for RS-485
extern void Serial_Process(void);					// Continuous Data TX
extern void Receive_Data(void);                     // Continuous Data RX
extern void Set_Value(void);                		// Set Data TX
extern void Response_Serial(void);                	// Data Response Select
extern void Response_FB(void);                    	// Set Data Feedback RX
extern void Check_Data(void);                     	//Set Data Chek & Rety
extern void Send_Serial(unsigned int ival,unsigned char pindex,unsigned char ch);


//====================================================================================//
//                          Variable Declaration                                      //
//====================================================================================//
// RS-232 Flag
extern UINT8    Set_OK;
extern UINT8	f_SData;
extern UINT8	f_EData;
extern UINT8 	Rindex;
extern UINT8	RxBuf[34];
extern UINT8 	TxBuf[34];
extern UINT8	TxIndex;
extern FLOAT32	CSpare[10];
extern FLOAT32	SSpare[10];
extern UINT8	EOT_Flag;

// RS-232 Setting data Flag
extern UINT8    Key_Cnt;

// Main Continous Data
extern FLOAT32	BattVoltage;
extern FLOAT32	BattCurrent;
extern FLOAT32	LoadVoltage;
extern FLOAT32	LoadCurrent;

// LCD Display Command
extern FLOAT32	BattHiVoltage;
extern FLOAT32	BattLowVoltage;
extern UINT16	RestartTimeValue;

// Module Receive Command Feedback

extern FLOAT32	BattHiVoltage_FB;
extern FLOAT32	BattLowVoltage_FB;
extern UINT16	RestartTimeValue_FB;

extern UINT8	gAutoManual_FB;
extern UINT8	gSelectModule_FB;
extern UINT8	gOperationMode_FB;
extern UINT8    gSysTypeMode_FB;


// Main Set Data
extern UINT8	gAutoManual;
extern UINT8	gSelectModule;
extern UINT8	LedState[10];
extern UINT8	ModuleState[10];
extern UINT8	BatteryState[10];
extern UINT8	WarningState[10];
extern UINT8    ChargeState[10];

// RS-232 Count
extern UINT8	CheckDataCnt;
extern UINT16   tmr2Count3;


