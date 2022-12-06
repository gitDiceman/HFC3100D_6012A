/* 
 * File:   ADC.h
 * Author: KHJ
 *
 * Created on 2018? 2? 9? (?), ?? 6:48
 */

//NOTE: The actual sampling rate realized may be 7998.698 Hz
//      due to a small round off error. Ensure you provide the
//      true sampling rate to dsPICworks if you are trying to plot
//      the sampled or filtered signal.
#define SAMPLINGRATE    8000
#define SAMPCOUNT       (FCY/SAMPLINGRATE)+1

#define ADC_ACV_R		ADCBUF0			
#define ADC_ACV_S		ADCBUF1			
#define ADC_ACV_T		ADCBUF2		
#define ADC_ACI_R		ADCBUF3
#define ADC_ACI_S		ADCBUF4	
#define ADC_ACI_T       ADCBUF5	
                                            // y = ax + b : y is DBM Value, x is ADC Raw Data
#define ADC_Gain      0.2	             // AC Input Votage ADC Gain

extern void ADC_Init(void);
extern int ADC_Conv(unsigned int Source_Data, int MAF_Data);

extern int   ACV_R_Raw, ACV_S_Raw, ACV_T_Raw, ACI_R_Raw, ACI_S_Raw, ACI_T_Raw;
extern float ACV_R, ACV_S, ACV_T, ACI_R, ACI_S, ACI_T;
