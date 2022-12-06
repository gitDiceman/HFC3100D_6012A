/* 
 * File:   RS232.h
 * Author: KHJ
 *
 * Created on 2018? 2? 7? (?), ?? 4:09
 */

//-----------------LCD Comm(RS-232)---------------------------//
extern void Init_Uart1(void);					// for RS-232
extern void Init_Uart2(void);					// for RS-232
extern void Serial_Process(void);				// Continuous Data TX
extern void Response_Serial(void);				// Data Response Select
extern void Response_Data(void);
extern void Response_FB(void);					// Set Data Feedback RX
extern void Response_FB_Debug(void);			// Set Data Feedback RX
extern void Continuous_FB(void);
extern void BittoByte(void);
extern void Send_Serial(unsigned int ival,unsigned char pindex,unsigned char ch);

//-------------------------------------------------------------------------
// RS232 -Debug Variable
extern UINT8       f_SData_Debug;
extern UINT8       f_EData_Debug;
extern UINT8       EOT_Flag_Debug;
extern UINT8       Rindex_Debug;
extern UINT8       RxBuf_Debug[36];
extern UINT8       TxBuf_Debug[36];
extern UINT8       Tx_Char_Debug[12];
extern UINT8       TxIndex_Debug;


// RS-232 Flag
extern UINT8	f_SData;
extern UINT8	f_EData;
extern UINT8 	Rindex;
extern UINT8    EOT_Flag;
extern UINT8	RxBuf[36];
extern UINT8 	TxBuf[36];
extern UINT8    Tx_Char[12];
extern UINT8	TxIndex;


// Module Continuous Data
extern FLOAT32  Batt_Volt;
extern FLOAT32  Batt_Amp;
extern FLOAT32  Load_Volt;
extern FLOAT32  Load_Amp;
extern UINT8    LED_Module1_NFB;
extern UINT8    LED_Module2_NFB;
extern UINT8    LED_Module3_NFB;
extern UINT8    LED_Module4_NFB;
extern UINT8    LED_DC1_NFB;
extern UINT8    LED_DC2_NFB;
extern UINT8    LED_DC3_NFB;
extern UINT8    LED_DC4_NFB;
extern UINT8    LED_Battery_NFB;
extern UINT8    LED_Spare1_NFB;
extern UINT8    LED_Spare2_NFB;
extern UINT8    LED_Load_NFB;
extern UINT8    LED_Spare3_NFB;
extern UINT8    LED_Spare4_NFB;
extern UINT8    LED_Spare5_NFB;
extern UINT8    LED_Spare6_NFB;
extern UINT8    SystemState_STS[4];
extern UINT8    Input_OV_STS[4];
extern UINT8    Input_UV_STS[4];
extern UINT8    Input_SD_STS[4];
extern UINT8    Output_OV_STS[4];
extern UINT8    Output_UV_STS[4];
extern UINT8    Output_OC_STS[4];
extern UINT8    Output_OT_STS[4];
extern UINT8    Module_OV_STS[4];
extern UINT8    Module_UV_STS[4];
extern UINT8    Batt_OV_STS;
extern UINT8    Batt_UV_STS;
extern UINT8    Batt_Mode_STS;//0 is Charge 1 is Dis-Charge


extern UINT8	bNfbState1,bNfbState1Old;			//nfb state : on/off
extern UINT8	bNfbState2,bNfbState2Old;			//nfb state : on/off

// Module Response Data
extern FLOAT32  Set_Batt_OV;
extern FLOAT32  Set_Batt_UV;
extern FLOAT32  RST_Time;
extern FLOAT32  Spare2;
extern FLOAT32  Spare3;
extern FLOAT32  Spare4;

// Module Response backup Data
extern FLOAT32  Set_Batt_OV_B;
extern FLOAT32  Set_Batt_UV_B;
extern FLOAT32  RST_Time_B;
extern FLOAT32  Spare2_B;
extern FLOAT32  Spare3_B;
extern FLOAT32  Spare4_B;



