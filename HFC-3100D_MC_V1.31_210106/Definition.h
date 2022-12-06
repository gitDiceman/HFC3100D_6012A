/* 
 * File:   Definition.h
 * Author: IV_Tracer
 *
 * Created on 2018? 3? 7? (?), ?? 4:26
 */
//=================================================================
#define 	FCY  			7500000		//7.5 MIPs, Fosc=30MHz
#define 	TMR1_PERIOD		937//46875		//FCY/(prescaler*20Hz),prescaler=8     50ms
#define 	TMR2_PERIOD		937			//3750		//FCY/(prescaler*1kHz),prescaler=8   	1ms

#define     Batt_UV_Ref     96.0           // 18.02.13 110 - KHJ
#define     Batt_OV_Ref     150.0           // 18.02.13 143 - KHJ
#define     RST_Time_Min    1.0             // Minimum Recovery System Restart Time
#define     RST_Time_Max    60.0            // Maximum Recovery System Restart Time 

#define		LDI_OFFSET		10
#define		ADCNT			15				//moving average counter.
#define		CHKERRCNT		50
#define		BATI_OFFSET		2014
#define     RS485_Period    500             //ms
#define     RS232_RX_Delay  100             //ms


#define     sdiv(a,b)			__builtin_divsd(a,b)

//UART1 Debug mode
//#define DEBUG

