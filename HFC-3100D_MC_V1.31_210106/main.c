/*
******************************************************************************************
* PROJECT : HFC-3100D MAIN CONTROL
* FILE NAME: MAIN.c
* DEVICE : dsPIC30F6012A
* COMPILER : MPLAB - C V3.12 
* REVISION HISTORY
* 2018.03.07 Draft Ver.
* ----------------------------------------------------------------------------------------
* HFC3100D_MC_V1.31_2021.01.06
* mplabx5x,6x, 
* mplabC30 V3.31
******************************************************************************************
*/
#include 	<p30f6012A.h>		
#include    "Definition.h"
#include 	"typedef.h"				//????? ??
#include 	"Memory.h"				
#include    "Initialization.h"
#include    "RS232.h"
#include    "SPI_RS485.h"
#include	"ADC.h"
#include    "LED.h"
#include    "Operation.h"

//--------------------------EXT OSC Device CONFIGURATION-------------------------------
_FOSC(CSW_FSCM_OFF & HS2_PLL4)				//30MIPs at 15MHz crystal
//_FWDT(WDT_ON & WDTPSA_8 & WDTPSB_16)		//watchdog Enable//2ms * 8 * 16	 = 256ms	
_FWDT(WDT_OFF)							//watchdog disable 
_FBORPOR(PBOR_ON & BORV_45 & PWRT_64 & MCLR_DIS)
           
//====================
//	Global Variables
//====================
UINT16  tmr1_1 = 0;                     // Modules Sequencailly Request Tic Time 
UINT16  tmr1_2 = 0;                     // Modules Sequencailly Request Tic Time 
        
UINT16	tmr2_1 = 0;
UINT16 	tmr2_3 = 0;
UINT16 	tmr2_4 = 0;
UINT16 	tmr2_5 = 0;
UINT16 	tmr2_6 = 0;

UINT16 	tmr2_7 = 0;
UINT16 	tmr2_8 = 0;
UINT16 	tmr2_9 = 0;

//======================================================

//====================//
// Start Main Program //
//====================//
int main()
{
    RCONbits.SWDTEN = 0;        //watchdog Timer Stop
    while(OSCCONbits.LOCK!=1) {};			//wait PLL Lock.
    //==================
	// Device Initialize
	//==================
	Init_Port();			//elapse time 10us
	Set_EEPROM();			//elapse time 1.6us		//@system.h//
    EEPROM_Init();          //EEPRom Initialize    
	ADC_Init();             //ADC Initialize
	// elapse time 750us//==================================================
	Init_Timer1(); 					//interrupt timer set interval 1ms
	Init_Timer2();					//interrupt timer set interval 1ms	
	//---------------------
	Init_SPI();	
	Init_Max3100();
    System_Check();
	//============================================================================
    // elapse time 60us //
    Init_Interrupt();		
	Init_Uart1();			
    Init_Uart2();
	/***************************************************
	*               Main Infinite Loop                 *  
	***************************************************/
	while(1)
	{
        // Dual Type
        if(OP_Type == 1)            
        {
            if(OP_MODE != 0 && OP_CMD != 0)
            {
                OP_MODE = 0;
                OP_CMD = 0;
                Set_OK = 1;
                Request_Send();                 // Forcing Operation Command Send to Modules
            }
        }
        
        // Stand-Alone Type
        else                        
        {
            OP_Logic();             // Operation Logic
        }

        Serial_Process();           // RS232 Processing
        Serial_Process_485();       // RS485 Processing        
        
        // Recovery Restart Timer Set : Minimum 60[sec]  
        if(RST_Time < 1.0) // Add Recovery Time OFF(Minimum 3sec))
        {
            if(tmr2_2 >= 7000)			
            {
                tmr2_2 = 0;
                RST_Timer++;
                if(RST_Timer > RST_Time)
                {
                    RST_Timer = RST_Time;
                }
            }
        }
        else
        {
            if(tmr2_2 >= 60000)			
            {
                tmr2_2 = 0;
                RST_Timer++;
                if(RST_Timer > RST_Time)
                {
                    RST_Timer = RST_Time;
                }
            }
        }
       
        
        // 10ms	Timer Processing
        if(tmr2_3 >= 10)
        {
            tmr2_3 = 0;
            Adc_Sensing();          // ADC Processing
            GetNfbState1();         // NFB Status Processing
            GetNfbState2();         // NFB Status Processing
            GetContact();           // Contact Status Processing
        }
       
        
        // RS485 Communication Terminate Check
        if(tmr2_4 >= 30000)
        {
            tmr2_4 = 30000;
            Alram_Sts[1].b7 = 0; 
        }  

        if(tmr2_5 >= 30000)
        {
            tmr2_5 = 30000;
            Alram_Sts[2].b7 = 0;
        }  
	}
	return 0;	
}
    
void __attribute__((interrupt,auto_psv)) _T1Interrupt()	// Not work
{
	IFS0bits.T1IF = 0;
}

void __attribute__((interrupt,auto_psv)) _T2Interrupt()	//interval is 1ms//
{
    tmr1_1++;               //Modules Sequencially Request Tic Time Increase
    tmr1_2++;               //Modules Sequencially Request Tic Time Increase
    if(tmr1_1 > 10000)
    {
        tmr1_1 = 0;
    }
    if(tmr1_2 > 10000)
    {
        tmr1_2 = 10000;
    }
    
	tmr2_1++;
	tmr2_2++;				// Timer is Used for Recovery Tic Time
	tmr2_3++;
    tmr2_4++;
    tmr2_5++;
    tmr2_6++;
    tmr2_7++; 
    tmr2_8++; 
    tmr2_9++;
   
	//---------------------------------------------------	
	if(tmr2_1 >= 100)			//100ms 
	{
		tmr2_1 = 0;
			//		
		if(fBzOn2==1)			//setting ok 
		{			
			BZ_TOGGLE();			
			
			if(++BzCnt>6)
			{
				BzCnt=0;
				fBzOn2=0;
				BZ_OFF();	
			}	
		}
		else if(fBzOn1==1)		// alarm buzzer	
		{	
			if(Buzzer_Mode=='Y')
			{	
				fBzOn1_ing++;
				if(fBzOn1_ing>4)
				{	
					fBzOn1_ing=0;
					BZ_TOGGLE();
				}	
			}	
			else
			{	
				BZ_OFF();	
			}	
		}
		else
		{
			BZ_OFF();	
		}
	}	
	//
	
	IFS0bits.T2IF = 0;			
}

//------------------------------------------------------------------------


