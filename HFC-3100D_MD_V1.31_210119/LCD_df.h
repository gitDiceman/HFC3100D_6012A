/*
**********************************************************
* PROJECT : HFC3100D MAIN DISPLAY
* FILE NAME: LCD_df.h
* DEVICE : dsPIC30F6012A
* COMPILER : MPLAB-C
* REVISION HISTORY
* 1) 2018.02.28 (V1.0)
* 
********************************************************** */

extern int read_EE1(int tbl_offset);
extern void write_EE1(int tbl_offset, int data);		//4ms
//LCD Module commands --- These settings can be found in the LCD datasheet
#define DISP_2Line_8Bit		0x0038		// 2 lines & 8 bits setting
#define DISP_2Line_4Bit		0x0028		// 2 lines & 4 bits setting
#define DISP_ON				0x00C		// Display on
#define CLR_DISP			0x001		// Clear the Display
#define ENTRY_INC			0x006		// Entry Increment and Cursor Move

//Display Define
#define LCD_RS			LATFbits.LATF0		// The definition of control pins
#define LCD_RW			LATBbits.LATB14
#define LCD_E			LATBbits.LATB13

#define	DIR_LCD_RS		TRISBbits.TRISB12	// Direction of control pins
#define	DIR_LCD_RW		TRISBbits.TRISB14
#define	DIR_LCD_E		TRISBbits.TRISB13

#define LCD_DATA		LATB				// PORTB[0:3] as LCD DB[4:7]
#define DIR_LCD_DATA	TRISB				// Direction of Databus

//Display
#define M0_LOGO					1
#define M0_OUTBATT				2
#define M0_LOAD					3
#define M0_SETUP				4
#define M0_HISTORY				5
#define M0_TIME					6
//
#define M1_SETUPSET				10
#define M1_BATTALARM			11
#define M1_MODULERESTART		12
#define M1_TIMESET				13
#define M1_SYSTEMINIT			14
#define M1_PASSWORD				15
#define M1_ALARMSTOP			16
//
#define M2_BATTALARMSET			20
#define M2_MODULERESTARTSET		21
#define M2_SYSTEMINITSET		22
#define M2_PASSWORDSET			23
#define M2_ALARMSTOPSET			24
//
#define M3_System				30
#define M3_BATTTYPE             31
#define M3_INPUTVOLT            32
#define M3_OPERATIONMODE		33
#define M3_SYSTYPE              34
//
#define M4_SystemSet			40
#define M4_BATTTYPESET          41
#define M4_INPUTVOLTSET         42
#define M4_OPERATIONMODESET		43
#define M4_SYSTYPESET           44







