/*
**********************************************************
* PROJECT : HFC3100D MAIN CONTROL
* FILE NAME: ADC.h
* DEVICE : dsPIC30F6012A
* COMPILER : MPLAB-C
* REVISION HISTORY
* 1) 2018.03.07 (V1.0)
* 
********************************************************** */

//NOTE: The actual sampling rate realized may be 7998.698 Hz
//      due to a small round off error. Ensure you provide the
//      true sampling rate to dsPICworks if you are trying to plot
//      the sampled or filtered signal.
#define SAMPLINGRATE    8000
#define SAMPCOUNT       (FCY/SAMPLINGRATE)+1

#define ADC_LOAD		ADCBUF8			
#define ADC_OUTP		ADCBUF9			
#define ADC_LOADI		ADCBUFA		
#define ADC_BATTI		ADCBUFB
#define ADC_TMP			ADCBUFC
                                            // y = ax + b : y is DBM Value, x is ADC Raw Data
#define ADC_Gain        0.2	             // AC Input Votage ADC Gain

extern void ADC_Init(void);
extern void Adc_Sensing(void);

extern UINT32   Ad_Sum_Buffer[11];			//ADC sum buffer
extern UINT8    ad_cnt;
extern UINT16 	_avgLdV,_avgOutV,_avgLdI,_avgBatI,_avgBatTmp;
extern UINT16 	avgLdV,avgLdI,avgOutV,avgBatI,avgBatTmp;
extern UINT8	adcerr1,adcerr2,adcerr3,adcerr4,adcerr5,adcerr6,adcerr7,adcerr8,adcerr9,adcerr10;
extern UINT8    LDV[3];	//load voltage
extern UINT8    OUTV[3];	//out voltage
extern UINT8    LDI[3];	//load current
extern UINT8    BATI[3];	//battery (dis)charge current
extern UINT8    BATTMP[3];	// battery temperature.

extern UINT16 	alm_ldv_hi, alm_ldv_low;
extern UINT16 	alm_btv_hi, alm_btv_low ;
extern UINT16	Cur_Lmt, Mod_Cur_Lmt, Bat_Cur_Lmt,Aci_Lmt;
extern UINT8	adtimer;
extern FLOAT32  Batt_Volt;
extern FLOAT32  Batt_Amp;
extern FLOAT32  Load_Volt;
extern FLOAT32  Load_Amp;



