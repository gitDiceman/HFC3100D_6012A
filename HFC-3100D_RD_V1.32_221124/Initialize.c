#include 	<p30f6012A.h>
#include	<stdio.h>
#include 	"sysinfo.h"
#include 	"eeprom.h"
#include 	"Definition.h"
#include    "Initialize.h"
#include    "Alram_Sts.h"
#include    "RS232.h"

int _EEDATA(2) EE_mem[2048] = {};					//100% EEPROM memory
			//
void Set_EEPROM()
{
	TBLPAG = __builtin_tblpage(EE_mem);
	NVMADRU = __builtin_tblpage(EE_mem);
}

void ParameterInitialize(void)
{
	gPin[0] = read_EE(PASSWD_0);
	gPin[1] = read_EE(PASSWD_1);
	gPin[2] = read_EE(PASSWD_2);
	gPin[3] = read_EE(PASSWD_3);
	gPin[4] = read_EE(PASSWD_4);
	gPin[5] = read_EE(PASSWD_5);
	gPin[6] = read_EE(PASSWD_6);
	gPin[7] = read_EE(PASSWD_7);
	gPin[8] = read_EE(PASSWD_8);
	gPin[9] = read_EE(PASSWD_9);
    
    gPin2[0] = '1';
	gPin2[1] = '1';
	gPin2[2] = '1';
	gPin2[3] = '1';
	gPin2[4] = '1';
	gPin2[5] = '1';
	gPin2[6] = '1';
	gPin2[7] = '1';
	gPin2[8] = '1';
	gPin2[9] = '1';
		
	fAlarmStop = read_EE(ALARM_STOP);
    
    if(read_EE(BATT_TYPE) == 0) 
    {
        CHG_Sts.b4 = 0;
        CHG_Sts.b3 = 0; 
    }
    else if(read_EE(BATT_TYPE) == 1)
    {
        CHG_Sts.b4 = 0;
        CHG_Sts.b3 = 1; 
    }
    else if(read_EE(BATT_TYPE) == 2)
    {
        CHG_Sts.b4 = 1;
        CHG_Sts.b3 = 0; 
    }    
    
	goutValarm[0] = read_EE(OUTVALARM_0);
	goutValarm[1] = read_EE(OUTVALARM_1);
	goutValarm[2] = read_EE(OUTVALARM_2);
	goutValarm[3] = read_EE(OUTVALARM_3);
	goutValarm[4] = read_EE(OUTVALARM_4);
	goutValarm[5] = read_EE(OUTVALARM_5);
	goutValarm[6] = read_EE(OUTVALARM_6);
	goutValarm[7] = read_EE(OUTVALARM_7);
	goutValarm[8] = read_EE(OUTVALARM_8);
	goutValarm[9] = read_EE(OUTVALARM_9);

	goutAalarm[0] = read_EE(OUTAALARM_0);
	goutAalarm[1] = read_EE(OUTAALARM_1);
	goutAalarm[2] = read_EE(OUTAALARM_2);
	goutAalarm[3] = read_EE(OUTAALARM_3);
	goutAalarm[4] = read_EE(OUTAALARM_4);
	goutAalarm[5] = read_EE(OUTAALARM_5);
	goutAalarm[6] = read_EE(OUTAALARM_6);
	goutAalarm[7] = read_EE(OUTAALARM_7);
	goutAalarm[8] = read_EE(OUTAALARM_8);
	goutAalarm[9] = read_EE(OUTAALARM_9);
	
	battVoltSet[0] = read_EE(BATTVOLT_0);
	battVoltSet[1] = read_EE(BATTVOLT_1);
	battVoltSet[2] = read_EE(BATTVOLT_2);
	battVoltSet[3] = read_EE(BATTVOLT_3);
	battVoltSet[4] = read_EE(BATTVOLT_4);
	battVoltSet[5] = read_EE(BATTVOLT_5);
	battVoltSet[6] = read_EE(BATTVOLT_6);
	battVoltSet[7] = read_EE(BATTVOLT_7);

    LoadAmpSet[0] = read_EE(LOADAMP_0);
	LoadAmpSet[1] = read_EE(LOADAMP_1);
	LoadAmpSet[2] = read_EE(LOADAMP_2);
    LoadAmpSet[3] = read_EE(LOADAMP_3);

	fbattVoltSet=1;
	battAmpSet[0] = read_EE(BATTAMP_0);
	battAmpSet[1] = read_EE(BATTAMP_1);
	battAmpSet[2] = read_EE(BATTAMP_2);
    battAmpSet[3] = read_EE(BATTAMP_3);
    
	//input alarm
	ginValarm[0] = read_EE(INVALARM_0);
	ginValarm[1] = read_EE(INVALARM_1);
	ginValarm[2] = read_EE(INVALARM_2);
	ginValarm[3] = read_EE(INVALARM_3);
	ginValarm[4] = read_EE(INVALARM_4);
	ginValarm[5] = read_EE(INVALARM_5);
	ginValarm[6] = read_EE(INVALARM_6);
	ginValarm[7] = read_EE(INVALARM_7);
	ginValarm[8] = read_EE(INVALARM_8);
	ginValarm[9] = read_EE(INVALARM_9);
	//shutDownOutV
	shutDownOutV[0] = read_EE(SHUTDOWNOUTV_0);
	shutDownOutV[1] = read_EE(SHUTDOWNOUTV_1);
	shutDownOutV[2] = read_EE(SHUTDOWNOUTV_2);
	shutDownOutV[3] = read_EE(SHUTDOWNOUTV_3);
	shutDownOutV[4] = read_EE(SHUTDOWNOUTV_4);
	shutDownOutV[5] = read_EE(SHUTDOWNOUTV_5);
	shutDownOutV[6] = read_EE(SHUTDOWNOUTV_6);
	shutDownOutV[7] = read_EE(SHUTDOWNOUTV_7);
	shutDownOutV[8] = read_EE(SHUTDOWNOUTV_8);
	shutDownOutV[9] = read_EE(SHUTDOWNOUTV_9);
	//shutDownOutA
	shutDownOutA[0] = read_EE(SHUTDOWNOUTA_0);
	shutDownOutA[1] = read_EE(SHUTDOWNOUTA_1);
	shutDownOutA[2] = read_EE(SHUTDOWNOUTA_2);
	shutDownOutA[3] = read_EE(SHUTDOWNOUTA_3);
	shutDownOutA[4] = read_EE(SHUTDOWNOUTA_4);
	shutDownOutA[5] = read_EE(SHUTDOWNOUTA_5);
	shutDownOutA[6] = read_EE(SHUTDOWNOUTA_6);
	shutDownOutA[7] = read_EE(SHUTDOWNOUTA_7);
	shutDownOutA[8] = read_EE(SHUTDOWNOUTA_8);
	shutDownOutA[9] = read_EE(SHUTDOWNOUTA_9);
	//ADC Gain
	ADJ_VGain = read_EE(ADJVGain_0);
	ADJ_CGain = read_EE(ADJCGain_0);
    
    Set_time[0] = read_EE(CHG_SET_NO) + 0x30;
    CHG_Cut_TIME = read_EE(CHG_SET_TIME);
    CHG_Cut_CURR = read_EE(CHG_SET_CURR);
    Set_COND[0] = (read_EE(CHG_SET_TIME)/10) + 0x30;
    Set_COND[1] = (read_EE(CHG_SET_TIME)%10) + 0x30;
    Set_COND[2] = (read_EE(CHG_SET_CURR)/10) + 0x30;
    Set_COND[3] = (read_EE(CHG_SET_CURR)%10) + 0x30; 
    
    BL=1;
	lcd_reflash();
	iMenuIndex=0;
		
	year = rm_bcd(read_ds1302(0x8d));
	if(!year)
	{	
		rtc_set_datetime(21,11,16,0,17,37);//(day,mth,year,dow,hour,min)
	}	
    if(read_EE(INPUT_VOLT) == 0)
    {
        Input_UV_Ref = MinInputVoltage_380;	//18.06.18 -10% - KHJ
        Input_OV_Ref = MaxInputVoltage_380;  	//18.06.18 +10% - KHJ
        Input_V_Ref = RefInputVoltage_380;
        Input_Shutdown_Ref = ShutdownVoltage_380;   //18.06.18 -40% - Order by GJJ 
    }
    else
    {
        Input_UV_Ref = MinInputVoltage_220;	//18.06.18 -10% - KHJ
        Input_OV_Ref = MaxInputVoltage_220;  	//18.06.18 +10% - KHJ
        Input_V_Ref = RefInputVoltage_220;
        Input_Shutdown_Ref = ShutdownVoltage_220;   //18.06.18 -40% - Order by GJJ 
    }

    //Init Value
	Set_ARef = (((goutAalarm[0]-0x30)*100)+((goutAalarm[1]-0x30)*10)+((goutAalarm[2]-0x30)));
	Set_CL = ((LoadAmpSet[0]-0x30)*100)+((LoadAmpSet[1]-0x30)*10)+(LoadAmpSet[2]-0x30);
	Set_VRef = ((battVoltSet[4]-0x30)*100)+((battVoltSet[5]-0x30)*10)+((battVoltSet[6]-0x30)*1)+((battVoltSet[7]-0x30)*0.1);
    Set_OV = ((shutDownOutV[0]-0x30)*100)+((shutDownOutV[1]-0x30)*10)+((shutDownOutV[2]-0x30)*1)+((shutDownOutV[3]-0x30)*0.1);
	Set_UV = ((shutDownOutV[4]-0x30)*100)+((shutDownOutV[5]-0x30)*10)+((shutDownOutV[6]-0x30)*1)+((shutDownOutV[7]-0x30)*0.1);
	Set_Input_OV = ((ginValarm[0]-0x30)*100)+((ginValarm[1]-0x30)*10)+((ginValarm[2]-0x30)*1)+((ginValarm[3]-0x30)*0.1);
	Set_Input_UV = ((ginValarm[4]-0x30)*100)+((ginValarm[5]-0x30)*10)+((ginValarm[6]-0x30)*1)+((ginValarm[7]-0x30)*0.1);
	Set_Output_OV = ((goutValarm[0]-0x30)*100)+((goutValarm[1]-0x30)*10)+((goutValarm[2]-0x30)*1)+((goutValarm[3]-0x30)*0.1);
	Set_Output_UV = ((goutValarm[4]-0x30)*100)+((goutValarm[5]-0x30)*10)+((goutValarm[6]-0x30)*1)+((goutValarm[7]-0x30)*0.1);
	Set_OT = OverTempLimitmax;
	BL = 0;
    //
    //goooo add below for charge battery current limit //
    Set_BattAmp=((battAmpSet[0] & 0x0F)*100) +
                ((battAmpSet[1] & 0x0F)*10)+ 
                ((battAmpSet[2] & 0x0F)*1)+
                ((battAmpSet[3] & 0x0F)*0.1);
}

void sysInit()
{
    write_EE(ALARM_STOP,		'Y');    
	write_EE(ALARM_TEMP_0,		'0');
	write_EE(ALARM_TEMP_1,		'9');
	write_EE(ALARM_TEMP_2,		'5');
	write_EE(ALARM_TEMP_3,		'0');    
	//OutValarm        
    if(read_EE(BATT_TYPE) == 0)//pb125
    {
        write_EE(SHUTDOWNOUTV_0,    '1');//Transfer Max,Min Volt
        write_EE(SHUTDOWNOUTV_1,    '5');
        write_EE(SHUTDOWNOUTV_2,    '0');
        write_EE(SHUTDOWNOUTV_3,    '0');
        write_EE(SHUTDOWNOUTV_4,    '0');//Transfer volt Min    
        write_EE(SHUTDOWNOUTV_5,    '9');
        write_EE(SHUTDOWNOUTV_6,    '8');
        write_EE(SHUTDOWNOUTV_7,    '6');

        write_EE(BATTVOLT_0,        '1');//float/Eq. Output voltage//
        write_EE(BATTVOLT_1,        '3');
        write_EE(BATTVOLT_2,        '1');
        write_EE(BATTVOLT_3,        '0');
        write_EE(BATTVOLT_4,        '1');
        write_EE(BATTVOLT_5,        '4');
        write_EE(BATTVOLT_6,        '1');
        write_EE(BATTVOLT_7,        '0');

        write_EE(OUTVALARM_0,		'1');//alarm out volt//
        write_EE(OUTVALARM_1,		'5');
        write_EE(OUTVALARM_2,		'0');
        write_EE(OUTVALARM_3,		'0');
        write_EE(OUTVALARM_4,		'1');
        write_EE(OUTVALARM_5,		'1');
        write_EE(OUTVALARM_6,		'0');
        write_EE(OUTVALARM_7,		'0');
    }
    else if(read_EE(BATT_TYPE) == 1)//pb110 goooo
    {
        write_EE(SHUTDOWNOUTV_0,    '1');
        write_EE(SHUTDOWNOUTV_1,    '5');
        write_EE(SHUTDOWNOUTV_2,    '0');
        write_EE(SHUTDOWNOUTV_3,    '0');
        write_EE(SHUTDOWNOUTV_4,    '0');    
        write_EE(SHUTDOWNOUTV_5,    '9');
        write_EE(SHUTDOWNOUTV_6,    '0');
        write_EE(SHUTDOWNOUTV_7,    '0');

        write_EE(BATTVOLT_0,        '1');
        write_EE(BATTVOLT_1,        '1');
        write_EE(BATTVOLT_2,        '5');
        write_EE(BATTVOLT_3,        '0');
        write_EE(BATTVOLT_4,        '1');
        write_EE(BATTVOLT_5,        '2');
        write_EE(BATTVOLT_6,        '4');
        write_EE(BATTVOLT_7,        '0');

        write_EE(OUTVALARM_0,		'1');
        write_EE(OUTVALARM_1,		'3');
        write_EE(OUTVALARM_2,		'5');
        write_EE(OUTVALARM_3,		'0');
        write_EE(OUTVALARM_4,		'0');
        write_EE(OUTVALARM_5,		'9');
        write_EE(OUTVALARM_6,		'5');
        write_EE(OUTVALARM_7,		'0');
    }
    else if(read_EE(BATT_TYPE) == 2)//litum-Ion battery
    {
        write_EE(SHUTDOWNOUTV_0,    '1');
        write_EE(SHUTDOWNOUTV_1,    '5');
        write_EE(SHUTDOWNOUTV_2,    '0');
        write_EE(SHUTDOWNOUTV_3,    '0');
        write_EE(SHUTDOWNOUTV_4,    '0');    
        write_EE(SHUTDOWNOUTV_5,    '9');
        write_EE(SHUTDOWNOUTV_6,    '0');
        write_EE(SHUTDOWNOUTV_7,    '0');                                   

        write_EE(BATTVOLT_0,        '1');//output voltage//
        write_EE(BATTVOLT_1,        '2');
        write_EE(BATTVOLT_2,        '5');
        write_EE(BATTVOLT_3,        '0');
        write_EE(BATTVOLT_4,        '1');
        write_EE(BATTVOLT_5,        '2');
        write_EE(BATTVOLT_6,        '5');
        write_EE(BATTVOLT_7,        '0');

        write_EE(OUTVALARM_0,		'1');
        write_EE(OUTVALARM_1,		'3');
        write_EE(OUTVALARM_2,		'5');
        write_EE(OUTVALARM_3,		'0');
        write_EE(OUTVALARM_4,		'0');
        write_EE(OUTVALARM_5,		'9');
        write_EE(OUTVALARM_6,		'5');
        write_EE(OUTVALARM_7,		'0');
    }
       
	//Output Current Alarm
	write_EE(OUTAALARM_0,		'1');
	write_EE(OUTAALARM_1,		'1');
	write_EE(OUTAALARM_2,		'0');
	write_EE(OUTAALARM_3,		'0');
	write_EE(OUTAALARM_4,		'0');
	write_EE(OUTAALARM_5,		'8');
	write_EE(OUTAALARM_6,		'0');
	write_EE(OUTAALARM_7,		'0');
	write_EE(OUTAALARM_8,		'0');
	write_EE(OUTAALARM_9,		'0');

    //LoadAmpSet add 
    write_EE(LOADAMP_0,   	 	'1'); 
	write_EE(LOADAMP_1,			'2');
	write_EE(LOADAMP_2,			'0');
	write_EE(LOADAMP_3,			'0');
      
	//BattAmpSet add JSS 2018.06.11
	write_EE(BATTAMP_0,   	 	'0');
	write_EE(BATTAMP_1,			'3');
	write_EE(BATTAMP_2,			'0');
	write_EE(BATTAMP_3,			'0');
    
	//Input Voltage
    if(read_EE(INPUT_VOLT) == 0)
    {
        write_EE(INVALARM_0,    	'4');
        write_EE(INVALARM_1,		'1');
        write_EE(INVALARM_2,		'8');
        write_EE(INVALARM_3,		'0');
        write_EE(INVALARM_4,		'3');
        write_EE(INVALARM_5,		'4');
        write_EE(INVALARM_6,		'2');
        write_EE(INVALARM_7,		'0');
        write_EE(INVALARM_8,		'0');
        write_EE(INVALARM_9,		'0');
    }
    else
    {
        write_EE(INVALARM_0,    	'2');
        write_EE(INVALARM_1,		'4');
        write_EE(INVALARM_2,		'2');
        write_EE(INVALARM_3,		'0');
        write_EE(INVALARM_4,		'1');
        write_EE(INVALARM_5,		'9');
        write_EE(INVALARM_6,		'8');
        write_EE(INVALARM_7,		'0');
        write_EE(INVALARM_8,		'0');
        write_EE(INVALARM_9,		'0');
    }
	
	write_EE(SHUTDOWNOUTA_0,   	'1');
	write_EE(SHUTDOWNOUTA_1,	'2');
	write_EE(SHUTDOWNOUTA_2,	'0');
	write_EE(SHUTDOWNOUTA_3,	'0');
	write_EE(SHUTDOWNOUTA_4,	'0');
	write_EE(SHUTDOWNOUTA_5,	'0');
	write_EE(SHUTDOWNOUTA_6,	'0');
	write_EE(SHUTDOWNOUTA_7,	'0');
	write_EE(SHUTDOWNOUTA_8,	'0');
	write_EE(SHUTDOWNOUTA_9,	'0');
        
    write_EE(ADJVGain_0,   	 	0x00);
	write_EE(ADJCGain_0,		0x00);
    
    write_EE(CHG_STS_0,   	 	0x00);
	write_EE(CHG_STS_1,         0x00);
    
    write_EE(CHG_SET_NO,        0x00);
    write_EE(CHG_SET_TIME,      0x01);
    write_EE(CHG_SET_CURR,      0x05);
    
    write_EE(SET_YEAR_0,        0x01);
    write_EE(SET_MONTH_0,       0x01);
    write_EE(SET_DAY_0,         0x01);
    write_EE(SET_HOUR_0,        0x00);
    write_EE(SET_MIN_0,         0x00);
    write_EE(SET_SEC_0,         0x00);
    
    write_EE(SET_YEAR_1,        0x01);
    write_EE(SET_MONTH_1,       0x01);
    write_EE(SET_DAY_1,         0x01);
    write_EE(SET_HOUR_1,        0x00);
    write_EE(SET_MIN_1,         0x00);
    write_EE(SET_SEC_1,         0x00);
    
    write_EE(SET_YEAR_2,        0x01);
    write_EE(SET_MONTH_2,       0x01);
    write_EE(SET_DAY_2,         0x01);
    write_EE(SET_HOUR_2,        0x00);
    write_EE(SET_MIN_2,         0x00);
    write_EE(SET_SEC_2,         0x00);
    
    write_EE(SET_YEAR_3,        0x01);
    write_EE(SET_MONTH_3,       0x01);
    write_EE(SET_DAY_3,         0x01);
    write_EE(SET_HOUR_3,        0x00);
    write_EE(SET_MIN_3,         0x00);
    write_EE(SET_SEC_3,         0x00);
    
    write_EE(SET_YEAR_4,        0x01);
    write_EE(SET_MONTH_4,       0x01);
    write_EE(SET_DAY_4,         0x01);
    write_EE(SET_HOUR_4,        0x00);
    write_EE(SET_MIN_4,         0x00);
    write_EE(SET_SEC_4,         0x00);
    
    write_EE(SET_YEAR_5,        0x01);
    write_EE(SET_MONTH_5,       0x01);
    write_EE(SET_DAY_5,         0x01);
    write_EE(SET_HOUR_5,        0x00);
    write_EE(SET_MIN_5,         0x00);
    write_EE(SET_SEC_5,         0x00);
    
    write_EE(SET_YEAR_6,        0x01);
    write_EE(SET_MONTH_6,       0x01);
    write_EE(SET_DAY_6,         0x01);
    write_EE(SET_HOUR_6,        0x00);
    write_EE(SET_MIN_6,         0x00);
    write_EE(SET_SEC_6,         0x00);
    
    write_EE(SET_YEAR_7,        0x01);
    write_EE(SET_MONTH_7,       0x01);
    write_EE(SET_DAY_7,         0x01);
    write_EE(SET_HOUR_7,        0x00);
    write_EE(SET_MIN_7,         0x00);
    write_EE(SET_SEC_7,         0x00);
    
    write_EE(SET_YEAR_8,        0x01);
    write_EE(SET_MONTH_8,       0x01);
    write_EE(SET_DAY_8,         0x01);
    write_EE(SET_HOUR_8,        0x00);
    write_EE(SET_MIN_8,         0x00);
    write_EE(SET_SEC_8,         0x00);
    
    write_EE(SET_YEAR_9,        0x01);
    write_EE(SET_MONTH_9,       0x01);
    write_EE(SET_DAY_9,         0x01);
    write_EE(SET_HOUR_9,        0x00);
    write_EE(SET_MIN_9,         0x00);
    write_EE(SET_SEC_9,         0x00);
    
}

void EEPROM_Initialize(void)
{
	if(read_EE(INIT_STATE) == 0)    // Only one after download
	{
		write_EE(INIT_STATE, 		1);
		write_EE(PASSWD_0,			'0');
		write_EE(PASSWD_1,			'0');
		write_EE(PASSWD_2,		    '0');
		write_EE(PASSWD_3,		    '0');
		write_EE(PASSWD_4,		    '0');
		write_EE(PASSWD_5,		    '0');
		write_EE(PASSWD_6,		    '0');
		write_EE(PASSWD_7,		    '0');
		write_EE(PASSWD_8,		    '0');
		write_EE(PASSWD_9,		    '0');
        //----------
        sysInit();	
        //----------
	}
    else
    {   
        write_EE(OP_MODE,   	 	'N');
        write_EE(CHG_STS_0,   	 	0x00);
        write_EE(CHG_STS_1,         0x00);
    }
}

void Init_Port()
{
	DIR_UP		= INPUT;
	DIR_DOWN	= INPUT;
	DIR_ESC		= INPUT;
	DIR_ENT		= INPUT;
	DIR_BZ		= OUTPUT;
	RTC_RST_DIR = 0;
	RTC_CLK_DIR = 0;
	
	TRISB = 0X00FF;	TRISC = 0X0000;	TRISD = 0X0000;	TRISF = 0X0015;	TRISG = 0X7009;
	PORTB = 0X7E00;	PORTC = 0X0002;	PORTD = 0X043F; PORTF = 0X0000; PORTG = 0XF001;
}

void Init_Timer1()			//50ms
{
	TMR1 = 0;			//count
	PR1 = TMR1_PERIOD - 1;		//period
	T1CONbits.TCS = 0;		//cONtrol, internal clock(Fosc/4)
	T1CONbits.TCKPS = 1;		//1:8 prescale
    IFS0bits.T1IF = 0;		//Flag Clear
	IEC0bits.T1IE = 1;		//Interrupt 0=>disable, 1=>enable
	T1CONbits.TON = 1;		//cONtrol, starts timer
}

void Init_Timer2()			//1ms
{
	TMR2 = 0;			//count
	PR2 = TMR2_PERIOD - 1;		//period
	T2CONbits.TCS = 0;		//cONtrol, internal clock(Fosc/4)
	T2CONbits.TCKPS = 1;		//1:8 prescale
	IFS0bits.T2IF = 0;		//Flag Clear
	IEC0bits.T2IE = 1;		//Interrupt 0=>disable, 1=>enable
	T2CONbits.TON = 1;		//cONtrol, starts timer
}

