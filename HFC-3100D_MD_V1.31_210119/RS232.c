/*
**********************************************************
* PROJECT : HFC3100D MAIN DISPLAY
* FILE NAME: RS232.c
* DEVICE : dsPIC30F6012A
* COMPILER : MPLAB-C
* REVISION HISTORY
* 1) 2018.02.28 (V1.0)
* 
********************************************************** */


#include    <p30F6012A.h>
#include    "Definition.h"
#include    "RS232.h"

//-----------------LCD Comm(RS-232)---------------------------//
void Init_Uart1(void);					// for RS-232
void Init_Uart2(void);					// for RS-485
void Serial_Process(void);				// Continuous Data TX
void Receive_Data(void);				// Continuous Data RX
void Set_Value(void);					// Set Data TX
void Response_Serial(void);				// Data Response Select
void Response_FB(void);					// Set Data Feedback RX
void Check_Data(void);					//Set Data Chek & Rety
void Send_Serial(unsigned int ival,unsigned char pindex,unsigned char ch);

// RS232 Variable
UINT8       f_SData;
UINT8       f_EData;
UINT8       Rindex=0;
UINT8       RxBuf[34];
UINT8       TxBuf[34];
UINT8       TxIndex=0;
UINT8       Set_OK = 0;
FLOAT32		CSpare[10]={0,};
FLOAT32		SSpare[10]={0,};
UINT8		EOT_Flag=0;
// Main Continous Data
FLOAT32     BattVoltage = 0;
FLOAT32     BattCurrent = 0;
FLOAT32     LoadVoltage = 0;
FLOAT32     LoadCurrent = 0;

// Main Set Data
FLOAT32	BattHiVoltage=0;
FLOAT32	BattLowVoltage=0;
UINT16	RestartTimeValue=0;

// Module Receive Command Feedback

FLOAT32	BattHiVoltage_FB=0;
FLOAT32	BattLowVoltage_FB=0;
UINT16	RestartTimeValue_FB=0;

UINT8	gAutoManual_FB;
UINT8	gSelectModule_FB;
UINT8	gOperationMode_FB;
UINT8   gSysTypeMode_FB;

//UINT8	gRestartTime;
UINT8	gAutoManual=0;
UINT8	gSelectModule=0;
UINT8	gOperationMode=0;
UINT8	gBattTypeMode=0;
UINT8	gSysTypeMode=0;
UINT8   gInVoltMode=0;
UINT8	LedState[10]={0x00,};
UINT8	ModuleState[10]={0x00,};
UINT8	BatteryState[10]={0x00,};
UINT8	WarningState[10]={0x00,};
UINT8   ChargeState[10]={0x00,};

// RS-232 Count
UINT8	CheckDataCnt=0;

void Init_Uart1(void)
{
	TRISFbits.TRISF3 = 0;		// I/O Setting UART TX
	TRISFbits.TRISF2 = 1;		// I/O Setting UART RX
	
	U1MODEbits.STSEL=0;			//1 stop bit
	U1MODEbits.PDSEL=0;			//no parity, 8 data bits
	U1MODEbits.ABAUD=0;			//disabled Auto-Baud
	U1BRG=U1BRGVAL;				//BRGVAL
	U1STAbits.URXISEL=0;		//Interrupt after one RX character is received;
	U1STAbits.UTXISEL=0;
	IFS0bits.U1RXIF=0;			//Clear Receive flag 
	IEC0bits.U1RXIE=1;			//Enable Receive Interrupt
	
	U1MODEbits.UARTEN=1;		//Enable UART
	U1STAbits.UTXEN=1;			//Enable Transmit
}

void Init_Uart2(void)
{
	TRISFbits.TRISF5 = 0;		// I/O Setting UART TX
	TRISFbits.TRISF4 = 1;		// I/O Setting UART RX
	
	U2MODEbits.STSEL=0;			//1 stop bit
	U2MODEbits.PDSEL=0;			//no parity, 8 data bits
	U2MODEbits.ABAUD=0;			//disabled Auto-Baud
	U2BRG=U2BRGVAL;				//BRGVAL
	U2STAbits.URXISEL=0;		//Interrupt after one RX character is received;
	U2STAbits.UTXISEL=0;
	IFS1bits.U2RXIF=0;			//Clear Receive flag 
	IEC1bits.U2RXIE=1;			//Enable Receive Interrupt
	
	U2MODEbits.UARTEN=1;		//Enable UART
	U2STAbits.UTXEN=1;			//Enable Transmit
}

// '<' + 'U'+ '0' + 'S' + TxBuf[4~27] + 
void Response_Serial(void)
{
	switch(RxBuf[1])                            //Unit Select
	{
        case 'U':
            switch(RxBuf[2])                    //Module ID Select          
            {                
				case '0':                       //Module 1
					switch(RxBuf[3])            //Ctrl Code Select
					{
						case 'R':               //Setting Data Set Start
						Response_FB();          //Module Response Data Recieve
						Check_Data();
						break;                  //Setting Data Set Start END
						
						case 'C':
						Receive_Data();           //LCD Continues Data Recieve
						break;
					  
						default:
						break;
					}                           //Ctrl Code Select
				break;                          //Module 1
                               
				default:
				break;
			}
		break;
        
		default:
		break;
	}
	f_SData = 0;
	f_EData = 0;
    EOT_Flag = 0;
}

void Check_Data(void)
{
	if(CheckDataCnt < 3)
	{
		CheckDataCnt++;
		if(BattHiVoltage != BattHiVoltage_FB || BattLowVoltage != BattLowVoltage_FB || RestartTimeValue != RestartTimeValue_FB)
		{
			Set_OK = 1;
		}
	}
	else	CheckDataCnt=0;
}

void Response_FB(void)
{
    int tmpvalue = 0;
    unsigned char a,b,c,d;
    // Battery Hi Voltage Setting Feedback
    a = RxBuf[4];
    b = RxBuf[5];
    c = RxBuf[6];
    d = RxBuf[7];
    if(a >= '0' && a <= '9')
    {
        if(b >= '0' && b <= '9')
        {
            if(c >= '0' && c <= '9')
            {
                if(d >= '0' && d <= '9')
                {
                    tmpvalue = (a-0x30)*1000 + (b-0x30)*100  + (c-0x30)*10 + (d-0x30)*1;		
                    BattHiVoltage_FB = tmpvalue * 0.1;  //  xxxx -> xxx.x
                }
            }
        }		
    }                          
                               
    // Battery Low Voltage Setting Feedback
    a = RxBuf[8];
    b = RxBuf[9];
    c = RxBuf[10];
    d = RxBuf[11];
    if(a >= '0' && a <= '9')
    {
        if(b >= '0' && b <= '9')
        {
            if(c >= '0' && c <= '9')
            {
                if(d >= '0' && d <= '9')
                {
                    tmpvalue = (a-0x30)*1000 + (b-0x30)*100  + (c-0x30)*10 + (d-0x30)*1;
                    BattLowVoltage_FB = tmpvalue * 0.1;    //  xxxx -> xxx.x
                }
            }
        }
    }

    // Restart Time Setting Feedback
    a = RxBuf[12];
    b = RxBuf[13];
    c = RxBuf[14];
    d = RxBuf[15];
    if(a >= '0' && a <= '9')
    {
        if(b >= '0' && b <= '9')
        {
            if(c >= '0' && c <= '9')
            {
                if(d >= '0' && d <= '9')
                {
                    tmpvalue = (a-0x30)*1000 + (b-0x30)*100  + (c-0x30)*10 + (d-0x30)*1;
                    RestartTimeValue_FB = tmpvalue * 0.1;      //  xxxx -> xxx.x
                }
            }
        }		
    }
    // Spare1 Feedback
    a = RxBuf[16];
    b = RxBuf[17];
    c = RxBuf[18];
    d = RxBuf[19];
    if(a >= '0' && a <= '9')
    {
        if(b >= '0' && b <= '9')
        {
            if(c >= '0' && c <= '9')
            {
                if(d >= '0' && d <= '9')
                {
                    tmpvalue = (a-0x30)*1000 + (b-0x30)*100  + (c-0x30)*10 + (d-0x30)*1;
                    //Spare1 = tmpvalue * 0.1;      //xxxx -> xxx.x
                }
            }
        }		
    }
    // Spare2 Feedback
    a = RxBuf[20];
    b = RxBuf[21];
    c = RxBuf[22];
    d = RxBuf[23];
    if(a >= '0' && a <= '9')
    {
        if(b >= '0' && b <= '9')
        {
            if(c >= '0' && c <= '9')
            {
                if(d >= '0' && d <= '9')
                {
                    tmpvalue = (a-0x30)*1000 + (b-0x30)*100  + (c-0x30)*10 + (d-0x30)*1;	
                    //Spare2 = tmpvalue * 0.1;        //xxxx -> xxx.x
                }
            }
        }		
    }
    // Spare3 Feedback
    a = RxBuf[24];
    b = RxBuf[25];
    c = RxBuf[26];
    d = RxBuf[27];
    if(a >= '0' && a <= '9')
    {
        if(b >= '0' && b <= '9')
        {
            if(c >= '0' && c <= '9')
            {
                if(d >= '0' && d <= '9')
                {
                    tmpvalue = (a-0x30)*1000 + (b-0x30)*100  + (c-0x30)*10 + (d-0x30)*1;	
                    //Spare3 = tmpvalue * 0.1;        //xxxx -> xxx.x
                }
            }
        }
    }
    
    // Mode, Module Select Feedback
    a = RxBuf[28];
    b = RxBuf[29];
    c = RxBuf[30];
    d = RxBuf[31];
	gAutoManual_FB = a-0x30;
	gSelectModule_FB = b-0x30;
    gOperationMode_FB = c-0x30;
    gSysTypeMode_FB = d-0x30;
}

void Set_Value()
{
    UINT8 i = 0,checksum=0;
    INT16 tmpvalue = 0;
    tmpvalue = BattHiVoltage * 10;
    Send_Serial(tmpvalue,1,'S');
    tmpvalue = BattLowVoltage * 10;
    Send_Serial(tmpvalue,2,'S');
    tmpvalue = RestartTimeValue * 10;
    Send_Serial(tmpvalue,3,'S');
    tmpvalue = SSpare[0] * 10;
    Send_Serial(tmpvalue,4,'S');
    tmpvalue = SSpare[1] * 10;
    Send_Serial(tmpvalue,5,'S');
    tmpvalue = SSpare[2] * 10;
    Send_Serial(tmpvalue,6,'S');
    //tmpvalue = SSpare[4] * 10;
    //Send_Serial(tmpvalue,7,'S');

    TxBuf[0] = '[';                      //Start of Transmission Packet - SOT => 0x5B '['
    TxBuf[1] = 'D';                      //Display Unit      => 0x44 'D'
    TxBuf[2] = '0';                      //Module ID data   => 0
    TxBuf[3] = 'S';                      //Data Type => Response => 0x53 'S'
	
	TxBuf[28] = gAutoManual+0x30;
	TxBuf[29] = gSelectModule+0x30;
	TxBuf[30] = gOperationMode+0x30;
	TxBuf[31] = gSysTypeMode+0x30;      
	
	for(i=0;i<32;i++)
	{
		checksum^=TxBuf[i];
	}
	TxBuf[32] = (checksum>>4)+0x30;
	TxBuf[33] = (checksum&0x0F)+0x30;
	TxBuf[34] = ']';					//End of Transmission Packet - EOT => 0x5D ']'
    for(i=0; i<35; i++)                 //Data Packet 28Byte
    {
        U1TXREG = TxBuf[i];
        while(!U1STAbits.TRMT);
    }
}

void Receive_Data(void)
{
    int tmpvalue = 0;
    unsigned char a,b,c,d;
    // Voltage Output
    a = RxBuf[4];
    b = RxBuf[5];
    c = RxBuf[6];
    d = RxBuf[7];
    if(a >= '0' && a <= '9')
    {
        if(b >= '0' && b <= '9')
        {
            if(c >= '0' && c <= '9')
            {
                if(d >= '0' && d <= '9')
                {
                    tmpvalue = (a-0x30)*1000 + (b-0x30)*100  + (c-0x30)*10 + (d-0x30)*1;		
                    BattVoltage = tmpvalue * 0.1;  //  xxxx -> xxx.x
                }
            }
        }		
    }                          
                               
    // Current Output
    a = RxBuf[8];
    b = RxBuf[9];
    c = RxBuf[10];
    d = RxBuf[11];
    if(a >= '0' && a <= '9')
    {
        if(b >= '0' && b <= '9')
        {
            if(c >= '0' && c <= '9')
            {
                if(d >= '0' && d <= '9')
                {
                    tmpvalue = (a-0x30)*1000 + (b-0x30)*100  + (c-0x30)*10 + (d-0x30)*1;
                    BattCurrent = tmpvalue * 0.1;  //  xxxx -> xxx.x
                }
            }
        }
    }

    // Temperature
    a = RxBuf[12];
    b = RxBuf[13];
    c = RxBuf[14];
    d = RxBuf[15];
    if(a >= '0' && a <= '9')
    {
        if(b >= '0' && b <= '9')
        {
            if(c >= '0' && c <= '9')
            {
                if(d >= '0' && d <= '9')
                {
                    tmpvalue = (a-0x30)*1000 + (b-0x30)*100  + (c-0x30)*10 + (d-0x30)*1;
                    LoadVoltage = tmpvalue * 0.1;  //  xxxx -> xxx.x
                }
            }
        }		
    }
	
    a = RxBuf[16];
    b = RxBuf[17];
    c = RxBuf[18];
    d = RxBuf[19];
    if(a >= '0' && a <= '9')
    {
        if(b >= '0' && b <= '9')
        {
            if(c >= '0' && c <= '9')
            {
                if(d >= '0' && d <= '9')
                {
                    tmpvalue = (a-0x30)*1000 + (b-0x30)*100  + (c-0x30)*10 + (d-0x30)*1;		
                    LoadCurrent = tmpvalue * 0.1;  //  xxxx -> xxx.x
                }
            }
        }		
    }
	//Module NFB LED Status
    LedState[0] = RxBuf[20];
    LedState[1] = RxBuf[21];
    LedState[2] = RxBuf[22];
    LedState[3] = RxBuf[23];
    ModuleState[0] = RxBuf[24];		//#1 Module Status
	ModuleState[1] = RxBuf[25];		//#2 Module Status
	ModuleState[2] = RxBuf[26];		//#3 Module Status
	ModuleState[3] = RxBuf[27];		//#4 Module Status
	BatteryState[0] = RxBuf[28];	//Battery Status bit3 => 0:Charge 1:Discharge
	WarningState[0] = RxBuf[29];	//Warning stats
    ChargeState[0] = RxBuf[30];     //Charge stats
	//ModuleState[7] = RxBuf[31];	//Spare3
}


//                                              Module -> Module LCD Transmit-KHJ                                    //
//===================================================================================================================//
//                                       Continuous Data Protocol Packet                                             //
//  '<' + 'U'+ '0' + 'C' + TxBuf[0~7](Vlotage[0~3] / Ampere[4~7]) / Temperature[8~11]                  //   
//  Input_Volt_R[12~15] / Input_Volt_S[16~19] / Input_Volt_S[20~23]) / STS[24] / SP1[25] / SP2[26] / SP2[27] + '>'   //
//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                        Response Data Protocol Packet                                                                       //
// '<' + 'U'+ '0' + 'R' + TxBuf[0~19](C_Ref[0~3] / OC_Limt[4~7] / V_Ref[8~11] / UV_Limit[12~15] / OV_Limit[16~19]) + '>'      //
//============================================================================================================================================//
void Send_Serial(unsigned int ival,unsigned char pindex,unsigned char ch)
{
	unsigned int tmpval;
	unsigned char a,b,c,d,e;
	//==================================================================// 
	//TxBuf[0~7] ===> 'C' [0~3] / [4~7]									//
	//TxBuf[0~19] ===> 'R' [1~3] / [4~7] / [8~11] / [12~15] / [16~19]	//
	//==================================================================//
	tmpval = ival;
	//
	a=__builtin_divud((long)tmpval,10000);
	b=__builtin_divud((long)(tmpval - (__builtin_muluu(a,10000)) ),1000);
	c=__builtin_divud((long)(tmpval - (__builtin_muluu(a,10000) + __builtin_muluu(b,1000)) ),100);
	d=__builtin_divud((long)(tmpval - (__builtin_muluu(a,10000) + __builtin_muluu(b,1000) + __builtin_muluu(c,100)) ),10);
	e= tmpval%10;
	//
    switch(ch)		//KHJ - RS-232 & RS-485
    {
        case 'C':	//Tx Type -Continuous
            switch(pindex)
            {
                case 1:
                    TxBuf[4]= b + 0x30;                 //Spare1
                    TxBuf[5]= c + 0x30;
                    TxBuf[6]= d + 0x30;
                    TxBuf[7]= e + 0x30;
                break;
                        //
                case 2:
                    TxBuf[8]= b + 0x30;                 //Spare2
                    TxBuf[9]= c + 0x30;
                    TxBuf[10]= d + 0x30;
                    TxBuf[11]= e + 0x30;
                break;
                
                case 3:
                    TxBuf[12]= b + 0x30;                 //Spare3
                    TxBuf[13]= c + 0x30;
                    TxBuf[14]= d + 0x30;
                    TxBuf[15]= e + 0x30;
                break;
                
                case 4:
                    TxBuf[16]= b + 0x30;                 //Spare4
                    TxBuf[17]= c + 0x30;
                    TxBuf[18]= d + 0x30;
                    TxBuf[19]= e + 0x30;
                break;
                
                case 5:
                    TxBuf[20]= b + 0x30;                 //Spare5
                    TxBuf[21]= c + 0x30;
                    TxBuf[22]= d + 0x30;
                    TxBuf[23]= e + 0x30;
                break;
                
                case 6:
                    TxBuf[24]= b + 0x30;                 //Spare6
                    TxBuf[25]= c + 0x30;
                    TxBuf[26]= d + 0x30;
                    TxBuf[27]= e + 0x30;
                break;
				
				case 7:
                    TxBuf[28]= b + 0x30;                 //Spare7
                    TxBuf[29]= c + 0x30;
                    TxBuf[30]= d + 0x30;
                    TxBuf[31]= e + 0x30;
                break;

                default:
                break;
            }
        break; 
        
        case 'S':		//Tx Type - Request
            switch(pindex)
            {
                case 1:
                    TxBuf[4]= b + 0x30;                 //Set_ARef/1000 => 1000
                    TxBuf[5]= c + 0x30;                 //Set_ARef/100 => 100
                    TxBuf[6]= d + 0x30;                 //Set_ARef/10  => 10
                    TxBuf[7]= e + 0x30;                 //Set_ARef%1  => 1
                break;
                //
                case 2:
                    TxBuf[8]= b + 0x30;                 //Set_CL/1000 => 1000
                    TxBuf[9]= c + 0x30;                 //Set_CL/100 => 100
                    TxBuf[10]= d + 0x30;                 //Set_CL/10  => 10
                    TxBuf[11]= e + 0x30;                 //Set_CL%1  => 1
                break;
                
                case 3:
                    TxBuf[12]= b + 0x30;                 //Set_VRef/1000 => 1000
                    TxBuf[13]= c + 0x30;                 //Set_VRef/100 => 100
                    TxBuf[14]= d + 0x30;                //Set_VRef/10  => 10
                    TxBuf[15]= e + 0x30;                //Set_VRef%1  => 1
                break;
                //
                case 4:
                    TxBuf[16]= b + 0x30;                 //Set_UV/1000 => 1000
                    TxBuf[17]= c + 0x30;                 //Set_UV/100 => 100
                    TxBuf[18]= d + 0x30;                 //Set_UV/10  => 10
                    TxBuf[19]= e + 0x30;                 //Set_UV%1  => 1
                break;
                
                case 5:
                    TxBuf[20]= b + 0x30;                 //Set_OV/1000 => 1000
                    TxBuf[21]= c + 0x30;                 //Set_OV/100 => 100
                    TxBuf[22]= d + 0x30;                 //Set_OV/10  => 10
                    TxBuf[23]= e + 0x30;                 //Set_OV%1  => 1
                break;
                
                case 6:
                    TxBuf[24]= b + 0x30;                 //Set_OT/1000 => 1000
                    TxBuf[25]= c + 0x30;                 //Set_OT/100 => 100
                    TxBuf[26]= d + 0x30;                 //Set_OT/10  => 10
                    TxBuf[27]= e + 0x30;                 //Set_OT%1  => 1
                break;
                
                case 7:
                    TxBuf[28]= b + 0x30;                 //Spare
                    TxBuf[29]= c + 0x30;                 //Spare
                    TxBuf[30]= d + 0x30;                 //Spare
                    TxBuf[31]= e + 0x30;                 //Spare
                break;
                   
                default:
                break;
                
            }
        break; 
     
        default:
        break;
    }

}

void Serial_Process(void)
{
    unsigned char i=0,checksum=0;
    int tmpvalue=0;
    
    if(f_SData && f_EData)
	{
		Response_Serial();
	}
    else
    {
        //-------------------------------------------------------------//
        //                  Control Request Data Send                  //
        //-------------------------------------------------------------//
        if(tmr2Count3 >= RS232_Period)				//elapse time about 500ms~750ms
        {
            tmr2Count3 = 0;
            //================================================================//
            //                 Continuous Data Protocol Packet                //
            //           '[' + 'U'+ '0' + 'C' + TxBuf[0~24]+ SP4 + ']'        // 
            //================================================================//
            if(Set_OK == 1)
            {
                if(Key_Cnt == 0)
                {
                    Key_Cnt = 1;
                    Set_Value();
                    Set_OK = 0; 
                }
                else
                {
                    Set_OK = 0;
                }
            }
            //----------------------------------------------------------------//
            //              Metering & Alarm Data Transmit-KHJ                //
            //----------------------------------------------------------------//  
            else if(Set_OK == 0)
            {    
                Key_Cnt = 0;
                ///////////////////////////////////////////
                //          Metering data(24byte)         //
                ///////////////////////////////////////////   
                tmpvalue = CSpare[0]*10;             //Spare1
                Send_Serial(tmpvalue,1,'C');

                tmpvalue = CSpare[1]*10;             //Spare2
                Send_Serial(tmpvalue,2,'C');

                tmpvalue = CSpare[2]*10;             //Spare3
                Send_Serial(tmpvalue,3,'C');                 

                tmpvalue = CSpare[3]*10;             //Spare4
                Send_Serial(tmpvalue,4,'C');

                tmpvalue = CSpare[4]*10;             //Spare5
                Send_Serial(tmpvalue,5,'C');

                tmpvalue = CSpare[5]*10;             //Spare6
                Send_Serial(tmpvalue,6,'C');

                tmpvalue = CSpare[6]*10;             //Spare7
                Send_Serial(tmpvalue,7,'C');

                //RS-232 Module

                TxBuf[0] = '[';                      //Start of Transmission Packet - SOT => 0x5B '[' 
                TxBuf[1] = 'D';                      //Chrger Unit      => 0x44 'D'
                TxBuf[2] = '0';                      //Module ID data   => 0x30 '0'
                TxBuf[3] = 'C';                      //Data Type => Continues 'C'
                //        
                for(i=0;i<32;i++)
                {
                    checksum^=TxBuf[i];
                }
                TxBuf[32]=(checksum>>4)+0x30;
                TxBuf[33]=(checksum&0x0F)+0x30;
                TxBuf[34]=']';						//End of Transmission Packet - EOT
                for(i=0; i<35; i++)                 //Data Packet 28Byte
                {
                    U1TXREG = TxBuf[i];
                    while(!U1STAbits.TRMT);
                }
            }
        }
    }         
}

//==================================== RS232 ===================================
void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt() //RS232
{
	UINT8 RcvData;
	UINT8 tmp=0,checksum=0,i;
    if(U1STAbits.OERR == 1)     //Clear Overflow Flag - KHJ
    {
        U1STAbits.OERR = 0;
    }
    RcvData = U1RXREG;              //RXbuff
	
    if(RcvData == '<' && EOT_Flag == 0)				//Start of Transmission Packet - SOT => 0x3C '<'
	{
        f_SData=1;                  //start serial data flag 
		Rindex=0;
		EOT_Flag=1;
	}
    //
	else if((f_SData == 1) && (RcvData == 0x3E) && (Rindex == 34))//EOT of Transmission Packet - EOT => 0x3E '>'
	{
		for(i=0;i<32;i++)
		{
			checksum^=RxBuf[i];
		}
		tmp=((RxBuf[32]-0x30)<<4)&0xF0;
		tmp=(tmp+((RxBuf[33]-0x30)&0x0F));
		if(tmp == checksum)
		{
			f_EData = 1;
			EOT_Flag = 0;
		}	
		else
		{
			f_SData = 0;
            f_EData=0;
            EOT_Flag = 0;
		}
	}
	RxBuf[Rindex]=RcvData;
	Rindex++;
	//
	if(Rindex>36)	//Occur overflow -> Reset buffer//
	{	
		Rindex=0;	
		f_SData=0;
		f_EData=0;
		EOT_Flag = 0;
	}
    
	IFS0bits.U1RXIF=0;
}  

void __attribute__((__interrupt__, no_auto_psv)) _U1TXInterrupt()
{
	IFS0bits.U1TXIF=0;
}  

