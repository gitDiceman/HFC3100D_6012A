/*
	HFC3100D MAIN DISPLAY BOARD - CHARACTER  LCD 16x4
	F/W : HFC-3100D_MD_V1.31_210119
	DEVICE : DSPIC30F6012A
	TOOL : MPLABX IDE V5.X, 6.X
	COMPILER : MPLABC30 V3.31


*/

#include 	<p30f6012A.h>
#include	<stdio.h>
#include 	"Definition.h"
#include 	"eeprom.h"
#include	"Function.h"
#include	"LCD_df.h"
#include	"LCD_fc.h"
#include	"RS232.h"
#include	"RS485.h"

_FOSC(CSW_FSCM_OFF & HS2_PLL4)				//30MIPs at 15MHz crystal
//_FWDT(WDT_ON & WDTPSA_8 & WDTPSB_16)		//watchdog Enable//2ms * 8 * 16	 = 256ms	
_FWDT(WDT_OFF)								//watchdog disable
_FBORPOR(PBOR_ON & BORV_45 & PWRT_64)
//====================================================================================//
//                          Function Declaration                                      //
//====================================================================================//
void Init_Port(void);
void Init_Timer1(void);
void Init_Timer2(void);

//------------//
//RTC function//
//------------//
void write_ds1302_byte(UINT8 cmd);
void write_ds1302(UINT8 cmd, UINT8 data);
void rtc_init(void);
void rtc_set_datetime(UINT8 day, UINT8 mth, UINT8 year, UINT8 dow, UINT8 hour, UINT8 min);
void rtc_get_date();
void rtc_get_time();
void rtc_write_nvr(UINT8 address, UINT8 data);
//---------------//
//EEPROM function//
//---------------//
void EEPROM_Initialize(void);
void Set_EEPROM();
int _EEDATA(2) EE_mem[2048] = {};					//100% EEPROM memory
			//
void Set_EEPROM()
{
	TBLPAG = __builtin_tblpage(EE_mem); 
	NVMADRU = __builtin_tblpage(EE_mem);
}
void ParameterInitialize(void);
void SystemInitialize(void);
void Init_Interrupt(void);
//----------------//
//Utility function//
//----------------//
void DelayNop(unsigned int delaycnt);
void Delay_Cnt(UINT16 cnt);

//====================================================================================//
//                          Variables Declaration                                     //
//====================================================================================//
//Global
UINT8 data_8,gCnt_8,buff_8;
UINT16 data_16,gCnt_16,buff_16;
UINT8 gCnt;
//Timer
UINT16 tmr1Count0=0,tmr1Count1=0,tmr1Count2=0,tmr1Count3=0,tmr1Count4=0;
UINT16 tmr2Count0=0,tmr2Count1=0,tmr2Count2=0,tmr2Count3=0,tmr2Count4=0;
UINT16 tmr50ms=0,tmr100ms=0,tmr200ms=0,tmr500ms=0,tmr1s=0;
UINT16 cursor100ms=0,cursor1s=0;
UINT8 errBz100ms=0;
UINT16 key1s=0;
//Key
UINT8 uc_key=0,fKey=0,SetKeyOk=0,fKeyAutoManual=0,fKeyNoSelect=0;
//Memory
UINT8 gPass[10]={'0','0','0','0','0','0','0','0','0','0'};
UINT8 SgPass[10]={'1','1','1','1','1','1','1','1','1','1'};
UINT8 gBattVoltSet[10]={'1','5','0','0','1','0','2','0','0','0'};
UINT8 gRestartTime[2]={'1',};
//Flag
//Count
UINT8 Key_Cnt=0;

//====================================================================================//
//                          Status Variables Declaration                              //
//====================================================================================//
void Delay_Cnt(UINT16 cnt)
{
	UINT16 i;
		for(i=0;i<cnt;i++)
		{
			__asm__ volatile ("NOP");
		}
		//__asm__ volatile ("CLRWDT");		
}

void Init_Port()
{
	DIR_SW_SCAN3		= OUTPUT;
	DIR_SW_SCAN1		= OUTPUT;
	DIR_SW_SCAN2		= OUTPUT;
	DIR_SW_KEY_NUM3		= INPUT;
	DIR_SW_KEY_NUM2		= INPUT;
	DIR_SW_KEY_NUM1		= INPUT;
	DIR_BZ				= OUTPUT;
	RTC_RST_DIR = 0;
	RTC_CLK_DIR = 0;
	
	TRISB = 0X00C0;	TRISC = 0X0000;	TRISD = 0X0000;	TRISF = 0X0014;	TRISG = 0X7008;
	PORTB = 0X1830;	PORTC = 0X2002;	PORTD = 0X0E07; PORTF = 0X0000; PORTG = 0X8003;
	
	//TRISGbits.TRISG6		= 0;//TestPoint1(TP1)
	//TRISFbits.TRISF6		= 0;//TestPoint2(TP2) 
	
	RS485_TX_EN=0;
}

void Init_Timer1()			//�̻��..50ms
{
	TMR1 = 0;			//count
	PR1 = TMR1_PERIOD - 1;		//period
	T1CONbits.TCS = 0;		//cONtrol, internal clock(Fosc/4)
	T1CONbits.TCKPS = 1;		//1:8 prescale
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

int main(void)
{	
	unsigned int i;
	while(OSCCONbits.LOCK!=1) {};
	//-----------------//
	//Initialize System//
	//-----------------//
//__asm__ volatile ("CLRWDT");
	Init_Port();
	rtc_init();
	Set_EEPROM();
	Init_Uart1();
	Init_Uart2();
	InitLCD();
	Init_Timer1();			//timer1 init(50msec)	
	Init_Timer2();			//timer2 init (1msec)
	Init_Interrupt();
	EEPROM_Initialize();
	ParameterInitialize();
	DispInit();
	//Set_OK=1;
	//
	for(i = 0; i < 150; i++)
	{
		gAlarmHistory[i][0] = read_EE(ALARM_STORAGE + (i*10)    );
		gAlarmHistory[i][1] = read_EE(ALARM_STORAGE + (i*10) + 1);
		gAlarmHistory[i][2] = read_EE(ALARM_STORAGE + (i*10) + 2);
		gAlarmHistory[i][3] = read_EE(ALARM_STORAGE + (i*10) + 3);
		gAlarmHistory[i][4] = read_EE(ALARM_STORAGE + (i*10) + 4);
		gAlarmHistory[i][5] = read_EE(ALARM_STORAGE + (i*10) + 5);
		gAlarmHistory[i][6] = read_EE(ALARM_STORAGE + (i*10) + 6);
		gAlarmHistory[i][7] = read_EE(ALARM_STORAGE + (i*10) + 7);
		gAlarmHistory[i][8] = read_EE(ALARM_STORAGE + (i*10) + 8);
		gAlarmHistory[i][9] = read_EE(ALARM_STORAGE + (i*10) + 9);
		gAlarmHistoryCnt = read_EE(ALARM_HISTORY_CNT);
	}
	year = rm_bcd(read_ds1302(0x8d));
	InitSetValue();
	gSelectModule = 1;
	Set_OK=1;
	if(!year)
	{	
		 	rtc_set_datetime(21,11,16,0,17,37);//(day,mth,year,dow,hour,min)
	}
		
	while(1)
	{
        Serial_Process();
		uc_key = getKeyData();
		if(tmr2Count0 > 5) //5ms
		{
			tmr2Count0 = 0;
			KeyDetecting();
			AutoManual();
			SelectModule();
		}
		//
		if(tmr2Count1 >= 100)//300ms
		{
			tmr2Count1 = 0;
			MainProcess();		//display menu select
			LedStatus();
			ModuleStatus();
			BatteryStatus();
			WarningStatus();
			ModuleStartupStatus();
            ChargeStatus();
			errBz_On();
			/*
			if(Set_OK == 1)
            {
				Set_OK = 0;
                if(Key_Cnt == 1)
                {
                    Set_OK = 0;
                }
                else
                {
                    Set_OK = 1;
                    Key_Cnt = 1;
                }
            }
            else
            {
                Set_OK = 0;
                Key_Cnt = 0;
            }
			*/
		}		
		//
		if(tmr2Count2 >= 100)//100ms
		{
			tmr2Count2=0;
			//if(ErrCode != 0)// || ErrCode != 20)
			//if(ErrCode != 0)
			//{
			//	AlarmOccurrenceApply(gPacketsize);
			//	gPacketsize = 0;
			//}
		}
		
		if(uc_key > 0 )
		{
			BL=1;
			tmr500ms=0;
		}
		if(tmr500ms>=20 && BL==1)
		{
			tmr500ms=0;
			BL=0;
		}
		uc_key = 0;
	}//end while
	return 0;
}	//end main
//==============================================================

void __attribute__((interrupt,auto_psv)) _T1Interrupt()
{	
	tmr1Count0++;		//50ms LampTet
	tmr1Count1++;		//50ms BackLight
	tmr1Count2++;		//50ms BZ Delay
	tmr1Count3++;		//50ms ERRBZ
	tmr1Count4++;		//50ms ERRBZ
	//
	if(tmr1Count0>1)	//5ms*1 every increment 50ms//
	{
		tmr1Count0=0;
		tmr50ms++;
	}
	//
	if(tmr1Count1>2)	//5ms*2 every increment 100ms//
	{
		tmr1Count1=0;
		tmr100ms++;
		errBz100ms++;
        cursor100ms++;
	}
	//
	if(tmr1Count2>4)	//50ms*4 every increment 200ms//
	{
		tmr1Count2=0;
		tmr200ms++;
	}
	//-------------------------------------
	if(tmr1Count3>5)	//50ms*10 every increment 500ms//
	{
		tmr1Count3=0;
		tmr500ms++;
	}
	//-------------------------------------
	if(tmr1Count4>20)	//50ms*20 every increment 1000ms//
	{
		tmr1Count4=0;
		tmr1s++;
		cursor1s++;
		key1s++;
	}
	//	
	IFS0bits.T1IF = 0;
}

void __attribute__((interrupt,auto_psv)) _T2Interrupt()	
{
	tmr2Count0++;
	tmr2Count1++;
	tmr2Count2++;
    tmr2Count3++;
	//
	IFS0bits.T2IF = 0;			
}
//
void DelayNop(unsigned int delaycnt)
{
	unsigned int j;
	
	for(j=0;j < delaycnt;j++)
		Nop();
}
//RTC
void rtc_init(void) 
{
	UINT8 x;
	RTC_RST=0;		
	Delay_Cnt(2);	
	RTC_CLK=0;		
	write_ds1302(0x8e,0);
	write_ds1302(0x90,0xa5);
	x=read_ds1302(0x81);
	if((x & 0x80)!=0)
	{	
		write_ds1302(0x80,0);
	} 
}
//
void write_ds1302(UINT8 cmd, UINT8 data) 
{
	RTC_RST=1;
	write_ds1302_byte(cmd);
	write_ds1302_byte(data);
	RTC_RST=0;
}
//
void write_ds1302_byte(UINT8 cmd) 
{
   UINT8 i;
   RTC_IO_DIR=0;
   RTC_CLK=0;
   for(i=0;i<=7;++i) 
   {
   		RTC_IO=(cmd & 0x01);
   		cmd>>=1;
     	RTC_CLK=1;
     	Delay_Cnt(2);
     	RTC_CLK=0;
     	Delay_Cnt(2);
   }
}
//
UINT8 read_ds1302(UINT8 cmd) 
{
	UINT8 i,data;   
	RTC_RST=1;
	write_ds1302_byte(cmd);
	RTC_IO_DIR=1;
	Delay_Cnt(2);
	data=0;
	for(i=0;i<=7;++i) 
	{
		data>>=1;
		if(RTC_IO)
		{
			data=data|0x80;
		}	
		else
		{
			data=data&0x7f;
		}	
		RTC_CLK=1;
		Delay_Cnt(2);
		RTC_CLK=0;
		Delay_Cnt(2);
	}
	RTC_RST=0;
	return(data);
}
//
UINT8 get_bcd(UINT8 data)
{
   UINT8 nibh;
   UINT8 nibl;
   nibh=data/10;
   nibl=data-(nibh*10);
   return((nibh<<4)|nibl);
}
//
UINT8 rm_bcd(UINT8 data)
{
   UINT8 i;	
   i=data;
   data=(i>>4)*10;
   data=data+(i&0x0f);
   return data;
}
//
void rtc_set_datetime(UINT8 day, UINT8 mth, UINT8 year, UINT8 dow, UINT8 hour, UINT8 min) 
{
   write_ds1302(0x86,get_bcd(day));	
   write_ds1302(0x88,get_bcd(mth));	
   write_ds1302(0x8c,get_bcd(year));
   write_ds1302(0x8a,get_bcd(dow));	
   write_ds1302(0x84,get_bcd(hour));
   write_ds1302(0x82,get_bcd(min));	
   write_ds1302(0x80,get_bcd(0));		
}

void rtc_get_date() 
{
   day = rm_bcd(read_ds1302(0x87));
   mth = rm_bcd(read_ds1302(0x89));
   year = rm_bcd(read_ds1302(0x8d));
   dow = rm_bcd(read_ds1302(0x8b));
}

void rtc_get_time()
{
   hour = rm_bcd(read_ds1302(0x85));
   min = rm_bcd(read_ds1302(0x83));
   sec = rm_bcd(read_ds1302(0x81));
}
//

//==============================================================
void Init_Interrupt()
{
	_NSTDIS 			= 1;
	IPC0bits.INT0IP		= 3;
	IPC0bits.T1IP 		= 5;
	IPC1bits.T2IP 		= 4;
	IPC6bits.U2RXIP 	= 2;
	IPC2bits.U1RXIP 	= 6;
	IFS0bits.T1IF 		= 0;					
	IFS0bits.T2IF 		= 0;					
	IFS0bits.INT0IF		= 0;
	_U1RXIF 			= 0;
	_U2RXIF 			= 0;	
	IEC0bits.ADIE 		= 0;
	IEC1bits.SPI2IE 	= 0;
	IEC0bits.T1IE 		= 1;
	IEC0bits.T2IE 		= 1;
	IEC0bits.U1RXIE 	= 1;
	IEC1bits.U2RXIE 	= 1;
	IEC0bits.INT0IE 	= 0;
	return;
}

void Delay10ms()		//10ms delay
{
	unsigned int i;
	for(i=0;i<15000;i++)		//15000�϶� 10ms
	{
		//
	}
}

void Delay2ms()
{
	unsigned int i;
	for(i=0;i<3000;i++)		//15000�϶� 10ms
	{
		//
	}
}

void Delay5ms()
{
	unsigned int i;
	for(i=0;i<7500;i++)		//15000�϶� 10ms
	{
		//
	}
}

void Delay500ms()
{
	unsigned char i;
	unsigned int j;
	for(i=0;i<50;i++)
	{
		for(j=0;j<15000;j++)		//15000�϶� 10ms
		{
			//
		}
	}
}

void Delay1s()
{
	unsigned char i;
	unsigned int j;
	for(i=0;i<100;i++)
	{
		for(j=0;j<15000;j++)		//15000�϶� 10ms
		{
			//
//__asm__ volatile ("CLRWDT");
		}
	}
}

void EEPROM_Initialize(void)
{
	if(read_EE(INIT_STATE) == 0)
	{
		write_EE(INIT_STATE, 	1);
		//
        write_EE(ALARM_STOP,			1);
        write_EE(SYS_TYPE,              0);
        write_EE(BATT_TYPE,             0);
        write_EE(OP_MODE,               0);
        
		write_EE(PASSWD_0,				'0');
		write_EE(PASSWD_1,				'0');
		write_EE(PASSWD_2,			    '0');
		write_EE(PASSWD_3,			    '0');
		write_EE(PASSWD_4,			    '0');
		write_EE(PASSWD_5,			    '0');
		write_EE(PASSWD_6,			    '0');
		write_EE(PASSWD_7,			    '0');
		write_EE(PASSWD_8,			    '0');
		write_EE(PASSWD_9,			    '0');
		//
        if(read_EE(BATT_TYPE) == 0)
        {
            write_EE(BATTVOLT_0,			'1');
            write_EE(BATTVOLT_1,			'5');
            write_EE(BATTVOLT_2,			'0');
            write_EE(BATTVOLT_3,			'0');
            write_EE(BATTVOLT_4,			'1');
            write_EE(BATTVOLT_5,			'0');
            write_EE(BATTVOLT_6,			'2');
            write_EE(BATTVOLT_7,			'0');
            write_EE(BATTVOLT_8,			'0');
            write_EE(BATTVOLT_9,			'0');
        }
        else
        {
            write_EE(BATTVOLT_0,			'1');
            write_EE(BATTVOLT_1,			'3');
            write_EE(BATTVOLT_2,			'5');
            write_EE(BATTVOLT_3,			'0');
            write_EE(BATTVOLT_4,			'0');
            write_EE(BATTVOLT_5,			'9');
            write_EE(BATTVOLT_6,			'0');
            write_EE(BATTVOLT_7,			'0');
            write_EE(BATTVOLT_8,			'0');
            write_EE(BATTVOLT_9,			'0');
        }
		//
		write_EE(RESTARTTIME_0,			1);
		write_EE(RESTARTTIME_1,			0);
		//
	}
}
//
void ParameterInitialize(void)
{
	gPass[0] = read_EE(PASSWD_0);
	gPass[1] = read_EE(PASSWD_1);
	gPass[2] = read_EE(PASSWD_2);
	gPass[3] = read_EE(PASSWD_3);
	gPass[4] = read_EE(PASSWD_4);
	gPass[5] = read_EE(PASSWD_5);
	gPass[6] = read_EE(PASSWD_6);
	gPass[7] = read_EE(PASSWD_7);
	gPass[8] = read_EE(PASSWD_8);
	gPass[9] = read_EE(PASSWD_9);
    
	fAlarmStop = read_EE(ALARM_STOP);
    gSysTypeMode = read_EE(SYS_TYPE); // 0 dual, 1 single//
    gBattTypeMode = read_EE(BATT_TYPE);// 0 pb125v, 1 pb 110v, 2 LiIon
    gOperationMode = read_EE(OP_MODE);// 0 standalone, 1 loadShare//
	//
	gBattVoltSet[0] = read_EE(BATTVOLT_0);
	gBattVoltSet[1] = read_EE(BATTVOLT_1);
	gBattVoltSet[2] = read_EE(BATTVOLT_2);
	gBattVoltSet[3] = read_EE(BATTVOLT_3);
	gBattVoltSet[4] = read_EE(BATTVOLT_4);
	gBattVoltSet[5] = read_EE(BATTVOLT_5);
	gBattVoltSet[6] = read_EE(BATTVOLT_6);
	gBattVoltSet[7] = read_EE(BATTVOLT_7);
	gBattVoltSet[8] = read_EE(BATTVOLT_8);
	gBattVoltSet[9] = read_EE(BATTVOLT_9);
	//
	gRestartTime[0] = read_EE(RESTARTTIME_0);
	gRestartTime[1] = read_EE(RESTARTTIME_1);
    
    
}
//void inputStatus

void SystemInitialize(void)
{	
	write_EE(ALARM_STOP,			1);
    write_EE(SYS_TYPE,              0);
    write_EE(BATT_TYPE,             0);
    write_EE(OP_MODE,               0);
    
    gSysTypeMode = read_EE(SYS_TYPE);
    gBattTypeMode = read_EE(BATT_TYPE);
    gOperationMode = read_EE(OP_MODE);
    
    if(read_EE(BATT_TYPE) == 0)//pb125v
    {
        write_EE(BATTVOLT_0,			'1');
        write_EE(BATTVOLT_1,			'5');
        write_EE(BATTVOLT_2,			'0');
        write_EE(BATTVOLT_3,			'0');
        write_EE(BATTVOLT_4,			'1');
        write_EE(BATTVOLT_5,			'0');
        write_EE(BATTVOLT_6,			'2');
        write_EE(BATTVOLT_7,			'0');
        write_EE(BATTVOLT_8,			'0');
        write_EE(BATTVOLT_9,			'0');
    }
    else						//pb110v, lituamIon
    {
        write_EE(BATTVOLT_0,			'1');
        write_EE(BATTVOLT_1,			'3');
        write_EE(BATTVOLT_2,			'5');
        write_EE(BATTVOLT_3,			'0');
        write_EE(BATTVOLT_4,			'0');
        write_EE(BATTVOLT_5,			'9');
        write_EE(BATTVOLT_6,			'0');
        write_EE(BATTVOLT_7,			'0');
        write_EE(BATTVOLT_8,			'0');
        write_EE(BATTVOLT_9,			'0');
    }
	//
	gBattVoltSet[0] = read_EE(BATTVOLT_0);
	gBattVoltSet[1] = read_EE(BATTVOLT_1);
	gBattVoltSet[2] = read_EE(BATTVOLT_2);
	gBattVoltSet[3] = read_EE(BATTVOLT_3);
	gBattVoltSet[4] = read_EE(BATTVOLT_4);
	gBattVoltSet[5] = read_EE(BATTVOLT_5);
	gBattVoltSet[6] = read_EE(BATTVOLT_6);
	gBattVoltSet[7] = read_EE(BATTVOLT_7);
	gBattVoltSet[8] = read_EE(BATTVOLT_8);
	gBattVoltSet[9] = read_EE(BATTVOLT_9);
	//
	write_EE(RESTARTTIME_0,			1);
	write_EE(RESTARTTIME_1,			0);
	//
	gRestartTime[0] = read_EE(RESTARTTIME_0);
	gRestartTime[1] = read_EE(RESTARTTIME_1);
}


