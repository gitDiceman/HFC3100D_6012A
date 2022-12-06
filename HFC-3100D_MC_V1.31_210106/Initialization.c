/*
**********************************************************
* PROJECT : HFC3100D MAIN CONTROL
* FILE NAME: Initialization.c
* DEVICE : dsPIC30F6012A
* COMPILER : MPLAB-C
* REVISION HISTORY
* 1) 2018.03.07 (V1.0)
* 
********************************************************** */
#include    <p30F6012A.h>
#include    "Definition.h"
#include    "typedef.h"
#include    "SPI_RS485.h"
#include    "Initialization.h"


void Init_Port(void);
void Init_Timer1(void);
void Init_Timer2(void);
void Init_Interrupt(void);

void Init_Port(void)
{
	//=================================================================
	// b15-b14-b13-b12|b11-b10-b09-b08|b07-b06-b05-b04|b03-b02-b01-b00 // 
	// Rds-mOE-NOE-Btp|BTi-LDi-OuV-LdV|PgD-PgC-aiT-aiS|aiR-avT-avS-avR//
	//   0-  0-  0-  A|  A-  A-  A-  A|  x-  x-  A-  A|  A-  A-  A-  A
	//=================================================================
	TRISB = 0x1FFF;
	
	//=================================================================
	// b15-b14-b13-b12|b11-b10-b09-b08|b07-b06-b05-b04|b03-b02-b01-b00 // 
	//   X-MD5-MD6-  X|  X-  X-  X-  X|  X-  X-  X-  X|  X-fd7-fn6-  X	
	//   1-  1-  1-  1|  1-  1-  1-  1|  1-  1-  1-  1|  1-  0-  0-  1 
	//=================================================================
	TRISC = 0xfff9;
	
	//=================================================================
	// b15-b14-b13-b12|b11-b10-b09-b08|b07-b06-b05-b04|b03-b02-b01-b00 // 
	//   X-  X-  X-  X| CS-flt- eq-fal|wr2-wr1-mod-MD1|MD2-MD3-MD4-MD7	
	//   1   1   1   1|  0   0   0   0|  0   0   0   1|  1   1   1   1
	//=================================================================
	TRISD = 0xf01f;
	
	//=================================================================
	// b15-b14-b13-b12|b11-b10-b09-b08|b07-b06 -b05  -b04  |b03  -b02  -b01  -b00 // 	
	//   X-  X-  X - X|  x-  x-  x-  x|  x-spiI-Lcdtx-Lcdrx|snmpT-snmpR-CanTx-CanRx
	//   1   1   1   1|  1   1   1   1|  1    1     0     1|    0     1     0     1
	//=================================================================
	TRISF = 0xffd4;			//0100//
	
	//=================================================================
	// b15-b14-b13-b12|b11-b10-b09-b08   |b07  -b06  -b05-b04|b03-b02-b01-b00 // 
	// fd5-fd2-fd4-fd3|  x-  x- BZ-spiOut|spiIn-spick-  x-  x|scd-scl-fd0-fd1 //fd->fnd 
	//   0   0   0   0|  1   1   0      0|    1     0   1   1|  0   0   0   0       //
	//=================================================================
	TRISG = 0x0cb0;				
	//a   b   c  d   e   f
	//10 11 12 13 14 15
	//------------------------------------------------------------------------------------------	
	PORTB = 0;
	PORTC = 0;
	PORTD = 0x0800;	  //b12 Max3100 chip select Active Low
	PORTF = 0;
	PORTG = 0;	
	//SRbits.IPL = 3;									//interrupt priority
}

void Init_Timer1(void)			//1ms
{
	TMR1 = 0;                   //count
	PR1 = TMR1_PERIOD - 1;		//period
	T1CONbits.TCS = 0;          //cONtrol, internal clock(Fosc/4)
	T1CONbits.TCKPS = 1;		//1:8 prescale
    IFS0bits.T1IF = 0;          //Flag Clear
	IEC0bits.T1IE = 0;          //Interrupt disable
	T1CONbits.TON = 1;          //cONtrol, starts timer
}

void Init_Timer2(void)			//1ms
{
	TMR2 = 0;                   //count
	PR2 = TMR2_PERIOD - 1;		//period
	T2CONbits.TCS = 0;          //cONtrol, internal clock(Fosc/4)
	T2CONbits.TCKPS = 1;		//1:8 prescale
	IFS0bits.T2IF = 0;          //Flag Clear
	IEC0bits.T2IE = 0;          //Interrupt enable
	T2CONbits.TON = 1;          //cONtrol, starts timer
}

void Init_Interrupt(void)
{
	_NSTDIS = 1;                //interrupt nesting disable 
	//------------------------------------------------------------------------------
	IPC0bits.INT0IP = 3;		//External Interrupt priority    
	//IPC0bits.T1IP = 5;        //Timer1 Interrupt priority(100us)
	IPC1bits.T2IP = 4;          //Timer2 Interrupt priority(1ms)	
	//-------------------------------------------------------------------------------	
	IFS0bits.T1IF = 0;          //Timer1 Interupt Flag
	IFS0bits.T2IF = 0;          //Timer2 Interupt Flag				
	IFS0bits.INT0IF = 0;        //Interrupt 0 Flag
	//-----------------------------------------------------------------
	IEC0bits.ADIE = 0;          //ADC Interrrupt disable
	IEC1bits.SPI2IE = 0;		//SPI Interrupt Disable
	IEC0bits.T1IE = 0;          //Timer1 Interrupt Disable
	IEC0bits.T2IE = 1;          //Timer2 Interrupt enable
	IEC0bits.INT0IE = 1;		//External0 Interrupt enable(Max3100 trigger)
	return;
}	


