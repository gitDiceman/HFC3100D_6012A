/* 
 * Company      : Seoul Electric Power System co,ltd
 * Application  : HFC-3100D (+125Vdc)
 * File         : SPI_RS485.C
 * Author       : Martin. Kim
 * Date         : Created on 2018. 04. 18
 */

#include    <p30F6012A.h>
#include    "Definition.h"
#include 	"typedef.h"			//Type Define
#include    "Operation.h"
#include    "SPI_RS485.h"       //SPI Header

void Init_SPI(void);                          // SPI Initialize for Max3100
void Init_Max3100(void);                     // MAX3100 Chipset Initialize
void Serial_Process_485(void);              // Continuous Data Processing 
void Response_Serial_485(void);             // Data Response Select
void Receive_Data_485(UINT8 ID);            // Set Data Recieve RX
void Continuous_Send(void);                 // Continuous Data TX
void Request_Send(void);                    // Request Data TX
void Send_Data_485(UINT8 data);             // RS-485 Data Transfer Mode Fuction        
void Set485_Rx_Mode(void);                  // RS-485 Data Receive Mode Fuction
void Delay_Cnt(UINT16 cnt);                  // System Delay

extern void Response_FB_Debug(void);        // Debug Com. RS-232

// RS485 Variable

UINT8       f_SData_485 = 0;                // Start Character Packet from Module
UINT8       f_EData_485 = 0;                // End Character Packet from Module 
UINT8       EOT_Flag485 = 0;                // End of Transmit Flag from Module
UINT8       Rindex_485 = 0;                 // Rxbuffer Index Variable from Module
UINT8       RxBuf_485[36];                  // Rxbuffer for Debug from Module
UINT8       TxBuf_485[36];                  // Txbuffer for Debug from Module
UINT8       TxIndex_485 = 0;                // Tx 1Byte buffer for Debug from Module

// Module #N Continous Data
FLOAT32     Voltage_Out[4];                 // Modules Voltage Output
FLOAT32     Current_Out[4];                 // Modules Current Output
FLOAT32     Temperature[4];                 // Modules Temperature
FLOAT32     Input_Volt_R[4];                // Modules Input Voltage R
FLOAT32     Input_Volt_S[4];                // Modules Input Voltage S
FLOAT32     Input_Volt_T[4];                // Modules Input Voltage T
UINT8       Rx_Char[4];                     // Modules Status
UINT8       Rx_Char2[4];                    // Modules Warning Status                   
UINT8       Rx_Char3[4];
UINT8       Rx_Spare1[4];

bool8       Alram_Sts[5];                   // Modules Operation Status
bool8       Warning_Sts[5];                 // Modules Warning Status
bool8       CHG_Info[5];                    // Module Charge type

// Module #N Set Data
UINT8       ID_485 = 1;                     // Modules No. initial First No. 1

//Operation Mode Variable
UINT8       OP_MODE = 1;                    // OP_MODE => '0' : Manual / '1': Auto 
UINT8       OP_CMD = 1;                     // Command Change Operation Command Module2
UINT8       OP_Type = 0;                    // 0 : Stand-alone or 1 : Load Share Mode


void Init_SPI(void)
{
	IFS1bits.SPI2IF 	= 0;    //Interrupt Flag clear	
	IPC6bits.SPI2IP     = 0;	//Interrupt Priority
	IEC1bits.SPI2IE 	= 0;	//Interrupt Enable
	//----------------------------------------------------
	INTCON2bits.INT0EP  = 1;    //1 is (falling)negative edge.
	IFS0bits.INT0IF     = 0;    //Interrupt0 Flag 
	IEC0bits.INT0IE     = 0;    //Interrupt0 Enable    
    
	//=======================================================================================
	//X-FRMEN-SPIFSD-X |DISSDO-MODE16-SMP-CKE|SSEN-CKP-MSTEN-SPRE2| SPRE1-SPRE0-PPRE1-PPRE0//
	//0-0    -0     -0 |     0-     1-  0-  1|   0-  0-    1-    1|     1-    1-    1-    1// 
    //                                                       Prescale 1:1 1.8432MHz => 115200bps
	//---------------------------------------------------------------------------------------
	//SPI MODE 00	:	 CKE 1	CKP 0				0x0531      //Active clock stste
	//SPI MODE 01	:	 CKE 0  CKP 0				0x0431
	//SPI MODE 10	:	 CKE 1  CKP 1				0x0571
	//SPI MODE 11	:	 CKE 0  CKP 1				0x0471	
 
	SPI2CON = 0x053F;        //SPI2CON = 0x053F;			//0000-0101-001   1-1111//			
 
	//MODE 16BIT S.S.PIN DISABLE..		0000-010 CKE - SSEN CKP 11-1111
	//--------------------------------------------------------------------------------------------
	//SPIEN-x-SPISIDL-x | x-x-x-x | x-SPIROV-x-x | x-x-SPITBF-SPIRBF
	//0-0-0-0           | 0-0-0-0 | 0-0-0-0      | 0-0-0-0
	//--------------------------------------------------------------------------
	SPI2STATbits.SPIROV = 0;
	SPI2STATbits.SPIEN  = 1;
}

void Init_Max3100(void)
{
	UINT16 dummy = 0;
	//====================
	//Write Configuration
	//====================
	SEL_SPI = 0;                    //chip select Max3100  -- Active Low
	Delay_Cnt(2);                   //Init_Max3100()
	SPI2BUF=0xc000|0x040A;		//1100-0000-0000-1010               Write Config 9600,1stop,no parity.
	//SPI2BUF = 0xC400;               //1100-/TM /RM /PM /RAM-0000-0000	Write Config 115200,1stop,no parity
	while(!SPI2STATbits.SPIRBF);    //SPI Read buffer Complete Check
	dummy = SPI2BUF;                //buffer Load
	while(!(dummy&0x4000));			//1 is T.bufsfer empty	
	SEL_SPI = 1;                    //chip select Max3100 Active
	Delay_Cnt(1);                   //Init_Max3100()
	SEL_SPI = 0;                    //chip select Max3100 DeActive
	dummy = SPI2BUF;				//buffer clear;
	SPI2BUF = (0x8600);             //1000-0110-0000-0000//???? RTS pin low TE disable
	while(!SPI2STATbits.SPIRBF);    //SPI Read buffer Complete Check
	dummy = SPI2BUF;                //buffer Load
	while(!(dummy&0x4000));			//1(true) is T.buffer empty			
	SEL_SPI = 1;                    //chip select Max3100 Active
}

void Serial_Process_485(void)
{       
    //--------------------------------------------------------------//
    //                  Continues Data Receive                      //
    //--------------------------------------------------------------//
    if(f_SData_485 && f_EData_485)
    {
        Response_Serial_485();
    }

    //--------------------------------------------------------------//
    //                  Control Command Send                        //
    //--------------------------------------------------------------//
    else
    {
        //////////////////  Continuous Status Data Send  ///////////////////////
        if(Set_OK == 0)
        {
            if(tmr1_1 >= RS485_Period)				//elapse time about 500ms~750ms
            {
                if(ID_485 == 1)
                {
                    ID_485 = 2;
                }//Set ID 485 : Module2
                else if(ID_485 == 2)
                {
                    ID_485 = 1;
                }//Set ID 485 : Module1
                Continuous_Send();          //Continuous Data Send to Module1(with Module2)
                tmr1_1 = 0;                 //Tmr 1_1 Reset
            }	
//            else if(tmr1_1 == (RS485_Period*2))         //elapse time about 1000ms
//            {
//                tmr1_1 = 0;                 //Tmr 1_1 Reset
//                ID_485 = 2;                 //Set ID 485 : Module1
//                Continuous_Send();          //Continuous Data Send to Module2(with Module1)
//            }
        }
        ////////////////////////////////////////////////////////////////////////
        
        ////////////////////  Manual Operation Command Send  ///////////////////
        else
        {               
            Request_Send();                  // Set Command Data Sent to Modules
            if(OP_CMD == 1)                  // Operation CMD : => Module1 ON
            {
                Alram_Sts[1].b7 = 1;         // Forcing ON Status Change to Module1(Not real, Because, Transmission Delay)
                Alram_Sts[2].b7 = 0;         // Forcing OFF Status Change to Module2(Not real,Because, Transmission Delay)
            }
            else if(OP_CMD == 2)
            {
                Alram_Sts[1].b7 = 0;        // Forcing OFF Status Change to Module1(Not real, Because, Transmission Delay)
                Alram_Sts[2].b7 = 1;        // Forcing ON Status Change to Module2(Not real, Because, Transmission Delay)
            }
            RST_cnt = 0;                    // Restart Count Clear
        }
    }
}
        

//                     Main Controller <-> Module - KHJ                       //
//============================================================================//
//                       Request Data Protocol Packet                         //
//   '[' + 'U'+ '1'/'2' + 'C' + Spare1[4] + Spare2[5] + Checksum[6~7] + ']'   //
// -------------------------------------------------------------------------- //                                                                                                                                                 //
//                      Response Data Protocol Packet                         //
// '<' + 'M'+ '1'/'2' + 'C' + TxBuf[4~31](Vlotage[4~7] / Ampere[8~11])//
//  / Temperature[12~15] / Input_Volt_R[16~19] / Input_Volt_S[20~23] /        //
//  Input_Volt_S[24~27]) / STS[28] / SP1[29] / SP2[30] / SP2[31]) + '>'       //
//============================================================================//

void Response_Serial_485(void)
{
	switch(RxBuf_485[1])                            //Unit Select
	{
        case 'M':
            switch(RxBuf_485[2])                    //Module ID Select          
            {                
                case '1':                           //Module #1
                    switch(RxBuf_485[3])            //Ctrl Code Select
                    {
                        case 'C':
                        tmr2_4 = 0;
                        Receive_Data_485(1);        //Continues Data Recieve
                        break;
                      
                        default:
                        break;
                    }                               //Ctrl Code Select
                break;                              //Module #1
                
                case '2':                           //Module #2
                    switch(RxBuf_485[3])            //Ctrl Code Select
                    {
                        case 'C':
                        tmr2_5 = 0;
                        Receive_Data_485(2);        //LCD Continues Data Recieve
                        break;
                      
                        default:
                        break;
                    }                               //Ctrl Code Select
                break;                              //Module #2
                
                               
                default:
                break;
            }
        break;
        
        default:
        break;
	}
  	f_SData_485 = 0;                                // Start Character Flag Clear 
    f_EData_485 = 0;                                // End Character Flag Clear
    EOT_Flag485 = 0;
}

void Receive_Data_485(UINT8 ID)
{
    int tmpvalue = 0;
    unsigned char a = 0, b = 0, c = 0, d = 0;
        
    // Voltage Output
    a = RxBuf_485[4];
    b = RxBuf_485[5];
    c = RxBuf_485[6];
    d = RxBuf_485[7];
    if(a >= '0' && a <= '9')
    {
        if(b >= '0' && b <= '9')
        {
            if(c >= '0' && c <= '9')
            {
                if(d >= '0' && d <= '9')
                {
                    tmpvalue = (a-0x30)*1000 + (b-0x30)*100  + (c-0x30)*10 + (d-0x30)*1;		
                    Voltage_Out[ID] = tmpvalue * 0.1;  //  xxxx -> xxx.x
                }
            }
        }		
    }                          
                               
    // Current Output
    a = RxBuf_485[8];
    b = RxBuf_485[9];
    c = RxBuf_485[10];
    d = RxBuf_485[11];
    if(a >= '0' && a <= '9')
    {
        if(b >= '0' && b <= '9')
        {
            if(c >= '0' && c <= '9')
            {
                if(d >= '0' && d <= '9')
                {
                    tmpvalue = (a-0x30)*1000 + (b-0x30)*100  + (c-0x30)*10 + (d-0x30)*1;
                    Current_Out[ID] = tmpvalue * 0.1;  //  xxxx -> xxx.x
                }
            }
        }
    }

    // Temperature
    a = RxBuf_485[12];
    b = RxBuf_485[13];
    c = RxBuf_485[14];
    d = RxBuf_485[15];
    if(a >= '0' && a <= '9')
    {
        if(b >= '0' && b <= '9')
        {
            if(c >= '0' && c <= '9')
            {
                if(d >= '0' && d <= '9')
                {
                    tmpvalue = (a-0x30)*1000 + (b-0x30)*100  + (c-0x30)*10 + (d-0x30)*1;
                    Temperature[ID] = tmpvalue * 0.1;  //  xxxx -> xxx.x
                }
            }
        }		
    }  
    
    // Input Voltage R
    a = RxBuf_485[16];
    b = RxBuf_485[17];
    c = RxBuf_485[18];
    d = RxBuf_485[19];
    if(a >= '0' && a <= '9')
    {
        if(b >= '0' && b <= '9')
        {
            if(c >= '0' && c <= '9')
            {
                if(d >= '0' && d <= '9')
                {
                    tmpvalue = (a-0x30)*1000 + (b-0x30)*100  + (c-0x30)*10 + (d-0x30)*1;		
                    Input_Volt_R[ID] = tmpvalue * 0.1;  //  xxxx -> xxx.x
                }
            }
        }		
    }                          
                               
    // Input Voltage S
    a = RxBuf_485[20];
    b = RxBuf_485[21];
    c = RxBuf_485[22];
    d = RxBuf_485[23];
    if(a >= '0' && a <= '9')
    {
        if(b >= '0' && b <= '9')
        {
            if(c >= '0' && c <= '9')
            {
                if(d >= '0' && d <= '9')
                {
                    tmpvalue = (a-0x30)*1000 + (b-0x30)*100  + (c-0x30)*10 + (d-0x30)*1;
                    Input_Volt_S[ID] = tmpvalue * 0.1;  //  xxxx -> xxx.x
                }
            }
        }
    }

    // Input Voltage T
    a = RxBuf_485[24];
    b = RxBuf_485[25];
    c = RxBuf_485[26];
    d = RxBuf_485[27];
    if(a >= '0' && a <= '9')
    {
        if(b >= '0' && b <= '9')
        {
            if(c >= '0' && c <= '9')
            {
                if(d >= '0' && d <= '9')
                {
                    tmpvalue = (a-0x30)*1000 + (b-0x30)*100  + (c-0x30)*10 + (d-0x30)*1;
                    Input_Volt_T[ID] = tmpvalue * 0.1;  //  xxxx -> xxx.x
                }
            }
        }		
    }
    
    // Input Status
    Rx_Char[ID]   = RxBuf_485[28];
    Alram_Sts[ID].b0 = Rx_Char[ID] & 0x01;
    Alram_Sts[ID].b1 = (Rx_Char[ID] & 0x02) >> 1;
    Alram_Sts[ID].b2 = (Rx_Char[ID] & 0x04) >> 2;
    Alram_Sts[ID].b3 = (Rx_Char[ID] & 0x08) >> 3;
    Alram_Sts[ID].b4 = (Rx_Char[ID] & 0x10) >> 4;
    Alram_Sts[ID].b5 = (Rx_Char[ID] & 0x20) >> 5;
    Alram_Sts[ID].b6 = (Rx_Char[ID] & 0x40) >> 6;
    Alram_Sts[ID].b7 = (Rx_Char[ID] & 0x80) >> 7;
        
    Rx_Char2[ID] = RxBuf_485[29];
    Warning_Sts[ID].b0 = Rx_Char2[ID] & 0x01;
    Warning_Sts[ID].b1 = (Rx_Char2[ID] & 0x02) >> 1;
    Warning_Sts[ID].b2 = (Rx_Char2[ID] & 0x04) >> 2;
    Warning_Sts[ID].b3 = (Rx_Char2[ID] & 0x08) >> 3;
    Warning_Sts[ID].b4 = (Rx_Char2[ID] & 0x10) >> 4;
    Warning_Sts[ID].b5 = (Rx_Char2[ID] & 0x20) >> 5;
    Warning_Sts[ID].b6 = (Rx_Char2[ID] & 0x40) >> 6;
    Warning_Sts[ID].b7 = (Rx_Char2[ID] & 0x80) >> 7;    
      
    Rx_Char3[ID] = RxBuf_485[30];
    CHG_Info[ID].b0 = Rx_Char3[ID] & 0x01;
    CHG_Info[ID].b1 = (Rx_Char3[ID] & 0x02) >> 1;
    CHG_Info[ID].b2 = (Rx_Char3[ID] & 0x04) >> 2;
    CHG_Info[ID].b3 = (Rx_Char3[ID] & 0x08) >> 3;
    CHG_Info[ID].b4 = (Rx_Char3[ID] & 0x10) >> 4;
    CHG_Info[ID].b5 = (Rx_Char3[ID] & 0x20) >> 5;
    CHG_Info[ID].b6 = (Rx_Char3[ID] & 0x40) >> 6;
    CHG_Info[ID].b7 = (Rx_Char3[ID] & 0x80) >> 7;
    
    Rx_Spare1[ID] = RxBuf_485[31];   
}

void Continuous_Send(void)
{   
    unsigned char i = 0;
    unsigned int Checksum = 0;
    
    TxBuf_485[0] = '[';                 //Start of Transmission Packet - SOT => 0x5B '[' 
    TxBuf_485[1] = 'U';                 //Display Unit => 0x55 'U'
    TxBuf_485[2] = ID_485 + 0x30;       //CODE
    TxBuf_485[3] = 'C';                 //Data Type => Continuous => 0x43 'C'
    TxBuf_485[4] = '0';                 //Spare
    TxBuf_485[5] = '0';                 //Spare
    TxBuf_485[6] = '0';                 //Spare
    
    //Checksum[0]~[6]
    for(i=0 ; i<7 ; i++)
    {
        Checksum ^= TxBuf_485[i];
    }

    TxBuf_485[7] = ((Checksum & 0xF0) >> 4) + 0x30;        //Checksum High nibble
    TxBuf_485[8] = (Checksum & 0x0F) + 0x30;               //Checksum Low nibble
    TxBuf_485[9] =']';                  //End of Transmission Packet - EOT => 0x5D ']'
    
    for(i=0 ; i<10 ; i++)
    {
        Send_Data_485(TxBuf_485[i]);
    }
}

void Request_Send(void)
{   
    unsigned char i = 0;
    unsigned int Checksum = 0;
    
    TxBuf_485[0] = '[';                 //Start of Transmission Packet - SOT => 0x5B '[' 
    TxBuf_485[1] = 'U';                 //Display Unit => 0x55 'U'
    TxBuf_485[2] = '0';                 //CODE
    TxBuf_485[3] = 'S';                 //Data Type => Setting => 0x53 'S'
    TxBuf_485[4] = OP_MODE + 0x30;      //Data Type => Operation Mode => 0x30 '0' : Manual / 0x31 '1' : Auto
    TxBuf_485[5] = OP_CMD + 0x30;       //Data Type => Operation Command => 0x31 '1' : Module #1 ON, Module #2 OFF / 0x32 '2' :Module #1 OFF, Module #2 ON
    TxBuf_485[6] = OP_Type + 0x30;      //Data Type => Operation Command => 0x30 '0' : Stand-alone / 0x31 '1' : Load-sharing
    //Checksum[0]~[6]
    for(i=0 ; i<7 ; i++)
    {
        Checksum ^= TxBuf_485[i];
    }

    TxBuf_485[7] = ((Checksum & 0xF0) >> 4) + 0x30;        //Checksum High nibble
    TxBuf_485[8] = (Checksum & 0x0F) + 0x30;               //Checksum Low nibble
    TxBuf_485[9] =']';                  //End of Transmission Packet - EOT => 0x5D ']'
    
    for(i=0 ; i<10 ; i++)
    {
        Send_Data_485(TxBuf_485[i]);
    } 
    Set_OK = 0;
    tmr1_1 = 0;
}

void Send_Data_485(UINT8 data)
{
	//=============
	//write data
	//=============
	Delay_Cnt(1);
	SEL_SPI = 1;			
	_NOP();	_NOP();	_NOP(); _NOP(); _NOP();	_NOP();	//~1us
	SEL_SPI = 0;									//chip select Max3100  -- Active Low
	_NOP();	_NOP();	_NOP(); _NOP(); _NOP();	_NOP();	//~1us
	SPI2BUF = (0x8000 + (UINT16)data);				//1000-0-te-rts-0-0000-0000			
													//write data & TE 'High'  ???? RTS '0'
	while(!SPI2STATbits.SPIRBF);
	SEL_SPI = 1;	
	__asm__ volatile ("CLRWDT");					//#2	
	Delay_Cnt(1500);								//Max about 1ms delay //last data(char) missing????//
	Set485_Rx_Mode();	
}

void Set485_Rx_Mode(void)
{
		UINT16 RcvData = 0;	
		SEL_SPI = 1;                                //chip select Max3100  -- Active Low
        //Data Transfer
		SEL_SPI = 0;                                //chip select Max3100  -- Active High
		RcvData = SPI2BUF;							//buffer clear;
		SPI2BUF = (0x8600);                         //1000-0110-0000-0000	RTS '1'
		while(!SPI2STATbits.SPIRBF);
		RcvData = SPI2BUF;
		while(!(RcvData & 0x4000));					//wait...1(Set) is T.buffer empty
		SEL_SPI = 1;                                //chip select Max3100  -- Active Low
}

//External Interrupt//
void __attribute__((interrupt,auto_psv)) _INT0Interrupt(void)
{		//16us elapse//
	UINT16 RcvData = 0, i = 0;
    unsigned int tmp = 0, Checksum = 0;
		//										
	SEL_SPI = 0;                    // max3100 select. active low
	RcvData = SPI2BUF;				// buffer clear	
	SPI2BUF = READ_DATA;			// Read data Command
	while(!SPI2STATbits.SPIRBF);	// SPI Read Buffer Complete Check		
	SPI2STATbits.SPIROV = 0; 		// Clear SPI2 receive overflow
	RcvData = SPI2BUF;				// Get Data
      
	//-------------------------
    if((UINT8)(RcvData & 0x00ff) == '<' && EOT_Flag485 == 0)             //Start of Transmission Packet - SOT => 0x3C '<'
	{  
        f_SData_485 = 1;                  //start serial data flag 
		Rindex_485 = 0;
        EOT_Flag485 = 1;
	}
    //
	else if((f_SData_485 == 1) && ((UINT8)(RcvData & 0x00ff) == '>') && (Rindex_485 == 34))//EOT of Transmission Packet - EOT => 0x3E '>'
	{
       for(i=0 ; i<32 ; i++)
        {
            Checksum ^= RxBuf_485[i];
        }
        
        //Checksum Check
        tmp = ((RxBuf_485[32] - 0x30) << 4) & 0xF0;
        tmp = tmp + ((RxBuf_485[33] - 0x30) & 0x0F);
        if(tmp == Checksum)
        {
            f_EData_485 = 1;
            EOT_Flag485 = 0; 
        }
        else
        {
            f_SData_485 = 0;
            f_EData_485 = 0;
            EOT_Flag485 = 0;
        }	    
	}		
	RxBuf_485[Rindex_485] = (UINT8)(RcvData & 0x00ff);
	Rindex_485++;

    //Occur overflow -> Reset buffer//
	if(Rindex_485 > 36)
	{	
		Rindex_485 = 0;	
		f_SData_485 = 0;
		f_EData_485 = 0;
        EOT_Flag485 = 0;
	}
	SEL_SPI = 1;                    //max3100 chipselct active High
	IFS0bits.INT0IF = 0;			//interrupt flag clear.	   
}

void __attribute__((interrupt,auto_psv)) _SPI2Interrupt()		
{
	IFS1bits.SPI2IF=0;
}

void Delay_Cnt(UINT16 cnt)
{
	UINT16 i;
		for(i=0;i<cnt;i++)
		{
			__asm__ volatile ("NOP");
		}
}



