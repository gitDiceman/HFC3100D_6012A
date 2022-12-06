/* 
 * File:   RS232.c
 * Author: KHJ
 *
 * Created on 2018. 02. 07
 */

#include    <p30F6012A.h>
#include    "Definition.h"
#include    "typedef.h"
#include    "Memory.h"
#include    "RS232.h"
#include    "SPI_RS485.h"
#include    "ADC.h"
#include    "Operation.h"


//-----------------LCD Comm(RS-232)---------------------------//
void Init_Uart1(void);                      // for RS-232 for Debug 
void Init_Uart2(void);                      // for RS-232 for Module
void Serial_Process(void);                 // Continuous Data Processing
void Response_Serial(void);                // Data Response Select
void Continuous_FB(void);                  // Continuous Data Feedback TX
void Response_Data(void);                  // Set Data Recieve RX
void Response_FB(void);                    // Set Data Feedback TX
void Response_FB_Debug(void);             // Debug Data TX
void BittoByte(void);                      // Converting Data Type Byte to Bit
void Send_Serial(unsigned int ival,unsigned char pindex,unsigned char ch);      //RS232 Send String
//-------------------------------------------------------------------------
#define     BAUDRATE		9600                          //9600bps 18.08.02 Resistance Noise
#define     BRGVAL			(((FCY/BAUDRATE)/16)-1)         //7.5MIPS - KHJ

bool8       LED_Sts1 = {0,0,0,0,0,0,0,0};         // NFB LED Status Variable for LCD
bool8       LED_Sts2 = {0,0,0,0,0,0,0,0};         // NFB LED Status Variable for LCD
bool8       LED_Sts3 = {0,0,0,0,0,0,0,0};         // Spare1 LED Status Variable for LCD
bool8       LED_Sts4 = {0,0,0,0,0,0,0,0};         // Spare2 LED Status Variable for LCD

bool8       Module1_Sts = {0,0,0,0,0,0,0,1};      // Module1 Operation Status for LCD
bool8       Module2_Sts = {0,0,0,0,0,0,0,1};      // Module2 Operation Status for LCD
bool8       Module3_Sts = {0,0,0,0,0,0,0,1};      // Module3 Operation Status for LCD
bool8       Module4_Sts = {0,0,0,0,0,0,0,1};      // Module4 Operation Status for LCD

bool8       Batt_Sts = {0,0,0,0,0,0,0,0};         // Battery Condition Status for LCD
bool8       Module_Sts = {0,0,0,0,0,0,0,0};       // Module Status for LCD
bool8       CHG_Sts = {0,0,0,0,0,0,0,0};       // Spare Status1 for LCD
bool8       Spare_Sts1 = {0,0,0,0,0,0,0,0};       // Spare Status2 for LCD

// RS232 -Debug Variable
UINT8       f_SData_Debug = 0;              // Start Character Packet for Debug
UINT8       f_EData_Debug = 0;              // End Character Packet for Debug 
UINT8       EOT_Flag_Debug = 0;             // End of Transmit Flag for Debug
UINT8       Rindex_Debug = 0;               // Rxbuffer Index Variable for Debug
UINT8       RxBuf_Debug[36];                // Rxbuffer for Debug for Debug            
UINT8       TxBuf_Debug[36];                // Txbuffer for Debug for Debug
UINT8       Tx_Char_Debug[12];              // Tx 1Byte buffer for Debug
UINT8       TxIndex_Debug=0;                // Txbuffer Index Variable for Debug

// RS232 Variable
UINT8       f_SData = 0;                    // Start Character Packet from Module
UINT8       f_EData = 0;                    // End Character Packet from Module 
UINT8       EOT_Flag = 0;                   // End of Transmit Flag from Module
UINT8       Rindex = 0;                     // Rxbuffer Index Variable from Module
UINT8       RxBuf[36];                      // Rxbuffer for Debug from Module
UINT8       TxBuf[36];                      // Txbuffer for Debug from Module
UINT8       Tx_Char[12];                    // Tx 1Byte buffer for Debug from Module
UINT8       TxIndex=0;                      // Txbuffer Index Variable from Module

// LCD Continous Data
FLOAT32     Batt_Volt = 0;                  // Battery Voltage from Module
FLOAT32     Batt_Amp = 0;                   // Battery Current from Module
FLOAT32     Load_Volt = 0;                  // Load Voltage from Module
FLOAT32     Load_Amp = 0;                   // Load Current from Module
UINT8       LED_Module1_NFB = 1;            // Module1_NFB LED from Module
UINT8       LED_Module2_NFB = 1;            // Module2_NFB LED from Module
UINT8       LED_Module3_NFB = 1;            // Module3_NFB LED from Module
UINT8       LED_Module4_NFB = 1;            // Module4_NFB LED from Module
UINT8       LED_DC1_NFB = 1;                    // DC1 NFB LED from Module
UINT8       LED_DC2_NFB = 1;                    // DC2 NFB LED from Module
UINT8       LED_DC3_NFB = 1;                    // DC3 NFB LED from Module
UINT8       LED_DC4_NFB = 1;                    // DC4 NFB LED from Module
UINT8       LED_Battery_NFB;                // Battery NFB LED from Module
UINT8       LED_Spare1_NFB;                 // Spare1 NFB LED from Module
UINT8       LED_Spare2_NFB;                 // Spare2 NFB LED from Module
UINT8       LED_Load_NFB;                   // Load NFB LED from Module
UINT8       LED_Spare3_NFB;                 // Spare3 NFB LED from Module 
UINT8       LED_Spare4_NFB;                 // Spare4 NFB LED from Module
UINT8       LED_Spare5_NFB;                 // Spare5 NFB LED from Module
UINT8       LED_Spare6_NFB;                 // Spare6 NFB LED from Module
UINT8       SystemState_STS[4];             // Module State Status from Module
UINT8       Input_OV_STS[4];                // Input Over Voltage Status from Module
UINT8       Input_UV_STS[4];                // Input Under Voltage Status from Module
UINT8       Input_SD_STS[4];                // Input Shutdown Status from Module
UINT8       Output_OV_STS[4];               // Output Over Voltage Status from Module
UINT8       Output_UV_STS[4];               // Output Under Voltage Status from Module
UINT8       Output_OC_STS[4];               // Output Over Current Status from Module
UINT8       Output_OT_STS[4];               // Over Temperature Status from Module
UINT8       Module_OV_STS[4];               // Output Over Voltage Warning Status from Module
UINT8       Module_UV_STS[4];               // Output Under Voltage Warning Status from Module
UINT8       CHG_MODE[4];                    // Charge Mode(Pb Type)
UINT8       CHG_STS[8];                     // Charge Status(Pb Type)
UINT8       Batt_OV_STS = 0;                // Battery Over Voltage Status from Module
UINT8       Batt_UV_STS = 0;                // Battery Ounder Voltage Status from Module
UINT8       Batt_Mode_STS = 0;              // Battery Mode Status from Module

UINT8		bNfbState1,bNfbState1Old;		//nfb state : on/off
UINT8		bNfbState2,bNfbState2Old;		//nfb state : on/off

// Module Response Data
FLOAT32     Set_Batt_OV = Batt_OV_Ref;      // Load the Reference Data into a Battery Over Voltage Value 
FLOAT32     Set_Batt_UV = Batt_UV_Ref;      // Load the Reference Data into a Battery Under Voltage Value
FLOAT32     RST_Time = RST_Time_Min;        // Load the Reference Data into a Minimum Restart Time Value
FLOAT32     Spare2 = 0;                     
FLOAT32     Spare3 = 0;
FLOAT32     Spare4 = 0;

// Module Response backup Data
FLOAT32     Set_Batt_OV_B = Batt_OV_Ref;    // Load the Reference Data into a Battery Over Voltage Value
FLOAT32     Set_Batt_UV_B = Batt_UV_Ref;    // Load the Reference Data into a Battery Under Voltage Value
FLOAT32     RST_Time_B = RST_Time_Min;      // Load the Reference Data into a Minimum Restart Time Value
FLOAT32     Spare2_B = 0;
FLOAT32     Spare3_B = 0;
FLOAT32     Spare4_B = 0;

//RS232 - Debug// 
void Init_Uart1(void)		
{
	TRISFbits.TRISF3 = 0;		// I/O Setting UART TX
	TRISFbits.TRISF2 = 1;		// I/O Setting UART RX
	
	U1MODEbits.STSEL=0;         //1 stop bit
	U1MODEbits.PDSEL=0;         //no parity, 8 data bits
	U1MODEbits.ABAUD=0;         //disabled Auto-Baud
	U1BRG = BRGVAL;             //BRGVAL
	U1STAbits.URXISEL = 0;      //Interrupt after one RX character is received;
	U1STAbits.UTXISEL = 0;      //Interrupt after one TX character is Send;
    IPC2bits.U1TXIP = 5;        //Uart1 Tx Interrupt Priority
	IFS0bits.U1RXIF = 0;		//Clear Receive flag 
	IEC0bits.U1RXIE = 1;		//Enable Receive Interrupt
	
	U1MODEbits.UARTEN=1;        //Enable UART
	U1STAbits.UTXEN=1;          //Enable Transmit
}

//RS232- for LCD//
void Init_Uart2(void)		
{
	TRISFbits.TRISF5 = 0;		// I/O Setting UART TX
	TRISFbits.TRISF4 = 1;		// I/O Setting UART RX
	
	U2MODEbits.STSEL=0;         //1 stop bit
	U2MODEbits.PDSEL=0;         //no parity, 8 data bits
	U2MODEbits.ABAUD=0;         //disabled Auto-Baud
	U2BRG = BRGVAL;             //BRGVAL
	U2STAbits.URXISEL = 0;      //Interrupt after one RX character is received;
	U2STAbits.UTXISEL = 0;
    IPC6bits.U2TXIP = 0;        //Uart2 Tx Interrupt Priority
	IFS1bits.U2RXIF = 0;		//Clear Receive flag
	IEC1bits.U2RXIE = 1;		//Enable Receive Interrupt
	
	U2MODEbits.UARTEN = 1;        //Enable UART
	U2STAbits.UTXEN = 1;          //Enable Transmit
}

// Continuous Data Processing
void Serial_Process(void)
{
    // Fully Protocol Packet Data Receive Complete Start to END
    if(f_SData && f_EData)
    {
        if(tmr1_2 >= RS232_RX_Delay)
        {
            Response_Serial();
        }  
    }
    if(f_SData_Debug && f_EData_Debug)
    {
        f_SData_Debug = 0;                // Start Character Flag Clear 
        f_EData_Debug = 0;                // End Character Flag Clear
        EOT_Flag_Debug = 0;
    }
}

// Debug Data TX
void Response_FB_Debug(void)
{
    unsigned char i = 0;
    unsigned int Checksum = 0;
    
   //RS-232 Module
    TxBuf_Debug[0] = '[';                       //Start of Transmission Packet - SOT => 0x3C '<' 
    TxBuf_Debug[1] = 'U';                       //Chrger Unit      => 0x55 'U'
    TxBuf_Debug[2] = '0';                       //Module ID data   => 0x30 '0'                
    TxBuf_Debug[3] = 'S';                       //Data Type => Continues 'R'
    
    //TxBuf[4]~[5]
    TxBuf_Debug[4] = OP_MODE + 0x30;            //OP_MODE => '0' : Manual / '1': Auto
    TxBuf_Debug[5] = OP_CMD + 0x30;             //OP_CMD => '1' : Module1 / '2' : Module2
    TxBuf_Debug[6] = OP_Type + 0x30;            //Data Type => Operation Command => 0x30 '0' : Stand-alone / 0x31 '1' : Load-sharing
           
    //Checksum TxBuf[0]~[6]
    for(i=0 ; i<7 ; i++)
    {
        Checksum ^= TxBuf_Debug[i];
    }
    TxBuf_Debug[7] = ((Checksum & 0xF0) >> 4) + 0x30;        //Checksum High nibble
    TxBuf_Debug[8] = (Checksum & 0x0F) + 0x30;               //Checksum Low nibble
    TxBuf_Debug[9] = ']';                       //End of Transmission Packet - EOT
    
    for(i=0; i<10; i++)                          //Data Packet
    {
        U1TXREG = TxBuf_Debug[i];
        while(!U1STAbits.TRMT);
    }
}

//                     Module <-> Module LCD-KHJ                             //
//============================================================================//
//                    Request Data Protocol Packet                            //
// '[' + 'D'+ '0' + 'S' + TxBuf[0~19](Set_ARef[0~3] / Set_CL[4~7] /           // 
//  Set_VRef[8~11] / Set_UV[12~15] / Set_OV[16~19] / Set_OT[20~23]) + ']'     //
//----------------------------------------------------------------------------// 
//                   Response Data Protocol Packet                            //
// '<' + 'U'+ '0' + 'R' + TxBuf[0~19](Set_ARef[0~3] / Set_CL[4~7] /  //
// Set_VRef[8~11] / Set_UV[12~15] / Set_OV[16~19] / Set_OT[20~23]) + '>'      //
//============================================================================//
// Data Response Select
void Response_Serial(void)
{
	switch(RxBuf[1])                            //Unit Select
	{
        case 'D':                               // Display
            switch(RxBuf[2])                    //Module ID Select          
            {                
                ////////////////////////////////////////////////////////////////
                //------------------------- Module ---------------------------//               
                //////////////////////////////////////////////////////////////// 
                case '0':                       //Module 1
                    switch(RxBuf[3])            //Ctrl Code Select
                    {
                        case 'S':               //Setting Data 
                        Response_Data();        //Module Response Data Recieve
                        Response_FB();          //Module Response Data Feedback Send
                        Set_OK = 1;             //Set, Change Bus Stop Condition of RS-485 before to Send Response Feedback
                        break;                  //Setting Data Set Start END
                        
                        case 'C':               //Continuous Data       9600bps = ~29.16ms / 115200 = ~2.43ms
                        Continuous_FB();        //Module Continuous Data Recieve
                        Set_OK = 0;
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
  	f_SData = 0;                // Start Character Flag Clear 
    f_EData = 0;                // End Character Flag Clear
    EOT_Flag = 0;
}

// Set Data Recieve RX
void Response_Data(void)
{
    int tmpvalue = 0;
    unsigned char a = 0, b = 0, c = 0, d = 0;
    // Current Reference Setting Feedback
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
                    Set_Batt_OV = tmpvalue * 0.1;  //  xxxx -> xxx.x
                    if(Set_Batt_OV > Batt_UV_Ref && Set_Batt_OV <= Batt_OV_Ref)
                    {
                        Set_Batt_OV_B = Set_Batt_OV;
                    }
                    else 
                    {
                        Set_Batt_OV = Set_Batt_OV_B;
                    }
                    
                }
            }
        }		
    }                          
                               
     // Current Limit Setting Feedback
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
                    Set_Batt_UV = tmpvalue * 0.1;    //  xxxx -> xxx.x
                    if(Set_Batt_UV >= Batt_UV_Ref && Set_Batt_UV < Batt_OV_Ref)
                    {
                        Set_Batt_UV_B = Set_Batt_UV;
                    }
                    else 
                    {
                        Set_Batt_UV = Set_Batt_UV_B;
                    }
                    
                }
            }
        }
    }

    //RST_Time
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
                    RST_Time = tmpvalue * 0.1;      //  xxxx -> xxx.x
                    if(RST_Time >= RST_Time_Min && RST_Time <= RST_Time_Max)
                    {
                        RST_Time_B = RST_Time;
                    }
                    else 
                    {
                        RST_Time = RST_Time_B;
                    }
                }
            }
        }		
    }
    //Spare2
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
                    Spare2 = tmpvalue * 0.1;      //xxxx -> xxx.x
                }
            }
        }		
    }
    //Spare3
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
                    Spare3 = tmpvalue * 0.1;        //xxxx -> xxx.x
                }
            }
        }		
    }
    //Spare4
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
                    Spare4 = tmpvalue * 0.1;        //xxxx -> xxx.x
                }
            }
        }
    }
        
    // Input Status
    OP_MODE = RxBuf[28] - 0x30;
    OP_CMD = RxBuf[29] - 0x30;
    OP_Type = RxBuf[30] - 0x30;
    SYS_Type = RxBuf[31] - 0x30;
    Mode_Check();
}

// Set Data Feedback TX
void Response_FB(void)
{
    unsigned char i = 0;
    unsigned int Checksum = 0;
    int tmpvalue = 0;
    
   //RS-232 Module
    TxBuf[0] = '<';                     //Start of Transmission Packet - SOT => 0x3C '<' 
    TxBuf[1] = 'U';                     //Chrger Unit      => 0x55 'U'
    TxBuf[2] = '0';						//Module ID data   => 0x30 '0'                
    TxBuf[3] = 'R';                     //Data Type => Continues 'R'
    
    //TxBuf[4]~[27]
    tmpvalue = Set_Batt_OV * 10;
    Send_Serial(tmpvalue,1,'R');
    tmpvalue = Set_Batt_UV * 10;
    Send_Serial(tmpvalue,2,'R');
    tmpvalue = RST_Time * 10;
    Send_Serial(tmpvalue,3,'R');
    tmpvalue = Spare2 * 10;
    Send_Serial(tmpvalue,4,'R');
    tmpvalue = Spare3 * 10;
    Send_Serial(tmpvalue,5,'R');
    tmpvalue = Spare4 * 10;
    Send_Serial(tmpvalue,6,'R');    
    
    //Status TxBuf[28]~[31]
    TxBuf[28] = OP_MODE + 0x30;
    TxBuf[29] = OP_CMD + 0x30;
    TxBuf[30] = OP_Type + 0x30;
    TxBuf[31] = SYS_Type + 0x30;
       
    //Checksum TxBuf[32]~[33]
    for(i=0 ; i<32 ; i++)
    {
        Checksum ^= TxBuf[i];
    }
    TxBuf[32] = ((Checksum & 0xF0) >> 4) + 0x30;        //Checksum High nibble
    TxBuf[33] = (Checksum & 0x0F) + 0x30;               //Checksum Low nibble
    
    TxBuf[34] = '>';                                    //End of Transmission Packet - EOT
    
    for(i=0; i<35; i++)                                 //Data Packet
    {
        U2TXREG = TxBuf[i];
        while(!U2STAbits.TRMT);
    }
    U1TXREG = 'S';
    while(!U1STAbits.TRMT); 
}


//============================================================================//
//                      Continuous Data Protocol Packet                       //
// '<' + 'U'+ '0' + 'C' + TxBuf[0~6](Vlotage[0~3] / Ampere[4~7]) + '>'  // 
//============================================================================//
// Continuous Data Feedback TX
void Continuous_FB(void)
{
    unsigned char i = 0;
    unsigned int Checksum = 0;
    int tmpvalue = 0;
        
    //----------------------------------------------------------//
    //              Metering & Alram Data Transmit-KHJ          //
    //----------------------------------------------------------//            
    //RS-232 Module
    TxBuf[0] = '<';                     //Start of Transmission Packet - SOT => 0x3C '<' 
    TxBuf[1] = 'U';                     //Chrger Unit      => 0x55 'U'
    TxBuf[2] = '0';						//Module ID data   => 0x30 '0'                
    TxBuf[3] = 'C';                     //Data Type => Continues 'C'
    
    //TxBuf[4]~[19]
	tmpvalue = Batt_Volt * 10;
    Send_Serial(tmpvalue,1,'C');
    tmpvalue = Batt_Amp * 10;
    Send_Serial(tmpvalue,2,'C');
    tmpvalue = Load_Volt * 10;
    Send_Serial(tmpvalue,3,'C');
    tmpvalue = Load_Amp * 10;
    Send_Serial(tmpvalue,4,'C');
    
    //TxBuf[20]~[31]
    BittoByte();
    for(i=0; i<12 ; i++)
    {
        TxBuf[i+20] = Tx_Char[i];
    }
     
    //Checksum[32]~[33]
    for(i=0 ; i<32 ; i++)
    {
        Checksum ^= TxBuf[i];
    }

    TxBuf[32] = ((Checksum & 0xF0) >> 4) + 0x30;        //Checksum High nibble
    TxBuf[33] = (Checksum & 0x0F) + 0x30;               //Checksum Low nibble
    
    TxBuf[34] = '>';                                    //End of Transmission Packet - EOT
    
    for(i=0; i<35; i++)                                 //Data Packet
    {
        U2TXREG = TxBuf[i];
        while(!U2STAbits.TRMT); 
    }
    U1TXREG = 'C';
    while(!U1STAbits.TRMT); 
}

// Converting Data Type Byte to Bit
void BittoByte(void)
{
	//////////////////////////////////////////
    //    LED Status1 data(1byte : 8bit)    //
    //////////////////////////////////////////
    LED_Module1_NFB = CheckBit(bNfbState1,7);
	LED_Module2_NFB = CheckBit(bNfbState1,6);
//	LED_Module3_NFB = CheckBit(bNfbState1,5);
//	LED_Module4_NFB = CheckBit(bNfbState1,4);
	LED_DC1_NFB = CheckBit(bNfbState1,5);
	LED_DC2_NFB = CheckBit(bNfbState1,4);
//	LED_DC3_NFB = CheckBit(bNfbState1,1);
//	LED_DC4_NFB = CheckBit(bNfbState2,7);
    
    LED_Sts1.b7 = LED_Module1_NFB;      //Module1_NFB
    LED_Sts1.b6 = LED_Module2_NFB;      //Module2_NFB
    LED_Sts1.b5 = LED_Module3_NFB;      //Module3_NFB
    LED_Sts1.b4 = LED_Module4_NFB;      //Module4_NFB
    LED_Sts1.b3 = LED_DC1_NFB;          //DC1_NFB
    LED_Sts1.b2 = LED_DC2_NFB;          //DC2_NFB
    LED_Sts1.b1 = LED_DC3_NFB;          //DC3_NFB
    LED_Sts1.b0 = LED_DC4_NFB;          //DC4_NFB
    
    //////////////////////////////////////////
    //    LED Status2 data(1byte : 8bit)    //
    //////////////////////////////////////////
    LED_Battery_NFB = CheckBit(bNfbState1,3);
//	LED_Spare1_NFB = CheckBit(bNfbState2,5);
//	LED_Spare2_NFB = CheckBit(bNfbState2,4);
	LED_Load_NFB = CheckBit(bNfbState1,2);
//	LED_Spare3_NFB = CheckBit(bNfbState2,2);
//	LED_Spare4_NFB = CheckBit(bNfbState2,1);

    LED_Sts2.b7 = LED_Battery_NFB;      //Battery_NFB
    LED_Sts2.b6 = LED_Spare1_NFB;       //Spare1_NFB
    LED_Sts2.b5 = LED_Spare2_NFB;       //Spare2_NFB
    LED_Sts2.b4 = LED_Load_NFB;         //Load_NFB
    LED_Sts2.b3 = LED_Spare3_NFB;       //Spare3_NFB
    LED_Sts2.b2 = LED_Spare4_NFB;       //Spare4_NFB
    LED_Sts2.b1 = LED_Spare5_NFB;       //Spare5_NFB
    LED_Sts2.b0 = LED_Spare6_NFB;       //Spare6_NFB
    
    //////////////////////////////////////////
    //    LED Status3 data(1byte : 8bit)    //
    //////////////////////////////////////////
    LED_Sts3.b7 = 0;        //Battery_NFB
    LED_Sts3.b6 = 0;        //Spare1_NFB
    LED_Sts3.b5 = 0;        //Spare2_NFB
    LED_Sts3.b4 = 0;        //Load_NFB
    LED_Sts3.b3 = 0;        //Spare3_NFB
    LED_Sts3.b2 = 0;        //Spare4_NFB
    LED_Sts3.b1 = 0;        //Spare5_NFB
    LED_Sts3.b0 = 0;        //Spare6_NFB
    
    //////////////////////////////////////////
    //    LED Status4 data(1byte : 8bit)    //
    //////////////////////////////////////////
    LED_Sts4.b7 = 0;        //Battery_NFB
    LED_Sts4.b6 = 0;        //Spare1_NFB
    LED_Sts4.b5 = 0;        //Spare2_NFB
    LED_Sts4.b4 = 0;        //Load_NFB
    LED_Sts4.b3 = 0;        //Spare3_NFB
    LED_Sts4.b2 = 0;        //Spare4_NFB
    LED_Sts4.b1 = 0;        //Spare5_NFB
    LED_Sts4.b0 = 0;        //Spare6_NFB
    
    //////////////////////////////////////////
    //   Module #1 Status1(1byte : 8bit)    //
    //////////////////////////////////////////
    SystemState_STS[1] = Alram_Sts[1].b7;
    Input_OV_STS[1] = Alram_Sts[1].b6;
    Input_UV_STS[1] = Alram_Sts[1].b5;
    Input_SD_STS[1] = Alram_Sts[1].b4;
    Output_OV_STS[1] = Alram_Sts[1].b3;
    Output_UV_STS[1] = Alram_Sts[1].b2;
    Output_OC_STS[1] = Alram_Sts[1].b1;
    Output_OT_STS[1] = Alram_Sts[1].b0;
        
    Module1_Sts.b7 = SystemState_STS[1];          //Operation
    Module1_Sts.b6 = Input_OV_STS[1];             //Input_OV
    Module1_Sts.b5 = Input_UV_STS[1];             //Input_UV
    Module1_Sts.b4 = Input_SD_STS[1];             //Input_Shutdown
    Module1_Sts.b3 = Output_OV_STS[1];            //Output_OV
    Module1_Sts.b2 = Output_UV_STS[1];            //Output_UV
    Module1_Sts.b1 = Output_OC_STS[1];            //Output_OC
    Module1_Sts.b0 = Output_OT_STS[1];            //Output_OT
    
       
     //////////////////////////////////////////
    //   Module #2 Status1(1byte : 8bit)    //
    //////////////////////////////////////////
    SystemState_STS[2] = Alram_Sts[2].b7;
    Input_OV_STS[2] = Alram_Sts[2].b6;
    Input_UV_STS[2] = Alram_Sts[2].b5;
    Input_SD_STS[2] = Alram_Sts[2].b4;
    Output_OV_STS[2] = Alram_Sts[2].b3;
    Output_UV_STS[2] = Alram_Sts[2].b2;
    Output_OC_STS[2] = Alram_Sts[2].b1;
    Output_OT_STS[2] = Alram_Sts[2].b0;
    
    Module2_Sts.b7 = SystemState_STS[2];          //Operation
    Module2_Sts.b6 = Input_OV_STS[2];             //Input_OV
    Module2_Sts.b5 = Input_UV_STS[2];             //Input_UV
    Module2_Sts.b4 = Input_SD_STS[2];             //Input_Shutdown
    Module2_Sts.b3 = Output_OV_STS[2];            //Output_OV
    Module2_Sts.b2 = Output_UV_STS[2];            //Output_UV
    Module2_Sts.b1 = Output_OC_STS[2];            //Output_OC
    Module2_Sts.b0 = Output_OT_STS[2];            //Output_OT
        
    //////////////////////////////////////////
    //   Module #3 Status1(1byte : 8bit)    //
    //////////////////////////////////////////
    SystemState_STS[3] = Alram_Sts[3].b7;
    Input_OV_STS[3] = Alram_Sts[3].b6;
    Input_UV_STS[3] = Alram_Sts[3].b5;
    Input_SD_STS[3] = Alram_Sts[3].b4;
    Output_OV_STS[3] = Alram_Sts[3].b3;
    Output_UV_STS[3] = Alram_Sts[3].b2;
    Output_OC_STS[3] = Alram_Sts[3].b1;
    Output_OT_STS[3] = Alram_Sts[3].b0;
    
    Module3_Sts.b7 = SystemState_STS[3];          //Operation
    Module3_Sts.b6 = Input_OV_STS[3];             //Input_OV
    Module3_Sts.b5 = Input_UV_STS[3];             //Input_UV
    Module3_Sts.b4 = Input_SD_STS[3];             //Input_Shutdown
    Module3_Sts.b3 = Output_OV_STS[3];            //Output_OV
    Module3_Sts.b2 = Output_UV_STS[3];            //Output_UV
    Module3_Sts.b1 = Output_OC_STS[3];            //Output_OC
    Module3_Sts.b0 = Output_OT_STS[3];            //Output_OT
            
    //////////////////////////////////////////
    //   Module #4 Status1(1byte : 8bit)    //
    //////////////////////////////////////////
    SystemState_STS[4]  = Alram_Sts[4].b7;
    Input_OV_STS[4]     = Alram_Sts[4].b6;
    Input_UV_STS[4]     = Alram_Sts[4].b5;
    Input_SD_STS[4]     = Alram_Sts[4].b4;
    Output_OV_STS[4]    = Alram_Sts[4].b3;
    Output_UV_STS[4]    = Alram_Sts[4].b2;
    Output_OC_STS[4]    = Alram_Sts[4].b1;
    Output_OT_STS[4]    = Alram_Sts[4].b0;
    
    Module4_Sts.b7 = SystemState_STS[4];          //Operation
    Module4_Sts.b6 = Input_OV_STS[4];             //Input_OV
    Module4_Sts.b5 = Input_UV_STS[4];             //Input_UV
    Module4_Sts.b4 = Input_SD_STS[4];             //Input_Shutdown
    Module4_Sts.b3 = Output_OV_STS[4];            //Output_OV
    Module4_Sts.b2 = Output_UV_STS[4];            //Output_UV
    Module4_Sts.b1 = Output_OC_STS[4];            //Output_OC
    Module4_Sts.b0 = Output_OT_STS[4];            //Output_OT
    
    //////////////////////////////////////////
    //   Battery Status(1byte : 8bit)       //
    //////////////////////////////////////////    
    Batt_Sts.b7 = Batt_OV_STS;              // Baterry Under voltage Status
    Batt_Sts.b6 = Batt_UV_STS;              // Baterry Under voltage Status
    Batt_Sts.b5 = Batt_Mode_STS;            // Baterry Charge : 0 / Discharge : 1 
    Batt_Sts.b4 = 0;                        //
    Batt_Sts.b3 = 0;                        //
    Batt_Sts.b2 = 0;                        //
    Batt_Sts.b1 = 0;                        //
    Batt_Sts.b0 = 0;                        //
    
    //////////////////////////////////////////
    //     Module Status(1byte : 8bit)      //
    //////////////////////////////////////////
    Module_OV_STS[1] = Warning_Sts[1].b7;
    Module_UV_STS[1] = Warning_Sts[1].b6;
    Module_OV_STS[2] = Warning_Sts[2].b7;
    Module_UV_STS[2] = Warning_Sts[2].b6;
    Module_OV_STS[3] = Warning_Sts[3].b7;
    Module_UV_STS[3] = Warning_Sts[3].b6;
    Module_OV_STS[4] = Warning_Sts[4].b7;
    Module_UV_STS[4] = Warning_Sts[4].b6;
    
    Module_Sts.b7 = Module_OV_STS[1];       //
    Module_Sts.b6 = Module_UV_STS[1];       //
    Module_Sts.b5 = Module_OV_STS[2];       //
    Module_Sts.b4 = Module_UV_STS[2];       //
    Module_Sts.b3 = Module_OV_STS[3];       //
    Module_Sts.b2 = Module_UV_STS[3];       //
    Module_Sts.b1 = Module_OV_STS[4];       //
    Module_Sts.b0 = Module_UV_STS[4];       //
    
    //////////////////////////////////////////
    //      Spare1 Status(1byte : 8bit)     //
    //////////////////////////////////////////
    CHG_STS[1] = CHG_Info[1].b7;
    CHG_STS[2] = CHG_Info[1].b6;
    CHG_STS[3] = CHG_Info[2].b7;
    CHG_STS[4] = CHG_Info[2].b6;
    CHG_STS[5] = CHG_Info[3].b7;
    CHG_STS[6] = CHG_Info[3].b6;
    CHG_STS[7] = CHG_Info[4].b7;
    CHG_STS[8] = CHG_Info[4].b6;
        
    CHG_Sts.b7 = CHG_STS[1];                        //
    CHG_Sts.b6 = CHG_STS[2];                        //
    CHG_Sts.b5 = CHG_STS[3];                        //
    CHG_Sts.b4 = CHG_STS[4];                        //
    CHG_Sts.b3 = CHG_STS[5];                        //
    CHG_Sts.b2 = CHG_STS[6];                        //
    CHG_Sts.b1 = CHG_STS[7];                        //
    CHG_Sts.b0 = CHG_STS[8];                        //
    
    //////////////////////////////////////////
    //      Spare2 Status(1byte : 8bit)     //
    //////////////////////////////////////////
    Spare_Sts1.b7 = 0;                        //
    Spare_Sts1.b6 = 0;                        //
    Spare_Sts1.b5 = 0;                        //
    Spare_Sts1.b4 = 0;                        //
    Spare_Sts1.b3 = 0;                        //
    Spare_Sts1.b2 = 0;                        //
    Spare_Sts1.b1 = 0;                        //
    Spare_Sts1.b0 = 0;                        //
    
    //////////////////////////////////////////
    //      LED Status1 Byte Package       //
    //////////////////////////////////////////
    Tx_Char[0] = LED_Sts1.b7;             
    Tx_Char[0] = (Tx_Char[0] << 1) | LED_Sts1.b6;
    Tx_Char[0] = (Tx_Char[0] << 1) | LED_Sts1.b5;
    Tx_Char[0] = (Tx_Char[0] << 1) | LED_Sts1.b4;
    Tx_Char[0] = (Tx_Char[0] << 1) | LED_Sts1.b3;
    Tx_Char[0] = (Tx_Char[0] << 1) | LED_Sts1.b2;
    Tx_Char[0] = (Tx_Char[0] << 1) | LED_Sts1.b1;
    Tx_Char[0] = (Tx_Char[0] << 1) | LED_Sts1.b0;
    
    //////////////////////////////////////////
    //      LED Status2 Byte Package       //
    //////////////////////////////////////////
    Tx_Char[1] = LED_Sts2.b7;             
    Tx_Char[1] = (Tx_Char[1] << 1) | LED_Sts2.b6;
    Tx_Char[1] = (Tx_Char[1] << 1) | LED_Sts2.b5;
    Tx_Char[1] = (Tx_Char[1] << 1) | LED_Sts2.b4;
    Tx_Char[1] = (Tx_Char[1] << 1) | LED_Sts2.b3;
    Tx_Char[1] = (Tx_Char[1] << 1) | LED_Sts2.b2;
    Tx_Char[1] = (Tx_Char[1] << 1) | LED_Sts2.b1;
    Tx_Char[1] = (Tx_Char[1] << 1) | LED_Sts2.b0;
    
    //////////////////////////////////////////
    //      LED Status3 Byte Package       //
    //////////////////////////////////////////
    Tx_Char[2] = LED_Sts3.b7;             
    Tx_Char[2] = (Tx_Char[2] << 1) | LED_Sts3.b6;
    Tx_Char[2] = (Tx_Char[2] << 1) | LED_Sts3.b5;
    Tx_Char[2] = (Tx_Char[2] << 1) | LED_Sts3.b4;
    Tx_Char[2] = (Tx_Char[2] << 1) | LED_Sts3.b3;
    Tx_Char[2] = (Tx_Char[2] << 1) | LED_Sts3.b2;
    Tx_Char[2] = (Tx_Char[2] << 1) | LED_Sts3.b1;
    Tx_Char[2] = (Tx_Char[2] << 1) | LED_Sts3.b0;
    
    //////////////////////////////////////////
    //      LED Status4 Byte Package       //
    //////////////////////////////////////////
    Tx_Char[3] = LED_Sts4.b7;             
    Tx_Char[3] = (Tx_Char[3] << 1) | LED_Sts4.b6;
    Tx_Char[3] = (Tx_Char[3] << 1) | LED_Sts4.b5;
    Tx_Char[3] = (Tx_Char[3] << 1) | LED_Sts4.b4;
    Tx_Char[3] = (Tx_Char[3] << 1) | LED_Sts4.b3;
    Tx_Char[3] = (Tx_Char[3] << 1) | LED_Sts4.b2;
    Tx_Char[3] = (Tx_Char[3] << 1) | LED_Sts4.b1;
    Tx_Char[3] = (Tx_Char[3] << 1) | LED_Sts4.b0;
    
    //////////////////////////////////////////
    //      Module1_Status Byte Package     //
    //////////////////////////////////////////
    Tx_Char[4] = Module1_Sts.b7;             
    Tx_Char[4] = (Tx_Char[4] << 1) | Module1_Sts.b6;
    Tx_Char[4] = (Tx_Char[4] << 1) | Module1_Sts.b5;
    Tx_Char[4] = (Tx_Char[4] << 1) | Module1_Sts.b4;
    Tx_Char[4] = (Tx_Char[4] << 1) | Module1_Sts.b3;
    Tx_Char[4] = (Tx_Char[4] << 1) | Module1_Sts.b2;
    Tx_Char[4] = (Tx_Char[4] << 1) | Module1_Sts.b1;
    Tx_Char[4] = (Tx_Char[4] << 1) | Module1_Sts.b0;
    
    //////////////////////////////////////////
    //      Module2_Status Byte Package     //
    //////////////////////////////////////////
    Tx_Char[5] = Module2_Sts.b7;             
    Tx_Char[5] = (Tx_Char[5] << 1) | Module2_Sts.b6;
    Tx_Char[5] = (Tx_Char[5] << 1) | Module2_Sts.b5;
    Tx_Char[5] = (Tx_Char[5] << 1) | Module2_Sts.b4;
    Tx_Char[5] = (Tx_Char[5] << 1) | Module2_Sts.b3;
    Tx_Char[5] = (Tx_Char[5] << 1) | Module2_Sts.b2;
    Tx_Char[5] = (Tx_Char[5] << 1) | Module2_Sts.b1;
    Tx_Char[5] = (Tx_Char[5] << 1) | Module2_Sts.b0;
    
    //////////////////////////////////////////
    //      Module3_Status Byte Package     //
    //////////////////////////////////////////
    Tx_Char[6] = Module3_Sts.b7;             
    Tx_Char[6] = (Tx_Char[6] << 1) | Module3_Sts.b6;
    Tx_Char[6] = (Tx_Char[6] << 1) | Module3_Sts.b5;
    Tx_Char[6] = (Tx_Char[6] << 1) | Module3_Sts.b4;
    Tx_Char[6] = (Tx_Char[6] << 1) | Module3_Sts.b3;
    Tx_Char[6] = (Tx_Char[6] << 1) | Module3_Sts.b2;
    Tx_Char[6] = (Tx_Char[6] << 1) | Module3_Sts.b1;
    Tx_Char[6] = (Tx_Char[6] << 1) | Module3_Sts.b0;
    
    //////////////////////////////////////////
    //      Module4_Status Byte Package     //
    //////////////////////////////////////////
    Tx_Char[7] = Module4_Sts.b7;             
    Tx_Char[7] = (Tx_Char[7] << 1) | Module4_Sts.b6;
    Tx_Char[7] = (Tx_Char[7] << 1) | Module4_Sts.b5;
    Tx_Char[7] = (Tx_Char[7] << 1) | Module4_Sts.b4;
    Tx_Char[7] = (Tx_Char[7] << 1) | Module4_Sts.b3;
    Tx_Char[7] = (Tx_Char[7] << 1) | Module4_Sts.b2;
    Tx_Char[7] = (Tx_Char[7] << 1) | Module4_Sts.b1;
    Tx_Char[7] = (Tx_Char[7] << 1) | Module4_Sts.b0;
    
    //////////////////////////////////////////
    //      Batt_Status Byte Package        //
    //////////////////////////////////////////
    Tx_Char[8] = Batt_Sts.b7;             
    Tx_Char[8] = (Tx_Char[8] << 1) | Batt_Sts.b6;
    Tx_Char[8] = (Tx_Char[8] << 1) | Batt_Sts.b5;
    Tx_Char[8] = (Tx_Char[8] << 1) | Batt_Sts.b4;
    Tx_Char[8] = (Tx_Char[8] << 1) | Batt_Sts.b3;
    Tx_Char[8] = (Tx_Char[8] << 1) | Batt_Sts.b2;
    Tx_Char[8] = (Tx_Char[8] << 1) | Batt_Sts.b1;
    Tx_Char[8] = (Tx_Char[8] << 1) | Batt_Sts.b0;
    
    //////////////////////////////////////////
    //      Module_Sts Byte Package       //
    //////////////////////////////////////////
    Tx_Char[9] = Module_Sts.b7;             
    Tx_Char[9] = (Tx_Char[9] << 1) | Module_Sts.b6;
    Tx_Char[9] = (Tx_Char[9] << 1) | Module_Sts.b5;
    Tx_Char[9] = (Tx_Char[9] << 1) | Module_Sts.b4;
    Tx_Char[9] = (Tx_Char[9] << 1) | Module_Sts.b3;
    Tx_Char[9] = (Tx_Char[9] << 1) | Module_Sts.b2;
    Tx_Char[9] = (Tx_Char[9] << 1) | Module_Sts.b1;
    Tx_Char[9] = (Tx_Char[9] << 1) | Module_Sts.b0;
    
    //////////////////////////////////////////
    //           CHG_Sts Byte Package       //
    //////////////////////////////////////////
    Tx_Char[10] = CHG_Sts.b7;             
    Tx_Char[10] = (Tx_Char[10] << 1) | CHG_Sts.b6;
    Tx_Char[10] = (Tx_Char[10] << 1) | CHG_Sts.b5;
    Tx_Char[10] = (Tx_Char[10] << 1) | CHG_Sts.b4;
    Tx_Char[10] = (Tx_Char[10] << 1) | CHG_Sts.b3;
    Tx_Char[10] = (Tx_Char[10] << 1) | CHG_Sts.b2;
    Tx_Char[10] = (Tx_Char[10] << 1) | CHG_Sts.b1;
    Tx_Char[10] = (Tx_Char[10] << 1) | CHG_Sts.b0;
    
    //////////////////////////////////////////
    //      Spare_Sts1 Byte Package       //
    //////////////////////////////////////////
    Tx_Char[11] = Spare_Sts1.b7;             
    Tx_Char[11] = (Tx_Char[11] << 1) | Spare_Sts1.b6;
    Tx_Char[11] = (Tx_Char[11] << 1) | Spare_Sts1.b5;
    Tx_Char[11] = (Tx_Char[11] << 1) | Spare_Sts1.b4;
    Tx_Char[11] = (Tx_Char[11] << 1) | Spare_Sts1.b3;
    Tx_Char[11] = (Tx_Char[11] << 1) | Spare_Sts1.b2;
    Tx_Char[11] = (Tx_Char[11] << 1) | Spare_Sts1.b1;
    Tx_Char[11] = (Tx_Char[11] << 1) | Spare_Sts1.b0; 
}

//                    Module -> Module LCD Transmit-KHJ                       //
//============================================================================//
//                    Continuous Data Protocol Packet                         //
//  '<' + 'U'+ '0' + 'C' + TxBuf[0~7](Vlotage[0~3] / Ampere[4~7]) /  //
//  Temperature[8~11] / Input_Volt_R[12~15] / Input_Volt_S[16~19] /           //
//  Input_Volt_S[20~23]) / STS[24] / SP1[25] / SP2[26] / SP2[27] + '>'        //
//----------------------------------------------------------------------------//
//                    Response Data Protocol Packet                           //
// '<' + 'U'+ '0' + 'R' + TxBuf[4~27](Batt_OV[4~7] / Batt_UV[8~11]   //
//  / RST_Time[12~15] / Spare2[16~19] / Spare3[20~23] / Spare4[24~27])        //
//  + OP_MODE + OP_CMD + '>'                                                  //
//============================================================================//
void Send_Serial(unsigned int ival,unsigned char pindex,unsigned char ch)
{
	unsigned int tmpval = 0;
	unsigned char a = 0, b = 0, c = 0, d = 0, e = 0;
	//==================================================================// 
	//TxBuf[0~7] ===> 'C' [0~3] / [4~7]									//
	//TxBuf[0~19] ===> 'R' [1~3] / [4~7] / [8~11] / [12~15] / [16~19]	//
	//==================================================================//
	tmpval = ival;
	a=__builtin_divud((long)tmpval,10000);
	b=__builtin_divud((long)(tmpval - (__builtin_muluu(a,10000)) ),1000);
	c=__builtin_divud((long)(tmpval - (__builtin_muluu(a,10000) + __builtin_muluu(b,1000)) ),100);
	d=__builtin_divud((long)(tmpval - (__builtin_muluu(a,10000) + __builtin_muluu(b,1000) + __builtin_muluu(c,100)) ),10);
	e= tmpval%10;
	
    switch(ch)		//KHJ - RS-232 & RS-485
    {
        case 'R':		//Tx Type - Response
            switch(pindex)
            {
                case 1:
                    TxBuf[4]= b + 0x30;                 //Batt_OV/1000 => 1000
                    TxBuf[5]= c + 0x30;                 //Batt_OV/100 => 100
                    TxBuf[6]= d + 0x30;                 //Batt_OV/10  => 10
                    TxBuf[7]= e + 0x30;                 //Batt_OV%1  => 1
                break;
                
                case 2:
                    TxBuf[8]= b + 0x30;                 //Batt_UV/1000 => 1000
                    TxBuf[9]= c + 0x30;                 //Batt_UV/100 => 100
                    TxBuf[10]= d + 0x30;                //Batt_UV/10  => 10
                    TxBuf[11]= e + 0x30;                //Batt_UV%1  => 1
                break;
                
                case 3:
                    TxBuf[12]= b + 0x30;                //RST_Time/1000 => 1000
                    TxBuf[13]= c + 0x30;                //RST_Time/100 => 100
                    TxBuf[14]= d + 0x30;                //RST_Time/10  => 10
                    TxBuf[15]= e + 0x30;                //RST_Time%1  => 1
                break;
                
                case 4:
                    TxBuf[16]= b + 0x30;                //Spare2/1000 => 1000
                    TxBuf[17]= c + 0x30;                //Spare2/100 => 100
                    TxBuf[18]= d + 0x30;                //Spare2/10  => 10
                    TxBuf[19]= e + 0x30;                //Spare2%1  => 1
                break;
                
                case 5:
                    TxBuf[20]= b + 0x30;                //Spare3/1000 => 1000
                    TxBuf[21]= c + 0x30;                //Spare3/100 => 100
                    TxBuf[22]= d + 0x30;                //Spare3/10  => 10
                    TxBuf[23]= e + 0x30;                //Spare3%1  => 1
                break;
                
                case 6:
                    TxBuf[24]= b + 0x30;                //Spare4/1000 => 1000
                    TxBuf[25]= c + 0x30;                //Spare4/100 => 100
                    TxBuf[26]= d + 0x30;                //Spare4/10  => 10
                    TxBuf[27]= e + 0x30;                //Spare4%1  => 1
                break;
                                              
                default:
                break;
            }
        break;   
        
        case 'C':		//Tx Type - Continuous
            switch(pindex)
            {
                case 1:
                    TxBuf[4]= b + 0x30;                 //Batt_Volt/1000 => 1000
                    TxBuf[5]= c + 0x30;                 //Batt_Volt/100 => 100
                    TxBuf[6]= d + 0x30;                 //Batt_Volt/10  => 10
                    TxBuf[7]= e + 0x30;                 //Batt_Volt%1  => 1
                break;
                
                case 2:
                    TxBuf[8]= b + 0x30;                 //Batt_Amp/1000 => 1000
                    TxBuf[9]= c + 0x30;                 //Batt_Amp/100 => 100
                    TxBuf[10]= d + 0x30;                //Batt_Amp/10  => 10
                    TxBuf[11]= e + 0x30;                //Batt_Amp%1  => 1
                break;
                
                case 3:
                    TxBuf[12]= b + 0x30;                //Load_Volt/1000 => 1000
                    TxBuf[13]= c + 0x30;                //Load_Volt/100 => 100
                    TxBuf[14]= d + 0x30;                //Load_Volt/10  => 10
                    TxBuf[15]= e + 0x30;                //Load_Volt%1  => 1
                break;
                
                case 4:
                    TxBuf[16]= b + 0x30;                //Load_Amp/1000 => 1000
                    TxBuf[17]= c + 0x30;                //Load_Amp/100 => 100
                    TxBuf[18]= d + 0x30;                //Load_Amp/10  => 10
                    TxBuf[19]= e + 0x30;                //Load_Amp%1  => 1
                break;
                                              
                default:
                break;
            }
        break;   
     
        default:
        break;
    }

}

//=============================
// 	Interrupt Service Routine
//=============================

//=========================== RS232 - For Debug ================================
void __attribute__((interrupt,auto_psv)) _U1RXInterrupt()		// UART1
{ //every 3second request from snmp agent
	unsigned char i = 0, RcvData = 0;
    unsigned int tmp = 0, Checksum = 0;
    
    //Clear Overflow Flag - KHJ
    if(U1STAbits.OERR == 1)                 
    {
        U1STAbits.OERR = 0;
    }
    
    //RXbuff 
    RcvData = U1RXREG; 
    
    //Start of Transmission Packet - SOT => 0x5B '['
    if(RcvData == 0x5B && EOT_Flag_Debug == 0)             
	{
        f_SData_Debug = 1;                  //start serial data flag 
		Rindex_Debug = 0;
        EOT_Flag_Debug = 1;
	}
    
    //EOT of Transmission Packet - EOT => 0x5D ']'
	else if((f_SData_Debug == 1) && (RcvData == 0x5D) && (Rindex_Debug == 34))
	{
        //Checksum Check
        for(i=0; i<32; i++)
        {
            Checksum ^= RxBuf_Debug[i];
        }   
        tmp = ((RxBuf_Debug[32] - 0x30) << 4) & 0xF0;
        tmp = tmp + ((RxBuf_Debug[33] - 0x30) & 0x0F);           
        if(tmp == Checksum)
        {
            f_EData_Debug = 1;
            EOT_Flag_Debug = 0;
        }
        else
        {
            f_SData_Debug = 0;
            f_EData_Debug = 0;
            EOT_Flag_Debug = 0;
        }	
	}		
	RxBuf_Debug[Rindex_Debug]= RcvData;
	Rindex_Debug++;
    
	//Occur overflow -> Reset buffer//
	if(Rindex_Debug > 36)
	{	
		Rindex_Debug=0;	
		f_SData_Debug=0;
		f_EData_Debug=0;
        EOT_Flag_Debug = 0;
	}
	IFS0bits.U1RXIF=0;
}

//RS232 - TX INT
void __attribute__((__interrupt__, no_auto_psv)) _U1TXInterrupt()   
{
	IFS0bits.U1TXIF=0;
}  

//=========================== RS232 - For LCD ==================================
void __attribute__((__interrupt__, no_auto_psv)) _U2RXInterrupt()
{
    unsigned char i = 0, RcvData = 0;
    unsigned int tmp = 0, Checksum = 0;
    //Clear Overflow Flag - KHJ
    if(U2STAbits.OERR == 1)     
    {
        U2STAbits.OERR = 0;
    }
    
    //RXbuff 
    RcvData = U2RXREG; 
    
    //Start of Transmission Packet - SOT => 0x5B '['
    if(RcvData == 0x5B && EOT_Flag == 0)             
	{
        f_SData = 1;                  //start serial data flag 
		Rindex = 0;
        EOT_Flag = 1;
	}
   
    //EOT of Transmission Packet - EOT => 0x5D ']'
	else if((f_SData == 1) && (RcvData == 0x5D) && (Rindex == 34))
	{
        //Checksum Check
        for(i=0; i<32; i++)
        {
            Checksum ^= RxBuf[i];
        }   
        tmp = ((RxBuf[32] - 0x30) << 4) & 0xF0;
        tmp = tmp + ((RxBuf[33] - 0x30) & 0x0F);           
        if(tmp == Checksum)
        {
            f_EData = 1;
            EOT_Flag = 0;
            tmr1_2 = 0;
        }
        else
        {
            f_SData = 0;
            f_EData = 0;
            EOT_Flag = 0;
            tmr1_2 = 0;
        }	
	}		
	RxBuf[Rindex]= RcvData;
	Rindex++;
	
    //Occur overflow -> Reset buffer//
	if(Rindex > 36)	
	{	
		Rindex=0;	
		f_SData=0;
		f_EData=0;
        EOT_Flag = 0;
        tmr1_2 = 0;
	}
	IFS1bits.U2RXIF = 0;

}  
//RS232 - TX INT
void __attribute__((__interrupt__, no_auto_psv)) _U2TXInterrupt()   
{
	IFS1bits.U2TXIF=0;
}  


