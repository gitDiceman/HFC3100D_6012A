/*
**********************************************************
* PROJECT : HFC3100D MAIN DISPLAY
* FILE NAME: Definition.h
* DEVICE : dsPIC30F6012A
* COMPILER : MPLAB-C
* REVISION HISTORY
* 1) 2018.02.28 (V1.0)
* 
********************************************************** */
#ifndef __DEFINITION_H
#define __DEFINITION_H
extern int read_EE1(int tbl_offset);
extern void write_EE1(int tbl_offset, int data);		//4ms

// Frequency
#define 	FCY  			7500000		//7.5 MIPs, Fosc=30MHz
#define 	TMR1_PERIOD		46875		//FCY/(prescaler*20Hz),prescaler=8//   50ms
#define 	TMR2_PERIOD		937			//FCY/(prescaler*1kHz),prescaler=8//  	1ms
#define     U1BAUDRATE		9600
#define     U1BRGVAL		(((FCY/U1BAUDRATE)/16)-1)
#define     U2BAUDRATE		9600
#define     U2BRGVAL		(((FCY/U2BAUDRATE)/16)-1)
#define     RS232_Period    300         //ms

// LED
#define	DC1_NFB				LATGbits.LATG15
#define	DC2_NFB				LATCbits.LATC1
#define	BATT_NFB			LATBbits.LATB5
#define	LOAD_NFB			LATBbits.LATB4
#define	M1_NFB				LATBbits.LATB11
#define	M2_NFB				LATBbits.LATB12
#define	M2_LINE				LATDbits.LATD9
#define	M1_LINE				LATDbits.LATD10
#define	MANUAL				LATDbits.LATD11
#define	AUTO				LATDbits.LATD0
#define	OUT_LINE			LATCbits.LATC13
#define	NORMAL				LATDbits.LATD1
#define	FAIL				LATDbits.LATD2
#define	NO_1				LATGbits.LATG1
#define	NO_2				LATGbits.LATG0
// BZ, TP
#define BZ					LATDbits.LATD4
#define BZ_ON()				LATDbits.LATD4=1
#define BZ_OFF()			LATDbits.LATD4=0
#define BZ_TOGGLE()			LATDbits.LATD4^=1		//#define BZ_TOGGLE()			LATDbits.LATD4^=1
#define	DIR_BZ				TRISDbits.TRISD4
#define TP_1_ON()			LATCbits.LATC2=1
#define TP_1_OFF()			LATCbits.LATC2=0
#define TP_1_TOGGLE()		LATCbits.LATC2^=1
#define TP_2_ON()			LATGbits.LATG6=1
#define TP_2_OFF()			LATGbits.LATG6=0
#define TP_2_TOGGLE()		LATGbits.LATG6^=1
#define TP_3_ON()			LATGbits.LATG7=1
#define TP_3_OFF()			LATGbits.LATG7=0
#define TP_3_TOGGLE()		LATGbits.LATG7^=1
#define TP_4_ON()			LATGbits.LATG9=1
#define TP_4_OFF()			LATGbits.LATG9=0
#define TP_4_TOGGLE()		LATGbits.LATG9^=1
#define TP_5_ON()			LATBbits.LATB9=1
#define TP_5_OFF()			LATBbits.LATB9=0
#define TP_5_TOGGLE()		LATBbits.LATB9^=1
#define TP_6_ON()			LATBbits.LATB10=1
#define TP_6_OFF()			LATBbits.LATB10=0
#define TP_6_TOGGLE()		LATBbits.LATB10^=1
#define TP_7_ON()			LATFbits.LATF6=1
#define TP_7_OFF()			LATFbits.LATF6=0
#define TP_7_TOGGLE()		LATFbits.LATF6^=1
#define TP_8_ON()			LATDbits.LATD8=1
#define TP_8_OFF()			LATDbits.LATD8=0
#define TP_8_TOGGLE()		LATDbits.LATD8^=1
#define TP_9_ON()			LATCbits.LATC14=1
#define TP_9_OFF()			LATCbits.LATC14=0
#define TP_9_TOGGLE()		LATCbits.LATC14^=1
#define TP_10_ON()			LATDbits.LATD3=1
#define TP_10_OFF()			LATDbits.LATD3=0
#define TP_10_TOGGLE()		LATDbits.LATD3^=1
#define TP_11_ON()			LATFbits.LATF1=1
#define TP_11_OFF()			LATFbits.LATF1=0
#define TP_11_TOGGLE()		LATFbits.LATF1^=1

//RS485
#define RS485_TX_EN			LATGbits.LATG8

//Backlight
#define BL					LATBbits.LATB8
#define DIR_BL				TRISBbits.TRISB8

//Key
#define	SW_SCAN3		LATDbits.LATD5	// for OUTPUT
#define	SW_SCAN1		LATDbits.LATD6	// for OUTPUT
#define	SW_SCAN2		LATDbits.LATD7	// for OUTPUT

#define	SW_KEY_NUM3		PORTGbits.RG12
#define	SW_KEY_NUM2		PORTGbits.RG13
#define	SW_KEY_NUM1		PORTGbits.RG14

#define	DIR_SW_SCAN3		TRISDbits.TRISD5
#define	DIR_SW_SCAN1		TRISDbits.TRISD6
#define	DIR_SW_SCAN2		TRISDbits.TRISD7
#define	DIR_SW_KEY_NUM3		TRISGbits.TRISG12
#define	DIR_SW_KEY_NUM2		TRISGbits.TRISG13
#define	DIR_SW_KEY_NUM1		TRISGbits.TRISG14

// RTC
#define   RTC_RST     	LATBbits.LATB15	//PORTBbits.RB15
#define   RTC_CLK     	PORTGbits.RG2
#define   RTC_IO        PORTGbits.RG3
#define   RTC_RST_DIR  	TRISBbits.TRISB15
#define   RTC_CLK_DIR  	TRISGbits.TRISG2
#define   RTC_IO_DIR 	TRISGbits.TRISG3

//EEPROM Address
#define INIT_STATE				1
#define BATT_TYPE               2
#define SYS_TYPE                3
#define OP_MODE                 4
#define ALARM_STOP				9
//
#define PASSWD_0				10
#define PASSWD_1			    11
#define PASSWD_2			    12
#define PASSWD_3			    13
#define PASSWD_4			    14
#define PASSWD_5			    15
#define PASSWD_6			    16
#define PASSWD_7			    17
#define PASSWD_8			    18
#define PASSWD_9			    19

#define SPASSWD_0				30
#define SPASSWD_1			    31
#define SPASSWD_2			    32
#define SPASSWD_3			    33
#define SPASSWD_4			    34
#define SPASSWD_5			    35
#define SPASSWD_6			    36
#define SPASSWD_7			    37
#define SPASSWD_8			    38
#define SPASSWD_9			    39

//
#define BATTVOLT_0				20
#define BATTVOLT_1				21
#define BATTVOLT_2				22
#define BATTVOLT_3				23
#define BATTVOLT_4				24
#define BATTVOLT_5				25
#define BATTVOLT_6				26
#define BATTVOLT_7				27
#define BATTVOLT_8				28
#define BATTVOLT_9				29

#define RESTARTTIME_0			40
#define RESTARTTIME_1			41

//Alarm History
#define ALARM_HISTORY_CNT	    50
#define ALARM_STORAGE		    51

#define HIGH    			(1)
#define LOW     			(!HIGH)
		//
#define LEFT				(0)
#define RIGHT 				(1)
		//
#define ON     				(0)   //active low//
#define OFF    				(!ON)
#define	INPUT				(1)
#define	OUTPUT				(0)


////////////// Key Define ////////////////////////////////
#define KEY_ENTER		1
#define KEY_LEFT				2
#define KEY_UP					3
#define KEY_RIGHT				4
#define KEY_DOWN				5
#define KEY_AUTOMANUAL			6
#define KEY_NO_SELECT			7
#define KEY_LAMP				8
#define KEY_BZ_OFF				9

#define F_KEY_ENTER				0
#define F_KEY_LEFT				1
#define F_KEY_UP				2
#define F_KEY_RIGHT				3
#define F_KEY_DOWN				4
#define F_KEY_AUTOMANUAL		5
#define F_KEY_NO_SELECT			6
#define F_KEY_LAMP				7
#define F_KEY_BZ_OFF			8

#define SEQ_NOP         (0)
#define SEQ_SOME        (1)
#define SEQ_ON_TIME     (2)
#define SEQ_CATCH       (3)
#define SEQ_REJECT      (4)
#define SEQ_OFF         (5)
#define SEQ_OFF_TIME    (6)
#define SEQ_END         (7)

#define KEY_ON_TIME     (3) 
#define KEY_REJECT_TIME (36)
#define KEY_OFF_TIME    (5) 

#define     keyNULL						(0)
#define     keyKEY_ENTER				(1)
#define     keyKEY_LEFT					(2)
#define     keyKEY_UP					(3)
#define     keyKEY_RIGHT				(4)
#define     keyKEY_DOWN	 				(5)
#define     keyKEY_AUTOMANUAL			(6)
#define     keyKEY_NO_SELECT			(7)
#define     keyKEY_LAMP					(8)
#define     keyKEY_BZ_OFF				(9)
#define     keyKEY_ENTERfast			(10)
#define     keyKEY_LEFTfast				(11)
#define     keyKEY_UPfast				(12)
#define     keyKEY_RIGHTfast			(13)
#define     keyKEY_DOWNfast				(14)
#define     keyKEY_AUTOMANUALfast		(15)
#define     keyKEY_NO_SELECTfast		(16)
#define     keyKEY_LAMPfast				(17)
#define     keyKEY_BZ_OFFfast			(18)
//#define		keyKEY_L_R			(20)

#define DELAY_LONG_PRESSED (300)//(750) //org 500


//Type Define
/* Compiler-independent, fixed size types */
typedef signed char     	INT8 ;      /* Signed 8-bit integer    */
typedef unsigned char      	UINT8 ;     /* Unsigned 8-bit integer  */
typedef signed short int   	INT16 ;     /* Signed 16-bit integer   */
typedef unsigned short int 	UINT16 ;    /* Unsigned 16-bit integer */
typedef long int           	INT32 ;     /* Signed 32-bit integer   */
typedef unsigned long      	UINT32 ;
typedef float              	FLOAT32 ;   /* 32-bit IEEE single precisiON */
typedef double             	FLOAT64 ;   /* 64-bit IEEE double precisiON */

typedef unsigned char  		BYTE ;
typedef unsigned int   		WORD;
typedef enum {false,true} 	BOOL;

typedef union
{
    UINT8 _byte;
    struct
    {
        unsigned lsb :4,
                 msb :4;
    };
}BYTE_T;

/***************************************************************************
 *  specific definitiONs
 ***************************************************************************/
typedef struct 
{
	unsigned b0 :1,b1 :1,b2 :1,b3 :1,b4 :1,b5 :1,b6 :1,b7 :1;	
} bool8;

typedef struct 
{
	unsigned b0 :1,b1 :1,b2 :1,b3 :1,b4 :1,b5 :1,b6 :1,b7 :1;
	unsigned b8 :1,b9 :1,b10:1,b11:1,b12:1,b13:1,b14:1,b15:1;
} bool16;



typedef struct  Tds1302_Time
{      
 		UINT8 Second;
 		UINT8 Minute;
 		UINT8 Hour;
 		UINT8 Date;
 		UINT8 Month;
 		UINT8 Year;
 		UINT8 Am;
 		UINT8 Md;
} ds1302_Time;
//struct Tds1302_Time  Ds1302_Time;

#define _NOP()		__asm__ volatile ("NOP")


#endif
//=======================================================


