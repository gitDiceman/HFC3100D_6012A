/* 
 * File:   ADC.c
 * Author: KHJ
 *
 * Created on 2018? 2? 9? (?), ?? 6:48
 */

#include    <p30F6012A.h>
#include    "Definition.h"
#include    "ADC.h"

int   ACV_R_Raw, ACV_S_Raw, ACV_T_Raw, ACI_R_Raw, ACI_S_Raw, ACI_T_Raw;
float ACV_R = 0, ACV_S = 0, ACV_T = 0, ACI_R = 0, ACI_S = 0, ACI_T = 0;

//Functions and Variables with Global Scope:
void ADC_Init(void);
int ADC_Conv(unsigned int Source_Data, int MAF_Data);
void __attribute__((__interrupt__)) _ADCInterrupt(void);

//Functions:
//ADC_Init() is used to configure A/D to convert 16 samples of 1 input
//channel per interrupt. The A/D is set up for a sampling rate of 8KHz
//Timer3 is used to provide sampling time delay.
//The input pin being acquired and converted is AN0~AN5.
void ADC_Init(void)         //Initialize the A/D converter
{    
    ADPCFG = 0x00C0;	//analog AN0~AN5 digital input pin 	//1 is digital
	ADCON1 = 0x00E4;    //samples CH0; SSRC = 111 (auto cONvert)
	ADCHS  = 0x0000;
	ADCSSL = 0xFFFF;	//A/D input scan select (AN0~AN15)//0b0000-0000-0011-1111
	//ADCON3 = 0x0109;	//auto sample time = TAD, TAD = (9+1)*TCY/2		//30MIPS, Tcy =33.33ns    00000
	ADCON3 = 0x0104;	//auto sample time = TAD, TAD = (4+1)*TCY/2		//7.5MIPS,Tcy =133.33ns
	ADCON2 = 0x043C;	//cONverts CH0, interrupt per 16 AD cONversiON
	IPC2bits.ADIP = 0;	//Interrupt Priority -> Not Use
	IFS0bits.ADIF = 0;	//Interrupt Flag Status
	IEC0bits.ADIE = 0;	//Interrrupt Disable
	ADCON1bits.ADON = 1;	//AD Module ON
}

int ADC_Conv(unsigned int Source_Data, int MAF_Data)
{
    UINT16	pre_Data, new_Data;

    pre_Data = sdiv((long)((ADCNT * MAF_Data)-MAF_Data),ADCNT);		
	new_Data = sdiv((long)(Source_Data),ADCNT);
	MAF_Data = pre_Data + new_Data;

    return MAF_Data;
}
//Timer Interrupt _ADCInterrupt() is the A/D interrupt service routine (ISR).
//The routine must have global scope in order to be an ISR.
//The ISR name is chosen from the device linker script.
void __attribute__((__interrupt__, no_auto_psv)) _ADCInterrupt()
{
        //Clear the Timer3 Interrupt Flag
        IFS0bits.T3IF = 0;
        //Clear the A/D Interrupt flag bit or else the CPU will
        //keep vectoring back to the ISR
        IFS0bits.ADIF = 0;
        //Copy the A/D conversion results to variable     
}



