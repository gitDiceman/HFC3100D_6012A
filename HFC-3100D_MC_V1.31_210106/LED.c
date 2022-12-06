/*
**********************************************************
* PROJECT : HFC3100D MAIN CONTROL
* FILE NAME: LED.c
* DEVICE : dsPIC30F6012A
* COMPILER : MPLAB-C
* REVISION HISTORY
* 1) 2018.03.09 (V1.0)
* 
********************************************************** */

#include    <p30F6012A.h>
#include	"typedef.h"
#include    "Memory.h"
#include    "Definition.h"
#include    "SPI_RS485.h"
#include	"RS232.h"
#include    "LED.h"

void GetNfbState1(void);
void GetNfbState2(void);
void GetContact(void);

void GetNfbState1()
{
	static	UINT8 NewInput =0xff;
	static	UINT8 OldInput =0xff;
	NFB2_READ=1;
	NFB1_READ=0;		//ACTIVE LOW
	//Delay_Cnt(2); 2013-05-16 오후 1:44:52
	__asm__ volatile ("NOP");
	__asm__ volatile ("NOP");
    
    NewInput = (!_RD4<<7|!_RD3<<6|!_RD2<<5|!_RD1<<4|!_RC14<<3|!_RC13<<2|!_RD0<<1|0x01);   //Module4
	//					    x   | module8| sp2_nfb   | sp1_nfb   | out_Nfb |In_NFB   | bat_nfb  | batfuse| 	
	bNfbState1   &= 	NewInput ^ OldInput;
	bNfbState1   |= 	NewInput & OldInput;
	OldInput 		= 	NewInput;				
				//	
	if(bNfbState1 != bNfbState1Old)//changed Nfb state
	{	
		bNfbState1Old = bNfbState1;
	}	
	NFB1_READ=1;
    
    // Input Voltage NFB Check
    if(_RD4 == 1)
    {
        Alram_Sts[1].b4 = 1;
    }

    if(_RD3 == 1)
    {
        Alram_Sts[2].b4 = 1;
    }

}

void GetNfbState2()
{
	static	UINT8 NewInput =0xff;
	static	UINT8 OldInput =0xff;
	NFB2_READ=0;			//ACTIVE LOW
	NFB1_READ=1;
	//Delay_Cnt(2); 2013-05-16 오후 1:45:39
	__asm__ volatile ("NOP");
	__asm__ volatile ("NOP");
	NewInput = (!_RD4<<7|!_RD3<<6|!_RD2<<5|!_RD1<<4|!_RC14<<3|!_RC13<<2|!_RD0<<1|0x01);
	//					    x   | module7| module6 | module5 | module4| module3| module2| module1| 
	bNfbState2 &= NewInput ^ OldInput;
	bNfbState2 |= NewInput & OldInput;
	OldInput = NewInput;				
	//
	if(bNfbState2 != bNfbState2Old)
	{	
		bNfbState2Old = bNfbState2;
	}	
	NFB2_READ=1;			//active low
	//-------------------------------------------
}

void GetContact()
{
	UINT8 contactState=0;
	contactState = Alram_Sts[1].b6;
    contactState = (contactState << 1) | Alram_Sts[1].b5;
    contactState = (contactState << 1) | Alram_Sts[1].b4;
    contactState = (contactState << 1) | Alram_Sts[1].b3;
    contactState = (contactState << 1) | Alram_Sts[1].b2;
    contactState = (contactState << 1) | Alram_Sts[1].b1;
    contactState = (contactState << 1) | Alram_Sts[1].b0;
    
	if(contactState || LED_Module1_NFB == 0 || LED_DC1_NFB == 0 || LED_Battery_NFB == 0 || LED_Load_NFB == 0 || Module_OV_STS[1] == 1 || Module_UV_STS[1] == 1)
//    if(contactState ||Module_OV_STS[1] == 1 || Module_UV_STS[1] == 1)
	{
        if(tmr2_7 >= 3000)
        {
            tmr2_7 = 3000;
            M1_FAIL_ON();
        }	
	}
	else 
    {
        tmr2_7 = 0;
        M1_FAIL_OFF();
    }
	
	contactState = Alram_Sts[2].b6;
    contactState = (contactState << 1) | Alram_Sts[2].b5;
    contactState = (contactState << 1) | Alram_Sts[2].b4;
    contactState = (contactState << 1) | Alram_Sts[2].b3;
    contactState = (contactState << 1) | Alram_Sts[2].b2;
    contactState = (contactState << 1) | Alram_Sts[2].b1;
    contactState = (contactState << 1) | Alram_Sts[2].b0;
    
	if(contactState || LED_Module2_NFB == 0 || LED_DC2_NFB == 0 || Module_OV_STS[2] == 1 || Module_UV_STS[2] == 1)
	{
        if(tmr2_8 >= 3000)
        {
            tmr2_8 = 3000;
            if(SYS_Type == 0)       M2_FAIL_ON();
            else  M2_FAIL_OFF();  
        }
	}
	else        
    {
        tmr2_8 = 0;
        M2_FAIL_OFF();
    }

	//
	if(Batt_UV_STS)
	{
        if(tmr2_9 >= 3000)
        {
            tmr2_9 = 3000;
            BATT_LOW_ON();
        }    
	}
	else
    {
        tmr2_9 = 0;
        BATT_LOW_OFF();
    }
        
}

