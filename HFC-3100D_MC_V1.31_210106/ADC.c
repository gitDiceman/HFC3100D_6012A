/*
**********************************************************
* PROJECT : HFC3100D MAIN CONTROL
* FILE NAME: ADC.c
* DEVICE : dsPIC30F6012A
* COMPILER : MPLAB-C
* REVISION HISTORY
* 1) 2018.03.07 (V1.0)
* 
********************************************************** */

#include    <p30F6012A.h>
#include    "Definition.h"
#include	"typedef.h"
#include    "ADC.h"
#include    "RS232.h"

//Functions and Variables with Global Scope:
void ADC_Init(void);
void Adc_Sensing(void);


UINT32 	Ad_Sum_Buffer[11]={0,};             //ADC sum buffer
UINT8	ad_cnt;
UINT16 	_avgLdV,_avgOutV,_avgLdI,_avgBatI,_avgBatTmp;
UINT16 	avgLdV,avgLdI,avgOutV,avgBatI,avgBatTmp;
UINT8	adcerr1=0,adcerr2=0,adcerr3=0,adcerr4=0,adcerr5=0,adcerr6=0,adcerr7=0,adcerr8=0,adcerr9=0,adcerr10=0;
UINT16 	alm_ldv_hi, alm_ldv_low;
UINT16 	alm_btv_hi, alm_btv_low ;
UINT16	Cur_Lmt, Mod_Cur_Lmt, Bat_Cur_Lmt,Aci_Lmt;
UINT8	adtimer;



//Functions:
//ADC_Init() is used to configure A/D to convert 16 samples of 1 input
//channel per interrupt. The A/D is set up for a sampling rate of 8KHz
//Timer3 is used to provide sampling time delay.
//The input pin being acquired and converted is AN0~AN5.

//Initialize the A/D converter
void ADC_Init(void)
{    
    ADPCFG = 0x0000|0xE0C0;     //analog AN0~AN15 digital input pin 	//1 is digital
	ADCON1= 0x00E4;             //samples CH0; SSRC = 111 (auto cONvert)
	ADCHS  = 0x0000;
	ADCSSL = 0xFFFF;            //A/D input scan select (AN0~AN15)//0b0001-1111-0011-1111
	ADCON3 = 0x0104;            //auto sample time = TAD, TAD = (4+1)*TCY/2		//7.5MIPS,Tcy =133.33ns
	ADCON2 = 0x043C;            //cONverts CH0, interrupt per 16 AD cONversiON
	IPC2bits.ADIP = 0;          //Interrupt Priority -> Not Use
	IFS0bits.ADIF = 0;          //Interrupt Flag Status
	IEC0bits.ADIE = 0;          //Interrrupt Disable
	ADCON1bits.ADON = 1;        //AD Module ON
}

//check periodically 10ms
void Adc_Sensing()
{
	UINT8 		a,b,c,d;
	UINT16 		AdBuf;
	//------------------------------------------------------
	UINT16 		Loadv_s,Outp_s,Loadi_s,Bati_s,Tmp_s;
	UINT16		pre_Data,new_Data,filter_Data;
	//------------------------------------------------------			
	Loadv_s		= ADC_LOAD;             //Load Voltage ADC CH
	Outp_s		= ADC_OUTP;             //Output Voltage ADC CH
	Loadi_s		= ADC_LOADI;            //Load Current ADC CH
	Bati_s		= ADC_BATTI;            //Battary Current ADC CH
	Tmp_s		= ADC_TMP;              //Temperature ADC CH
	//------------------------------------------------------
	if(Loadi_s>=10)Loadi_s=Loadi_s - LDI_OFFSET;        // ADC Reference Offset Voltage
    
	// Average Filtering (Normal Average)
	Ad_Sum_Buffer[0]+=(Loadv_s);        // Integrate Load Voltage for Avarage Filter 
	Ad_Sum_Buffer[1]+=(Outp_s);         // Integrate Output Voltage for Avarage Filter 
	ad_cnt++;                           // AD Count Increase
	if(ad_cnt>=32)                      // Filtering No.
	{
        //Average Calculation 
		_avgLdV = (UINT16)(Ad_Sum_Buffer[0]>>5);        // Avg_Load Voltage = Voltage / 2^5(32) 
		_avgOutV = (UINT16)(Ad_Sum_Buffer[1]>>5);       // Avg_Output Voltage = Voltage / 2^5(32)
		ad_cnt=0;                                       // AD count Reset
		Ad_Sum_Buffer[0]=0;                             // AVG Buffer Reset
		Ad_Sum_Buffer[1]=0;                             
	}		

    // Load Current MAF(Moving Average Filter)
	pre_Data = sdiv((long)((ADCNT * _avgLdI)-_avgLdI),ADCNT);		
	new_Data = sdiv((long)(Loadi_s),ADCNT);
	filter_Data = pre_Data + new_Data;
	_avgLdI = filter_Data;				//final value		
	// Battary Current MAF(Moving Average Filter)
	pre_Data = sdiv((long)((ADCNT * _avgBatI)-_avgBatI),ADCNT);		
	new_Data = sdiv((long)(Bati_s),ADCNT);
	filter_Data = pre_Data + new_Data;
	_avgBatI = filter_Data;				//final value
	//Battery Temperature MAF(Moving Average Filter)	
	pre_Data = sdiv((long)((ADCNT * _avgBatTmp)-_avgBatTmp),ADCNT);		
	new_Data = sdiv((long)(Tmp_s),ADCNT);
	filter_Data = pre_Data + new_Data;
	_avgBatTmp = filter_Data;				//final value		
	//=================================================	
     //ADC Converting Real Value - Load Voltage 
	AdBuf	= 	sdiv((long)_avgLdV*100,167);	
	avgLdV=AdBuf;
	// Check
	if(adcerr7>CHKERRCNT)
	{
		adcerr7=CHKERRCNT+1;
		Load_Volt=avgLdV*0.1;
	}
	else if(avgLdV>=alm_ldv_hi || avgLdV<=alm_ldv_low)
	{
		++adcerr7;
	}
	else	
	{
		adcerr7=0;
	}
	Load_Volt=avgLdV*0.1;

    //ADC Converting Real Value - Output Voltage		
	AdBuf = sdiv((long)_avgOutV*100,167);	
	avgOutV = AdBuf;
	if(adcerr8>CHKERRCNT)
	{
		adcerr8=CHKERRCNT+1;
		Batt_Volt=avgOutV*0.1;
	}
	else if(avgOutV>=alm_btv_hi || avgOutV<=alm_btv_low)
	{
		++adcerr8;
	}
	else
	{
		adcerr8=0;
		Batt_Volt=avgOutV*0.1;
	}
	Batt_Volt=avgOutV*0.1;
    
    //Check Battery Over Voltage
    if(Batt_Volt >= Set_Batt_OV)
    {
        Batt_OV_STS = 1;
    }
    //Check Battery Under Voltage
    else if(Batt_Volt <= Set_Batt_UV)
    {
        Batt_UV_STS = 1;
    }
    else
    {
        Batt_OV_STS = 0;
        Batt_UV_STS = 0;
    }
    
	//ADC Converting Real Value - Load Current
	AdBuf	= 	sdiv((long)_avgLdI * 10,15);	//100A 
	avgLdI = AdBuf;

	if(adcerr9>CHKERRCNT)
	{
		adcerr9=CHKERRCNT+1;
		a = (avgLdI/1000);b = (avgLdI % 1000)/100;	c = (avgLdI % 100)/10;  d = (avgLdI % 10);
		Load_Amp = avgLdI*0.1;
	}
	else if(avgLdI >=Cur_Lmt)
	{
		++adcerr9;
	}
	else
	{
		adcerr9=0;
		a = (avgLdI/1000);b = (avgLdI % 1000)/100;	c = (avgLdI % 100)/10;  d = (avgLdI % 10);
		Load_Amp = avgLdI*0.1;
	}
    
    //ADC Converting Real Value - Battery Temp
	AdBuf	= 	sdiv((long)_avgBatTmp*10,57);	
	avgBatTmp = AdBuf;	
	a = (avgBatTmp/1000);b = (avgBatTmp % 1000)/100;c = (avgBatTmp % 100)/10;d = (avgBatTmp % 10);		
	AdBuf=_avgBatI;

    //Charge state Check - Initial value 2020 
	if(AdBuf >= BATI_OFFSET)		
	{	
		Batt_Mode_STS = 0;			//0 is Charge 
		AdBuf=AdBuf-BATI_OFFSET;
		if(AdBuf<10)
		{	
			AdBuf=0;
		}	
	}	
    //Discharge state
	else    
	{	
		Batt_Mode_STS = 1;			//1 is Dis-Charge
		AdBuf=BATI_OFFSET-AdBuf;
		if(AdBuf<10)
		{	
			AdBuf=0;		
			Batt_Mode_STS=0;
		}	
	}
				
	//ADC Converting Real Value - Battery Current
	avgBatI = sdiv((long)(AdBuf),16);			//CT 100A		

    //Forcing Change Battery Status 
	if(avgBatI <= 1) avgBatI = 0;                   // Forcing Change Zero Current for Minimun Current
	if(avgBatI == 0) Batt_Mode_STS = 0;         	// Battery Status 3-state Define //
	
	if(adcerr10>CHKERRCNT)
	{
		adcerr10=CHKERRCNT+1;
		a = (avgBatI/1000);
		b = (avgBatI % 1000)/100;	c = (avgBatI % 100)/10;  d = (avgBatI % 10);
		Batt_Amp = avgBatI;
	}
	else if((10*avgBatI) >= Bat_Cur_Lmt)
	{
		++adcerr10;
	}	
	else
	{
		adcerr10=0;
		a = (avgBatI/1000);
		b = (avgBatI % 1000)/100;	c = (avgBatI % 100)/10;  d = (avgBatI % 10);
		Batt_Amp = avgBatI;
	}
	Batt_Amp = avgBatI;
	adtimer++;
	if(adtimer>200)adtimer=200;
}//end Adc_Sensing()//



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



