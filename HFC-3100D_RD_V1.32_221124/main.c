//--------------------------------------------------------------------------//
// 2022.04.05 : V1_30 --> V1_31 revision (modify goooo)                    //
//  dspic30f6012a - PT package                                              //
//  graphics lcd page0, page1, disAlign. fix                                //
//  lcdcmd(),lcdwrite() - LCD_RST care                                      //  
//  mplab x v5_35, C30(v3_31)                                               //
// -------------------------------------------------------------------------//
// 22.09.13 (goooo)                                                                 //
//   Add 'Set_BattAmp' set value @ParameterInit()                           //  
//--------------------------------------------------------------------------//

#include 	<p30f6012A.h>
//#include	<stdio.h>
//#include	"resource.h"
//#include 	"typedef.h"
#include 	"sysinfo.h"
#include 	"eeprom.h"
#include 	"Definition.h"
#include    "Initialize.h"
#include    "RS232.h"
#include    "ADC.h"
#include    "Alram_Sts.h"
#include	"fnt3_cd.h"		/* lcd display internal ram code header */
//
_FOSC(CSW_FSCM_OFF & HS2_PLL4)				//30MIPs at 15MHz crystal
_FWDT(WDT_OFF)								//watchdog disable
_FBORPOR(PBOR_ON & BORV_45 & PWRT_64)
        
//============================================================================//
//                          Function Declaration                              //
//============================================================================//
void write_ds1302_byte(UINT8 cmd);
void write_ds1302(UINT8 cmd, UINT8 data) ;
UINT8 read_ds1302(UINT8 cmd) ;
void rtc_init(void);
UINT8 get_bcd(UINT8 data);
UINT8 rm_bcd(UINT8 data);
void rtc_set_datetime(UINT8 day, UINT8 mth, UINT8 year, UINT8 dow, UINT8 hour, UINT8 min);
void rtc_get_date();
void rtc_get_time();

//---------------//
//EEPROM function//
//---------------//
void EEPROM_Initialize(void);

void ParameterInitialize(void);
void Init_Interrupt(void);
void ledTest(void);
void dispInit(void);
void errBz_On(void);

void Delay_Cnt(UINT16 cnt);

void lcdcmd(unsigned char dd);
void lcdfill(unsigned char dd);
void lcdwrite(unsigned char dd);
void lcd_inst(unsigned int RW,unsigned int DI,unsigned char lcd_data);					// LCD instruction routine
void lcd_poss(unsigned char x_poss,unsigned char y_poss);						// LCD x-y loading function
void lcd_put_char(unsigned char dchar);								// LCD data put charictor load
void dispMenu(void);
void MainProcess(void);
void delay500ms();
void Faultstate(void);

void dispLogo(void);
void digit_disp(void);

ds1302_Time dstime;

unsigned char buf_tmp = 1;

void Delay_Cnt(UINT16 cnt)
{
	UINT16 i;
		for(i=0;i<cnt;i++)
		{
			__asm__ volatile ("NOP");
		}
}
static unsigned int mainCounter=0;
int main(void)
{	
	while(OSCCONbits.LOCK!=1) {};
	//-----------------//
	//Initialize System//
	//-----------------//
    //__asm__ volatile ("CLRWDT");
	Init_Port();
	Init_Interrupt();
	Init_Uart2();           //RS232 Initializing
	Init_Timer1();			//timer1 init(50msec)	
	Init_Timer2();			//timer2 init (1msec)
	ADC_Init();             //ADC init & timer3 init
	rtc_init();
    DIR_RST=1;
	LCD_RST = HI;
	initLCD();	
	iMenuIndex=0;
	dispMenu();
	Set_EEPROM();
	EEPROM_Initialize();
	ParameterInitialize();
	dispMenu();
	Set_Value();//send to Rect Controller//
	dispInit();
	Set_Value();
	Set_OK = 1;
	Serial_Process();
	while(1)//main loop//
	{      
        Serial_Process();
        
		if(tmr2Count0 >= 10)				//10ms
		{
            
			//------------
			// ADC Processing
			//------------
            ACV_R_Raw = ADC_Conv(ADC_ACV_R, ACV_R_Raw);
            ACV_S_Raw = ADC_Conv(ADC_ACV_S, ACV_S_Raw);	
            ACV_T_Raw = ADC_Conv(ADC_ACV_T, ACV_T_Raw);		
            ACI_R_Raw = ADC_Conv(ADC_ACI_R, ACI_R_Raw);				
            ACI_S_Raw = ADC_Conv(ADC_ACI_S, ACI_S_Raw);				
            ACI_T_Raw = ADC_Conv(ADC_ACI_T, ACI_T_Raw);	
            
            ACV_R = (ACV_R_Raw*ADC_Gain);          //Real Value
            ACV_S = (ACV_S_Raw*ADC_Gain);          //Real Value	
            ACV_T = (ACV_T_Raw*ADC_Gain);          //Real Value	
            ACI_R = (ACI_R_Raw*ADC_Gain);          //Real Value
            ACI_S = (ACI_S_Raw*ADC_Gain);          //Real Value	
            ACI_T = (ACI_T_Raw*ADC_Gain);          //Real Value		
//		    mainCounter++;
//            if(mainCounter>1000)//10ms * 1500 = 15000ms(15second)//
//            {    
//                if(f_FirstComm==0xaa)
//                {    
//                    f_FirstComm=0x55;//
//                    Set_OK=1;
//                    Set_Value();
//                }
//                mainCounter=0;
//            }
//            if(f_FirstComm==0x55)
//            {
//                ACV_R=381.1;
//                ACV_S=382.2;
//                ACV_T=383.3;
//            }
            
            //--------------------------------
            Alram_Sts();
			tmr2Count0 = 0;
			__asm__ volatile ("CLRWDT");		//#1
		}
        
		if(tmr2Count1 >= 100)//100ms
		{
			tmr2Count1 = 0;
			dispMenu();
			Faultstate();
			errBz_On();
            CHG_Check();
		}
        
		if(tmr2Count2 >= 5)
		{
			tmr2Count2=0;
			up=!UP,down=!DOWN,esc=!ESC,enter=!ENT;
			if(up||down||esc||enter)
			{
				if(fKey==1) setKey=0;
				else
				{
					fKey=1;
					setKey=1;
					BL=1;
					tmr500ms=0;
				}
			}
			else fKey=0,setKey=0;
		}
		key_oper();
		digit_disp();
		if(tmr500ms>=60 && BL==1)
		{
			tmr500ms=0;
			BL=0;
		}        
	}
}

void __attribute__((interrupt,auto_psv)) _U1RXInterrupt()	//RS232
{
	_U1RXIF = 0;
}

void __attribute__((interrupt,auto_psv)) _T1Interrupt()
{
    tmr1Count1++;
    tmr1Count3++;
	tmr1Count4++;
    if(tmr1Count1>=2)	//50ms*2 every increment 100ms//
	{
		tmr1Count1=0;
		errBz100ms++;
	}
    if(tmr1Count3>10)	//50ms*10 every increment 500ms//
	{
		tmr1Count3=0;
		tmr500ms++;
	}
	//-------------------------------------
    if(tmr1Count4 == (20*60))	//50ms*20 = 1000ms * 60 = 1Min //
	{
		tmr1Count4 = 0;
        tmr1min++;
        if(tmr1min == 60)
        {
            tmr1min = 0;
            Cut_TIMER++;        //tmr1hour++;
            if(Cut_TIMER > 99) Cut_TIMER = 99;
        }
	}
	IFS0bits.T1IF = 0;
}

void __attribute__((interrupt,auto_psv)) _T2Interrupt()	
{    
	tmr2Count0++;
	tmr2Count1++;
	tmr2Count2++;
	tmr2Count3++;
	IFS0bits.T2IF = 0;
}

void Time_Set()
{
	rtc_set_datetime(day, mth, year, 0, hour, min); 
}

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

void write_ds1302(UINT8 cmd, UINT8 data) 
{
	RTC_RST=1;
	write_ds1302_byte(cmd);
	write_ds1302_byte(data);
	RTC_RST=0;
}

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

UINT8 get_bcd(UINT8 data)
{
   UINT8 nibh;
   UINT8 nibl;
   nibh=data/10;
   nibl=data-(nibh*10);
   return((nibh<<4)|nibl);
}

UINT8 rm_bcd(UINT8 data)
{
   UINT8 i;	
   i=data;
   data=(i>>4)*10;
   data=data+(i&0x0f);
   return data;
}

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

void ledTest()
{
	if(lampTestFlag==1)
	{
		SP2=0;
		Floating=0;
		EQUALIZING=0;
		MANUAL=0;
	}
	else if(lampTestFlag==0)
	{
		SP2=1;
		Floating=1;
		EQUALIZING=1;
		MANUAL=1;
	}
}

void dispInit()
{
	delay500ms();
	delay500ms();
	delay500ms();
	delay500ms();
	NORMAL=ON;
	delay500ms();
	NORMAL=OFF;
	FAULT=ON;
	delay500ms();
	FAULT=OFF;
	Floating=ON;
	delay500ms();
	Floating=OFF;
	EQUALIZING=ON;
	delay500ms();
	EQUALIZING=OFF;
	MANUAL=ON;
	delay500ms();
	MANUAL=OFF;
	delay500ms();
	delay500ms();
	delay500ms();
	delay500ms();
	iMenuIndex=1;
}

void errBz_On()
{
	unsigned char data_8=0;
	data_8 = read_EE(ALARM_STOP);
	if((fFaultState == 1) || (fAlarmState == 1))
	{
		if(data_8 == 'Y')
		{
			if(errBz100ms>=20)
			{
				BZ_OFF();
			}
			if(errBz100ms>=40)
			{
				errBz100ms=0;
				BZ_ON();
			}
		}
		else
		{
			BZ_OFF();
		}
		fFaultState=0;
		fAlarmState=0;
	}
	else BZ_OFF();
}

void dispMenu()
{
	unsigned char x_cnt,y_cnt;//,data_8;
	unsigned int data_16;
	for(y_cnt=0;y_cnt<8;y_cnt++)
	{
		lcd_poss(0,y_cnt);_NOP();//delay(10);
		for(x_cnt=0;x_cnt<64;x_cnt++)
		{
			data_16 = x_cnt+(y_cnt*128);
			lcd_inst(LO,HI,MENUD[iMenuIndex][data_16]);
			_NOP();_NOP();_NOP();_NOP();_NOP();_NOP();
		}
		lcd_poss(64,y_cnt);_NOP();//delay(10);
		for(x_cnt=64;x_cnt<128;x_cnt++)
		{
			data_16 = x_cnt+(y_cnt*128);
			lcd_inst(LO,HI,MENUD[iMenuIndex][data_16]);
			_NOP();_NOP();_NOP();_NOP();_NOP();_NOP();
		}
	}
}

void dispLogo()
{
	unsigned int x_cnt,y_cnt,data_16;
	for(y_cnt=0;y_cnt<8;y_cnt++) {
				lcd_poss(0,y_cnt);_NOP();_NOP();_NOP();//delay(10);
				for(x_cnt=0;x_cnt<64;x_cnt++) {
					data_16 = x_cnt+(y_cnt*128);
					lcd_inst(LO,HI,MENUD[0][data_16]);
					_NOP();_NOP();_NOP();_NOP();_NOP();_NOP();
					_NOP();_NOP();_NOP();_NOP();_NOP();_NOP();
				}
				lcd_poss(64,y_cnt);_NOP();_NOP();_NOP();//delay(10);
				for(x_cnt=64;x_cnt<128;x_cnt++) {
					data_16 = x_cnt+(y_cnt*128);
					lcd_inst(LO,HI,MENUD[0][data_16]);
					_NOP();_NOP();_NOP();_NOP();_NOP();_NOP();
					_NOP();_NOP();_NOP();_NOP();_NOP();_NOP();
				}
			}	
}

//    LCD busy check  routine
void	lcdwrite(unsigned char dd) 
{
    unsigned int ibuf;
    D_I=HI;
    ibuf = (unsigned int)dd;
    LCD_DATA = ibuf & 0xFEFF;//R/W(RD8) to low for Write Mode//
    if(cs_bit == 0)CS1=HI;
    else CS2=HI;
    EN = HI;
    Delay_Cnt(5); //about 6us//
    EN = LO;
    Delay_Cnt(5); 
    /*
    unsigned char buf;
    _NOP();_NOP();_NOP();//delay(10);
    R_W = LO;D_I = HI;
    buf=LCD_DATA;
    LCD_DATA=buf|(dd&0xFF);
    if(cs_bit == 0) CS1=1;
    else CS2 = 1;
    _NOP();EN = HI;
    _NOP();_NOP();
    EN = LO;_NOP();
    CS1 = LO;CS2 = LO;
    LCD_DATA=buf&0xFF00;
    */
 }//lcdwrite end

void	lcdcmd(unsigned char dd)
{
    unsigned int ibuf;
    D_I = LO;
    ibuf = (unsigned int)dd;
    LCD_DATA = ibuf & 0xFEFF;//R/W(RD8) to low for Write Mode//
    if(cs_bit == 0)CS1=HI;
    else CS2=HI;
    EN = HI;
    Delay_Cnt(5); //about 6us//
    EN = LO;
    Delay_Cnt(5); 
    /*
    unsigned char buf;
    _NOP();_NOP();_NOP();
    R_W = LO;D_I = LO;
    buf=LCD_DATA;
    LCD_DATA=dd&0xFF;
    if(cs_bit == 0) CS1=1;
    else CS2 = 1;			
    _NOP();_NOP();
    EN = HI;_NOP();
    EN = LO;_NOP();
    CS1 = LO;CS2 = LO;
    LCD_DATA=0xFFFF;
    */
}//lcdcmd end

void initLCD()
{
    LCD_RST = HI;
    Delay_Cnt(5);    
    //
    cs_bit = 0;
	lcdcmd(0x3E);//Display Off//
    cs_bit = 1;
	lcdcmd(0x3E);
    //    	
	cs_bit = 0;
	lcdcmd(0xc0);//Set z address(start line 0)//
    cs_bit = 1;
	lcdcmd(0xc0);
    //    
    cs_bit = 0;
	lcdcmd(0x3f);//Display On//	
    cs_bit = 1;
	lcdcmd(0x3f);
    //
	lcdfill(0);
}// lcdinit end

//LCD clear routine	
void	lcdfill(unsigned char dd) {
		unsigned char lcdx,lcdy;
		unsigned char cnt;
		delay(10);
		cs_bit = LO;CS1 = HI;CS2 = LO;
		for(cnt=0;cnt<2;cnt++) {
			for(lcdy=0;lcdy<8;lcdy++) {
				lcdcmd(lcdy|0xb8);_NOP();_NOP();
				lcdcmd(0x40);_NOP();_NOP();
				for(lcdx=0;lcdx<64;lcdx++) {
					lcdwrite(dd);
					_NOP();_NOP();
					WDT = HI;
				}
			}
			cs_bit = HI;
			CS1 = LO;CS2 = HI;
		}
		cs_bit = LO;
		CS1 = LO;CS2 = LO;
	}//lcdfill end

void delay500ms()
{
	unsigned char i;
	unsigned int j;
	for(i=0;i<50;i++)
	{
		for(j=0;j<15000;j++)		//15000 * 50 => 500ms
		{
			//
		}
	}
}

void	delay(unsigned int dly_time) {
		while(dly_time == 0) {
			WDT = HI;_NOP();_NOP();_NOP();WDT = LO;
			dly_time--;			
		}
		WDT = HI;
	}//delay end

void MainProcess()
{
		switch(iMenuIndex)
		{
			case M0_LOGO:
				dispLogo();
				break;
				
			default: dispLogo();
				break;
		}	// END switch (iMenuIndex)
}

void	lcd_reflash() {
		if(soft_1sec[2] == 0) {
			soft_1sec[2] = REFLASH_LCD;
			if(!modi) {
				LCD_RST = LO;
				_NOP();_NOP();_NOP();
				LCD_RST = HI;
				initLCD();										// LCD module initializing
				iMenuIndex=0;
				dispMenu();
			}
			else soft_1sec[2] = REFLASH_LCD;
		}
	}//lcd_reflash end

void Faultstate()
{
	unsigned char faultstate=0;
	if(Set_Output_UV > Voltage_Out)
	{
		FAULT=ON;
		fAlarmState = 1;
		Output_UV=0x40;
	}
	else
	{
		if(fAlarmState == 1 || fFaultState == 1)	FAULT=ON;
		else	FAULT=OFF;
		Output_UV=0x00;
	}
	if(Set_Output_OV < Voltage_Out)
	{
		FAULT=ON;
		fAlarmState = 1;
		Output_OV=0x80;
	}
	else
	{
		if(fAlarmState == 1 || fFaultState == 1)	FAULT=ON;
		else	FAULT=OFF;
		Output_OV=0x00;
	}
	if(Set_ARef < Current_Out)
	{
		FAULT=ON;
		fAlarmState = 1;
	}
	else
	{
		if(fAlarmState == 1 || fFaultState == 1)	FAULT=ON;
		else	FAULT=OFF;
	}
	faultstate = Input_OV | Input_UV | Input_Shutdown;
	if(faultstate == 0x00 && (Rx_Char == 0x80 || Rx_Char == 0x00))
	{
		if(fAlarmState == 0)
		{
			NORMAL=ON;
			FAULT=OFF;
		}
		else
		{
			NORMAL=OFF;
			FAULT=ON;
		}			
	}		
    else NORMAL=OFF, FAULT=ON, fFaultState = 1;
}

void key_oper()
{
	unsigned char	data_8,data_cal,i,cnt,buff_8;
	unsigned int	data_16,buff_16;

	if(setKey==1)
	{
		switch(iMenuIndex)
		{							//       down key < CoAc > up key   //
			case(1): {							// 4.system setup <0.logo> 1.input volt & amp
				if(up) iMenuIndex = 5;
				else if(down) iMenuIndex = 2;
				key_clr();
			}break;
			case(2): {
				if(up) iMenuIndex = 1;
				else if(down) iMenuIndex = 3;
				key_clr();
			}break;
			case(3): {
				if(up) iMenuIndex = 2;
				else if(down) iMenuIndex = 4;
				key_clr();
			}break;
			case(4): {
				if(up) iMenuIndex = 3;
				else if(down) iMenuIndex = 5;
				key_clr();
			}break;
            
			case(5): {
				if(!modi)
				{
					if(up) iMenuIndex = 4;
					else
					{
						if(down) iMenuIndex = 1;
						else if(enter)
						{		//press Enter key routine -> else is Enter key pressed routine
							modi = HI;
							poss_x = 0;
							for(cnt=0;cnt<6;cnt++) temp_buffer[cnt] = '0';
						}
					}
					key_clr();
				}
				else
				{	//system setup passwd display Enter Key
					if(up)
					{
						data_8 = temp_buffer[poss_x];
						if(data_8 == '9') data_8 = '0';
						else data_8++;
						temp_buffer[poss_x] = data_8;
					}
					else
					{
						if(down)
						{
							data_8 = temp_buffer[poss_x];
							if(data_8 == '0') data_8 = '9';
							else data_8--;
							temp_buffer[poss_x] = data_8;
						}
						else if(enter)
						{
							if(poss_x >= 5)
							{
								i = 0;modi = LO;
								for(cnt=0;cnt<6;cnt++)
								{
									data_8 = gPin[cnt];
									data_cal = temp_buffer[cnt];
									i += (data_8 ^ data_cal);
								}
								if(i == 0) 
                                {
                                    if(read_EE(BATT_TYPE) == 2) iMenuIndex = 9;
                                    else iMenuIndex = 10;
                                }
                                    
								else
								{
									i = 0;
									for(cnt=0;cnt<6;cnt++)
									{
										data_8 = gPin[cnt];
										data_cal = temp_buffer[cnt];
										i += (data_8 ^ data_cal);
									}
									if(i == 0) 
                                    {
                                        if(read_EE(BATT_TYPE) == 2) iMenuIndex = 9;
                                        else iMenuIndex = 10;
                                    }
								}
							}						
							else poss_x++;
						}
						if(esc) {
							modi = LO;
							iMenuIndex = 5;		//pressed ESC key Return to System Setting Password Menu
						}
					}
					key_clr();
				}
			}break;
            
            case(9): {
				if(!modi)              //
				{
					if(up)iMenuIndex = 25;
					else 
					{
						if(down) iMenuIndex = 11;
						else if(enter) 
						{
							modi = HI;
                            MANUAL=OFF;
							poss_x = poss_y = 0;
							for(cnt=0;cnt<8;cnt++) battVoltSet[cnt] = read_EE(BATTVOLT_0+cnt);
						}
					}
					if(esc) iMenuIndex = 5;
					key_clr();
				}
				else 						//later Enter key Menu in state// adjust volt. value//
				{
					if(up) 
					{
						data_8 = battVoltSet[poss_x];
						switch(poss_x) 	
						{
							case(0):
                            {
                                if(data_8 >= '1') data_8 = '0';
								else data_8++;
                            }
							break;
								//
							case(1):		
								{
									if(battVoltSet[0] <= '0')
									{	 
										data_8 = '9';										
									}	
									else if(battVoltSet[0] >= '1')
									{
										if(data_8 >= '3') data_8 = '0';
                                        else if (data_8 < '0') data_8 = '3';
										else data_8++;
									}
							}break;
								//
							case(2):
								{
									if((battVoltSet[0] <= '0') && (battVoltSet[1] <= '9'))
									{
										if(data_8 >= '9') data_8 = '6';
                                        else if(data_8 < '6') data_8 = '9';
										else data_8++;
									}
									else if((battVoltSet[0] >= '1') && (battVoltSet[1] >= '3'))
									{
										if(data_8 >= '5') data_8 = '0';
                                        else if(data_8 < '0') data_8 = '5';
										else data_8++;
									}
									else
									{
										if(data_8 >= '9') data_8 = '0';
										else data_8++;
									}
							}break;
							//
							case(3):
								{
									if((battVoltSet[0] >= '1') && (battVoltSet[1] >= '3') && (battVoltSet[2] >= '5'))
									{
										data_8 = '0';
									}								
									else
									{
										if(data_8 >= '9') data_8 = '0';
										else data_8++;
									}
							}break;
						}//end - switch(poss_x)
						battVoltSet[poss_x] = data_8;
					}
					else //down key pressed 
					{
						if(down) 						//case(n) => fl [0][01][2].[3]  eq [4][5][6].[7]
						{
							data_8 = battVoltSet[poss_x];
							switch(poss_x) 
							{
								case(0): 
								{
                                    if(data_8 <= '0') data_8 = '1';
                                    else data_8--;
                                }
								break;
								//
								case(1):
								{
									if(battVoltSet[0] <= '0')
									{	 
										data_8 = '9';
									}	
									else if(battVoltSet[0] >= '1')
									{
										if(data_8 <= '0') data_8 = '3';
                                        else if(data_8 > '3') data_8 = '0';
										else data_8--;
									}
                                    else
                                    {
                                        if(data_8 <= '0') data_8 = '9';
										else data_8--;
                                    }
								}
								break;
								//
								case(2): 								//xxN.x	// fl [0][01][2].[3]  eq [4][5][6].[7]
								{
									if((battVoltSet[0] <= '0') && (battVoltSet[1] <= '9'))
									{
										if(data_8 <= '6') data_8 = '9';
                                        else if(data_8 > '9') data_8 = '6';
										else data_8--;
									}
									else if((battVoltSet[0] >= '1') && (battVoltSet[1] >= '3'))
									{
										if(data_8 <= '0') data_8 = '5';
                                        else if(data_8 > '5') data_8 = '0';
										else data_8--;
									}
									else
									{
										if(data_8 <= '0') data_8 = '9';
										else data_8--;
									}
								}
								break;
								//
								case(3):
								{
									if((battVoltSet[0] >= '1') && (battVoltSet[1] >= '3') && (battVoltSet[2] >= '5'))
									{
										data_8 = '0';
									}								
									else
									{
										if(data_8 <= '0') data_8 = '9';
										else data_8--;
									}
								}break;				
							}//end - switch
							battVoltSet[poss_x]=data_8;
						}
						else if(enter) 
						{
							if(poss_x >= 3) 
							//if(poss_x >= 3) 	//goo//4,5,6,7 
							{
								modi = LO;
                                data_16 = 0;
                                data_16 += (battVoltSet[0]-0x30) * 1000;
                                data_16 += (battVoltSet[1]-0x30) * 100;
                                data_16 += (battVoltSet[2]-0x30) * 10;
                                data_16 += (battVoltSet[3]-0x30);
								if(data_16 > MaxOutputVoltage_LI*10)
								{
									battVoltSet[0] = ((MaxOutputVoltage_LI*10)/1000)+0x30;
									battVoltSet[1] = (((MaxOutputVoltage_LI*10)-((battVoltSet[0]-0x30)*1000))/100)+0x30;
									battVoltSet[2] = (((MaxOutputVoltage_LI*10)-((battVoltSet[0]-0x30)*1000)-((battVoltSet[1]-0x30)*100))/10)+0x30;
									battVoltSet[3] = ((MaxOutputVoltage_LI*10)-((battVoltSet[0]-0x30)*1000)-((battVoltSet[1]-0x30)*100)-((battVoltSet[2]-0x30)*10))+0x30;
								}
								else if(data_16 < MinOutputVoltage_LI*10)
								{
									battVoltSet[0] = ((MinOutputVoltage_LI*10)/1000)+0x30;
									battVoltSet[1] = (((MinOutputVoltage_LI*10)-((battVoltSet[0]-0x30)*1000))/100)+0x30;
									battVoltSet[2] = (((MinOutputVoltage_LI*10)-((battVoltSet[0]-0x30)*1000)-((battVoltSet[1]-0x30)*100))/10)+0x30;
									battVoltSet[3] = ((MinOutputVoltage_LI*10)-((battVoltSet[0]-0x30)*1000)-((battVoltSet[1]-0x30)*100)-((battVoltSet[2]-0x30)*10))+0x30;
								}
                                battVoltSet[4] = battVoltSet[0];
                                battVoltSet[5] = battVoltSet[1];
                                battVoltSet[6] = battVoltSet[2];
                                battVoltSet[7] = battVoltSet[3];
								for(cnt=0;cnt<8;cnt++) write_EE(BATTVOLT_0+cnt, battVoltSet[cnt]);
								Set_VRef = ((battVoltSet[0]-0x30)*100)+((battVoltSet[1]-0x30)*10)+((battVoltSet[2]-0x30)*1)+((battVoltSet[3]-0x30)*0.1);
								//Set_CL=((battVoltSet[4]-0x30)*100)+((battVoltSet[5]-0x30)*10)+(battVoltSet[6]-0x30);
								cal_error = LO;modi_end = HI;Set_OK=1;fEqFl=0;
							}
							else poss_x++;
							//============================================
						}
					}					
					if(esc) modi = LO;
					key_clr();
				}
            }break;
            
            
			case(10): {
				if(!modi)              //
				{       
					if(up)
                    {
                        if(read_EE(BATT_TYPE) == 2) iMenuIndex = 9;
                        else iMenuIndex = 25;
                    }
					else 
					{
						if(down) iMenuIndex = 11;
						else if(enter) 
						{
							modi = HI;
							poss_x = poss_y = 0;
							for(cnt=0;cnt<8;cnt++) battVoltSet[cnt] = read_EE(BATTVOLT_0+cnt);
						}
					}
					if(esc) iMenuIndex = 5;
					key_clr();
				}
				else 						//later Enter key Menu in state// adjust volt. value//
				{
                    if(read_EE(BATT_TYPE) == 0)
                    {
                        if(up) 
                        {
                            data_8 = battVoltSet[poss_x];
                            switch(poss_x) 	
                            {
                                case(0):
                                    data_8 = '1';
                                break;
                                    //
                                case(1):		
                                    {
                                        if(data_8 >= '4') data_8 = '2';
                                        else if(data_8 < '2') data_8 = '4';
                                        else data_8++;
                                    }break;
                                    //
                                case(2):
                                    {
                                        if(battVoltSet[0] <= '1' && battVoltSet[1] <= '2')
                                        {
                                            if(data_8 >= '9') data_8 = '4';
                                            else if(data_8 < '4') data_8 = '9';
                                            else data_8++;
                                        }
                                        else if(battVoltSet[0] >= '1' && battVoltSet[1] >= '4')
                                        {
                                            if(data_8 >= '1') data_8 = '0';
                                            else if(data_8 < '0') data_8 = '1';
                                            else data_8++;
                                        }
                                        else 
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                    }break;
                                //
                                case(3):
                                    {
                                        if(battVoltSet[0] <= '1' && battVoltSet[1] <= '2' && battVoltSet[2] <= '4')
                                        {
                                           if(data_8 >= '9') data_8 = '7';
                                           else if(data_8 < '7') data_8 = '9';
                                            else data_8++;
                                        }
                                        else if(battVoltSet[0] >= '1' && battVoltSet[1] >= '4' && battVoltSet[2] >= '1') data_8 = '0';
                                        else
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                    }break;
                                case(4):
                                    {
                                        data_8 = '1';
                                    }break;
                                    //
                                case(5):		
                                    {
                                        if(data_8 >= '4') data_8 = '2';
                                        else data_8++;                                   
                                    }break;
                                    //
                                case(6):
                                    {
                                        if(battVoltSet[4] <= '1' && battVoltSet[5] <= '2')
                                        {
                                            if(data_8 >= '9') data_8 = '7';
                                            else if(data_8 < '7') data_8 = '9';
                                            else data_8++;
                                        }
                                        else if(battVoltSet[4] >= '1' && battVoltSet[5] >= '4')
                                        {
                                            if(data_8 >= '7') data_8 = '0';
                                            else if(data_8 < '0') data_8 = '7';
                                            else data_8++;
                                        }
                                        else 
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                    }break;
                                //
                                case(7):
                                    {
                                        if(battVoltSet[4] <= '1' && battVoltSet[5] <= '2' && battVoltSet[6] <= '7') 
                                        {
                                            if(data_8 >= '9') data_8 = '6';
                                            else if(data_8 < '6') data_8 = '9';
                                            else data_8++;
                                        }
                                        else if(battVoltSet[4] >= '1' && battVoltSet[5] >= '4' && battVoltSet[6] >= '7') 
                                        {
                                            data_8 = '0';
                                        }
                                        else
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                    }break;    
                            }//end - switch(poss_x)
                            battVoltSet[poss_x] = data_8;
                        }
                        else //down key pressed 
                        {
                            if(down) 						//case(n) => fl [0][01][2].[3]  eq [4][5][6].[7]
                            {
                                data_8 = battVoltSet[poss_x];
                                switch(poss_x) 
                                {
                                    case(0): 
                                    data_8 = '1';
                                    break;
                                    //
                                    case(1):
                                    {
                                        if(data_8 <= '2') data_8 = '4';
                                        else if(data_8 > '4') data_8 = '2';
                                        else data_8--;
                                    }
                                    break;
                                    //
                                    case(2): 								//xxN.x	// fl [0][01][2].[3]  eq [4][5][6].[7]
                                    {
                                        if(battVoltSet[0] <= '1' && battVoltSet[1] <= '2')
                                        {
                                            if(data_8 <= '4') data_8 = '9';
                                            else if(data_8 > '9') data_8 = '4';
                                            else data_8--;
                                        }
                                        else if(battVoltSet[0] >= '1' && battVoltSet[1] >= '4')
                                        {
                                            if(data_8 <= '0') data_8 = '1';
                                            else if(data_8 > '1') data_8 = '0';
                                            else data_8--;
                                        }
                                        else
                                        {
                                            if(data_8 <= '0') data_8 = '9';
                                            else data_8--;
                                        }
                                    }
                                    break;
                                    //
                                    case(3):
                                    {
                                        if(battVoltSet[0] <= '1' && battVoltSet[1] <= '2' && battVoltSet[2] <= '4')
                                        {
                                            if(data_8 <= '7') data_8 = '9';
                                            else if(data_8 > '9') data_8 = '7';
                                            else data_8--;
                                        }
                                        else if(battVoltSet[0] >= '1' && battVoltSet[1] >= '4' && battVoltSet[2] >= '1') data_8 = '0';
                                        else
                                        {
                                            if(data_8 <= '0') data_8 = '9';
                                            else if(data_8 > '9') data_8 = '0';
                                            else data_8--;
                                        }
                                    }break;	
                                    case(4): 
                                    {
                                       data_8 = '1';
                                    }
                                    break;
                                    //
                                    case(5):
                                    {
                                        if(data_8 <= '2') data_8 = '4';
                                        else if(data_8 > '4') data_8 = '2';
                                        else data_8--;
                                    }
                                    break;
                                    //
                                    case(6): 								//xxN.x	// fl [0][01][2].[3]  eq [4][5][6].[7]
                                    {
                                        if(battVoltSet[4] <= '1' && battVoltSet[5] <= '2') 
                                        {
                                            if(data_8 <= '7') data_8 = '9';
                                            else data_8--;
                                        }
                                        else if(battVoltSet[4] >= '1' && battVoltSet[5] >= '4') 
                                        {
                                            if(data_8 <= '0') data_8 = '7';
                                            else data_8--;
                                        }
                                        else
                                        {
                                            if(data_8 <= '0') data_8 = '9';
                                            else data_8--;
                                        }
                                    }
                                    break;
                                    //
                                    case(7):
                                    {
                                        if(battVoltSet[4] <= '1' && battVoltSet[5] <= '2' && battVoltSet[6] <= '7')
                                        {
                                            if(data_8 <= '6') data_8 = '9';
                                            else data_8--;
                                        }
                                        else if(battVoltSet[4] >= '1' && battVoltSet[5] >= '4' && battVoltSet[6] >= '7') data_8 = '0';
                                        else
                                        {
                                            if(data_8 <= '0') data_8 = '9';
                                            else data_8--;
                                        }
                                    }break;	
                                }//end - switch
                                battVoltSet[poss_x]=data_8;
                            }
                            else if(enter) 
                            {
                                if(poss_x >= 7) 
                                {
                                    modi = LO;
                                    CHG_EN = 0;
                                    data_16 = 0;
                                    data_16 +=((battVoltSet[0] & 0x0f) * 1000);
                                    data_16 +=((battVoltSet[1] & 0x0f) * 100);
                                    data_16 +=((battVoltSet[2] & 0x0f) * 10);
                                    data_16 += (battVoltSet[3] & 0x0f);
                                    if(data_16 < (MinOutputVoltage_PB1*10))
                                    {
                                        battVoltSet[0] = ((MinOutputVoltage_PB1*10)/ 1000) + 0x30;
                                        battVoltSet[1] = (((MinOutputVoltage_PB1*10) - ((battVoltSet[0] & 0x0f) * 1000))/100) + 0x30;
                                        battVoltSet[2] = (((MinOutputVoltage_PB1*10) - ((battVoltSet[0] & 0x0f) * 1000) - ((battVoltSet[1] & 0x0f) * 100))/10)+ 0x30;
                                        battVoltSet[3] = ((MinOutputVoltage_PB1*10) - ((battVoltSet[0] & 0x0f) * 1000) - ((battVoltSet[1] & 0x0f) * 100) - ((battVoltSet[2] & 0x0f)*10)) + 0x30;
                                    }
                                    else if(data_16 > (MaxOutputVoltage_PB1*10))
                                    {
                                        battVoltSet[0] = ((MaxOutputVoltage_PB1*10)/ 1000) + 0x30;
                                        battVoltSet[1] = (((MaxOutputVoltage_PB1*10) - ((battVoltSet[0] & 0x0f) * 1000))/100) + 0x30;
                                        battVoltSet[2] = (((MaxOutputVoltage_PB1*10) - ((battVoltSet[0] & 0x0f) * 1000) - ((battVoltSet[1] & 0x0f) * 100))/10)+ 0x30;
                                        battVoltSet[3] = ((MaxOutputVoltage_PB1*10) - ((battVoltSet[0] & 0x0f) * 1000) - ((battVoltSet[1] & 0x0f) * 100) - ((battVoltSet[2] & 0x0f)*10)) + 0x30;
                                    }   

                                    data_16 = 0;
                                    data_16 +=((battVoltSet[4] & 0x0f) * 1000);
                                    data_16 +=((battVoltSet[5] & 0x0f) * 100);
                                    data_16 +=((battVoltSet[6] & 0x0f) * 10);
                                    data_16 += (battVoltSet[7] & 0x0f);
                                    if(data_16 < (MinOutputVoltage_PB2*10))
                                    {
                                        battVoltSet[4] = ((MinOutputVoltage_PB2*10)/ 1000) + 0x30;
                                        battVoltSet[5] = (((MinOutputVoltage_PB2*10) - ((battVoltSet[4] & 0x0f) * 1000))/100) + 0x30;
                                        battVoltSet[6] = (((MinOutputVoltage_PB2*10) - ((battVoltSet[4] & 0x0f) * 1000) - ((battVoltSet[5] & 0x0f) * 100))/10)+ 0x30;
                                        battVoltSet[7] = ((MinOutputVoltage_PB2*10) - ((battVoltSet[4] & 0x0f) * 1000) - ((battVoltSet[5] & 0x0f) * 100) - ((battVoltSet[6] & 0x0f)*10)) + 0x30;
                                    }
                                    else if(data_16 > (MaxOutputVoltage_PB2*10))
                                    {
                                        battVoltSet[4] = ((MaxOutputVoltage_PB2*10)/ 1000) + 0x30;
                                        battVoltSet[5] = (((MaxOutputVoltage_PB2*10) - ((battVoltSet[4] & 0x0f) * 1000))/100) + 0x30;
                                        battVoltSet[6] = (((MaxOutputVoltage_PB2*10) - ((battVoltSet[4] & 0x0f) * 1000) - ((battVoltSet[5] & 0x0f) * 100))/10)+ 0x30;
                                        battVoltSet[7] = ((MaxOutputVoltage_PB2*10) - ((battVoltSet[4] & 0x0f) * 1000) - ((battVoltSet[5] & 0x0f) * 100) - ((battVoltSet[6] & 0x0f)*10)) + 0x30;       
                                    } 
                                    for(cnt=0;cnt<8;cnt++) write_EE(BATTVOLT_0+cnt, battVoltSet[cnt]);                            
                                    CHG_SEL = 0;
                                    CHG_CMD();
                                    cal_error = LO;modi_end = HI;Set_OK=1;fEqFl=0; 
                                }
                                else poss_x++;
                                //============================================
                            }
                        }	
                    }
                    else if(read_EE(BATT_TYPE) == 1)
                    {
                        if(up) 
                        {
                            data_8 = battVoltSet[poss_x];
                            switch(poss_x) 	
                            {
                                case(0):
                                    data_8 = '1';
                                break;
                                    //
                                case(1):		
                                    {
                                        if(data_8 >= '3') data_8 = '1';
                                        else data_8++;
                                    }break;
                                    //
                                case(2):
                                    {
                                        if(battVoltSet[0] <= '1' && battVoltSet[1] <= '1')
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                        else if(battVoltSet[0] >= '1' && battVoltSet[1] >= '3')
                                        {
                                            data_8 = '0';
                                        }
                                        else 
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                    }break;
                                //
                                case(3):
                                    {
                                        if(battVoltSet[0] <= '1' && battVoltSet[1] <= '1' && battVoltSet[2] <= '0')
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                        else if(battVoltSet[0] >= '1' && battVoltSet[1] >= '3' && battVoltSet[2] >= '0') data_8 = '0';
                                        else
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                    }break;
                                case(4):
                                    {
                                        data_8 = '1';
                                    }break;
                                    //
                                case(5):		
                                    {
                                        if(data_8 >= '5') data_8 = '2';
                                        else data_8++;                                   
                                    }break;
                                    //
                                case(6):
                                    {
                                        if(battVoltSet[4] <= '1' && battVoltSet[5] <= '2')
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                        else if(battVoltSet[4] >= '1' && battVoltSet[5] >= '5')
                                        {
                                            data_8 = '0';
                                        }
                                        else 
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                    }break;
                                //
                                case(7):
                                    {
                                        if(battVoltSet[4] <= '1' && battVoltSet[5] <= '2' && battVoltSet[6] <= '0') 
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                        else if(battVoltSet[4] >= '1' && battVoltSet[5] >= '5' && battVoltSet[6] >= '0') 
                                        {
                                            data_8 = '0';
                                        }
                                        else
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                    }break;    
                            }//end - switch(poss_x)
                            battVoltSet[poss_x] = data_8;
                        }
                        else //down key pressed 
                        {
                            if(down) 						//case(n) => fl [0][01][2].[3]  eq [4][5][6].[7]
                            {
                                data_8 = battVoltSet[poss_x];
                                switch(poss_x) 
                                {
                                    case(0): 
                                    data_8 = '1';
                                    break;
                                    //
                                    case(1):
                                    {
                                        if(data_8 <= '1') data_8 = '3';
                                        else data_8--;
                                    }
                                    break;
                                    //
                                    case(2): 								//xxN.x	// fl [0][01][2].[3]  eq [4][5][6].[7]
                                    {
                                        if(battVoltSet[0] <= '1' && battVoltSet[1] <= '1')
                                        {
                                            if(data_8 <= '0') data_8 = '9';
                                            else data_8--;
                                        }
                                        else if(battVoltSet[0] >= '1' && battVoltSet[1] >= '3')
                                        {
                                            data_8 = '0';
                                        }
                                        else
                                        {
                                            if(data_8 <= '0') data_8 = '9';
                                            else data_8--;
                                        }
                                    }
                                    break;
                                    //
                                    case(3):
                                    {
                                        if(battVoltSet[0] <= '1' && battVoltSet[1] <= '1' && battVoltSet[2] <= '0')
                                        {
                                            if(data_8 <= '0') data_8 = '9';
                                            else data_8--;
                                        }
                                        else if(battVoltSet[0] >= '1' && battVoltSet[1] >= '3' && battVoltSet[2] >= '0') data_8 = '0';
                                        else
                                        {
                                            if(data_8 <= '0') data_8 = '9';
                                            else data_8--;
                                        }
                                    }break;	
                                    case(4): 
                                    {
                                       data_8 = '1';
                                    }
                                    break;
                                    //
                                    case(5):
                                    {
                                        if(data_8 <= '2') data_8 = '5';
                                        else data_8--;
                                    }
                                    break;
                                    //
                                    case(6): 								//xxN.x	// fl [0][01][2].[3]  eq [4][5][6].[7]
                                    {
                                        if(battVoltSet[4] <= '1' && battVoltSet[5] <= '2') 
                                        {
                                            if(data_8 <= '0') data_8 = '9';
                                            else data_8--;
                                        }
                                        else if(battVoltSet[4] >= '1' && battVoltSet[5] >= '5') 
                                        {
                                            data_8 = '0';
                                        }
                                        else
                                        {
                                            if(data_8 <= '0') data_8 = '9';
                                            else data_8--;
                                        }
                                    }
                                    break;
                                    //
                                    case(7):
                                    {
                                        if(battVoltSet[4] >= '1' && battVoltSet[5] >= '5' && battVoltSet[6] >= '0') data_8 = '0';
                                        else
                                        {
                                            if(data_8 <= '0') data_8 = '9';
                                            else data_8--;
                                        }
                                    }break;	
                                }//end - switch
                                battVoltSet[poss_x]=data_8;
                            }
                            else if(enter) 
                            {
                                if(poss_x >= 7) 
                                {
                                    modi = LO;
                                    CHG_EN = 0;
                                    data_16 = 0;
                                    data_16 +=((battVoltSet[0] & 0x0f) * 1000);
                                    data_16 +=((battVoltSet[1] & 0x0f) * 100);
                                    data_16 +=((battVoltSet[2] & 0x0f) * 10);
                                    data_16 += (battVoltSet[3] & 0x0f);
                                    if(data_16 < (MinOutputVoltage2_PB1*10))
                                    {
                                        battVoltSet[0] = ((MinOutputVoltage2_PB1*10)/ 1000) + 0x30;
                                        battVoltSet[1] = (((MinOutputVoltage2_PB1*10) - ((battVoltSet[0] & 0x0f) * 1000))/100) + 0x30;
                                        battVoltSet[2] = (((MinOutputVoltage2_PB1*10) - ((battVoltSet[0] & 0x0f) * 1000) - ((battVoltSet[1] & 0x0f) * 100))/10)+ 0x30;
                                        battVoltSet[3] = ((MinOutputVoltage2_PB1*10) - ((battVoltSet[0] & 0x0f) * 1000) - ((battVoltSet[1] & 0x0f) * 100) - ((battVoltSet[2] & 0x0f)*10)) + 0x30;
                                    }
                                    else if(data_16 > (MaxOutputVoltage2_PB1*10))
                                    {
                                        battVoltSet[0] = ((MaxOutputVoltage2_PB1*10)/ 1000) + 0x30;
                                        battVoltSet[1] = (((MaxOutputVoltage2_PB1*10) - ((battVoltSet[0] & 0x0f) * 1000))/100) + 0x30;
                                        battVoltSet[2] = (((MaxOutputVoltage2_PB1*10) - ((battVoltSet[0] & 0x0f) * 1000) - ((battVoltSet[1] & 0x0f) * 100))/10)+ 0x30;
                                        battVoltSet[3] = ((MaxOutputVoltage2_PB1*10) - ((battVoltSet[0] & 0x0f) * 1000) - ((battVoltSet[1] & 0x0f) * 100) - ((battVoltSet[2] & 0x0f)*10)) + 0x30;
                                    }   

                                    data_16 = 0;
                                    data_16 +=((battVoltSet[4] & 0x0f) * 1000);
                                    data_16 +=((battVoltSet[5] & 0x0f) * 100);
                                    data_16 +=((battVoltSet[6] & 0x0f) * 10);
                                    data_16 += (battVoltSet[7] & 0x0f);
                                    if(data_16 < (MinOutputVoltage2_PB2*10))
                                    {
                                        battVoltSet[4] = ((MinOutputVoltage2_PB2*10)/ 1000) + 0x30;
                                        battVoltSet[5] = (((MinOutputVoltage2_PB2*10) - ((battVoltSet[4] & 0x0f) * 1000))/100) + 0x30;
                                        battVoltSet[6] = (((MinOutputVoltage2_PB2*10) - ((battVoltSet[4] & 0x0f) * 1000) - ((battVoltSet[5] & 0x0f) * 100))/10)+ 0x30;
                                        battVoltSet[7] = ((MinOutputVoltage2_PB2*10) - ((battVoltSet[4] & 0x0f) * 1000) - ((battVoltSet[5] & 0x0f) * 100) - ((battVoltSet[6] & 0x0f)*10)) + 0x30;
                                    }
                                    else if(data_16 > (MaxOutputVoltage2_PB2*10))
                                    {
                                        battVoltSet[4] = ((MaxOutputVoltage2_PB2*10)/ 1000) + 0x30;
                                        battVoltSet[5] = (((MaxOutputVoltage2_PB2*10) - ((battVoltSet[4] & 0x0f) * 1000))/100) + 0x30;
                                        battVoltSet[6] = (((MaxOutputVoltage2_PB2*10) - ((battVoltSet[4] & 0x0f) * 1000) - ((battVoltSet[5] & 0x0f) * 100))/10)+ 0x30;
                                        battVoltSet[7] = ((MaxOutputVoltage2_PB2*10) - ((battVoltSet[4] & 0x0f) * 1000) - ((battVoltSet[5] & 0x0f) * 100) - ((battVoltSet[6] & 0x0f)*10)) + 0x30;       
                                    } 
                                    for(cnt=0;cnt<8;cnt++) write_EE(BATTVOLT_0+cnt, battVoltSet[cnt]);                            
                                    CHG_SEL = 0;
                                    CHG_CMD();
                                    cal_error = LO;modi_end = HI;Set_OK=1;fEqFl=0; 
                                }
                                else poss_x++;
                                //============================================
                            }
                        }	
                    }
					if(esc) modi = LO;
					key_clr();
				}
			}break;
			//
			case(11):
			{

				if(!modi)
				{
                    if(up)
                    {
                        if(read_EE(BATT_TYPE) == 2) iMenuIndex = 9;
                        else iMenuIndex = 10;
                    }
					else
					{
						if(down) iMenuIndex = 12;
						else if(enter) 
                        {
                            modi = HI; poss_x = 3;
                            CHG_EN = 1;
                            CHG_SEL = 1;
                            CHG_CMD();
						}
					}
					if(esc) iMenuIndex = 5;
                    
					if(fEqFl == 0)  // Initial Start Equalizing Charge
					{
						//fEqFl=0;
                        for(cnt=0;cnt<4;cnt++) 
						{
                            if(CHG_Mode == 0)   // Equalizing Charge
                            {
                                outSetNew[cnt] = read_EE(BATTVOLT_4+cnt);
                            }
                            else if(CHG_Mode == 1)  // Floating Charge
                            {
                                outSetNew[cnt] = read_EE(BATTVOLT_0+cnt);
                            }   
							LoadAmpSet[cnt] = read_EE(LOADAMP_0+cnt);
						}
						data_16 = ((outSetNew[0] & 0x0f)*1000);
						buff_16 = ((outSetNew[1] & 0x0f)*100);
						data_8 = ((outSetNew[2] & 0x0f)*10);
						buff_8 = (outSetNew[3] & 0x0f);			
						hex1_16 = (data_16+buff_16+data_8+buff_8);
					}
					else 
					{
                        if(CHG_EN == 0)
                        {
                            for(cnt=0;cnt<4;cnt++) 
                            {
                                if(CHG_Mode == 0)   // Equalizing Charge
                                {
                                    outSetNew[cnt] = read_EE(BATTVOLT_4+cnt);
                                }
                                else if(CHG_Mode == 1)  // Floating Charge
                                {
                                    outSetNew[cnt] = read_EE(BATTVOLT_0+cnt);
                                } 
                            }
                        }
                        else
                        {
                            
                        }
                        data_16 = ((outSetNew[0] & 0x0f)*1000);
						buff_16 = ((outSetNew[1] & 0x0f)*100);
						data_8 = ((outSetNew[2] & 0x0f)*10);
						buff_8 = (outSetNew[3] & 0x0f);
						hex1_16 = (data_16+buff_16+data_8+buff_8);
					}
                    if(read_EE(BATT_TYPE) == 0)
                    {
                        if((hex1_16 < MinOutputVoltage_PB1 * 10) && (hex1_16 > MaxOutputVoltage_PB * 10)) hex1_16 = RefOutputVoltage_PB * 10;			// default set volt
                    }
                    else if(read_EE(BATT_TYPE) == 1)
                    {
                        if((hex1_16 < MinOutputVoltage2_PB1 * 10) && (hex1_16 > MaxOutputVoltage2_PB2 * 10)) hex1_16 = RefOutputVoltage2_PB * 10;			// default set volt
                    }
                    else if(read_EE(BATT_TYPE) == 2)
                    {
                        if((hex1_16 < MinOutputVoltage_LI * 10) && (hex1_16 > MaxOutputVoltage_LI * 10)) hex1_16 = RefOutputVoltage_LI * 10;			// default set volt
                    }                    
					key_clr();
				}
				else 
                {//when modi is HIGH//
					if(up) 
                    {
						data_8 = LoadAmpSet[poss_x-4];
						switch(poss_x) 
                        {
							case(0):case(1):case(2):case(3): Set_OK=1;break;
							case(4):case(5):case(6): break;
						}						
						switch(poss_x) 
                        {
							case(3): 
                            {	
                                if(read_EE(BATT_TYPE) == 0)
                                {      
                                    // manual volt edit and increment
                                    if(hex1_16 >= MaxOutputVoltage_PB * 10) hex1_16 = MaxOutputVoltage_PB * 10;
                                    else hex1_16 += 1;
                                } 
                                else if(read_EE(BATT_TYPE) == 1)
                                {      
                                    // manual volt edit and increment
                                    if(hex1_16 >= MaxOutputVoltage2_PB2 * 10) hex1_16 = MaxOutputVoltage2_PB2 * 10;
                                    else hex1_16 += 1;
                                } 
                                else if(read_EE(BATT_TYPE) == 2)
                                {
                                    // manual volt edit and increment
                                    if(hex1_16 >= MaxOutputVoltage_LI * 10) hex1_16 = MaxOutputVoltage_LI * 10;
                                    else hex1_16 += 1;
                                }
                                
							}break;
							//
							case(4): 
                            {
                                if(data_8 >= '1') data_8 = '0';
                                else data_8++;
                            }break;
							//
							case(5): 
                            {
							   if(LoadAmpSet[0] >= '1')
								{
									if(data_8 >= '2') data_8 = '0';
									else data_8++;
								}
                               else if(LoadAmpSet[0] <= '0')
								{
									if(data_8 >= '9') data_8 = '8';
                                    else if(data_8 < '8') data_8 = '8';
									else data_8++;
								}
								else 
								{
									if(data_8 >= '9') data_8 = '0';
									else data_8++;
								}
							}
							break;
							//
							case(6): 
                            {
								if(LoadAmpSet[0] >= '1' && LoadAmpSet[1] >= '2')
								{
									data_8 = '0';
								}
								else 
								{
									if(data_8 >= '9') data_8 = '0';
									else data_8++;
								}
							}break;
						}
                        LoadAmpSet[poss_x-4] = data_8;
						data_16 = hex1_16; 
                        thnd = ((data_16/1000)+0x30);
                        data_16 %= 1000; 
                        hund = ((data_16/100)+0x30);
                        data_16 %= 100;	
                        ten = ((data_16/10)+0x30);
                        data_16 %= 10; 
                        one = (data_16+0x30);
						outSetNew[0] = thnd; outSetNew[1] = hund; outSetNew[2] = ten; outSetNew[3] = one;
                        Set_VRef = ((outSetNew[0]-0x30)*100)+((outSetNew[1]-0x30)*10)+((outSetNew[2]-0x30)*1)+((outSetNew[3]-0x30)*0.1);
//                        WDT = HI;
//                        Set_OK=1;
					}
					else if(down)
                    {
							data_8 = LoadAmpSet[poss_x-4];
							switch(poss_x) 
                            {
								case(0):case(1):case(2):case(3): Set_OK=1;break;
								case(4):case(5):case(6): break;
							}
							switch(poss_x) 
                            {
								case(3): 
                                {
                                    if(read_EE(BATT_TYPE) == 0)      
                                    {
                                        if(hex1_16 <= MinOutputVoltage_PB1 * 10) hex1_16 = MinOutputVoltage_PB1 * 10;
                                        else hex1_16-= 1;
                                    }
                                    else if(read_EE(BATT_TYPE) == 1)      
                                    {
                                        if(hex1_16 <= MinOutputVoltage2_PB1 * 10) hex1_16 = MinOutputVoltage2_PB1 * 10;
                                        else hex1_16-= 1;
                                    }
                                    else if(read_EE(BATT_TYPE) == 2)                                    
                                    {
                                        if(hex1_16 <= MinOutputVoltage_LI * 10) hex1_16 = MinOutputVoltage_LI * 10;
                                        else hex1_16-= 1;
                                    }
								}break;
								//
								case(4):
								{
									if(data_8 <= '0') data_8 = '1';
									else data_8--;
								}
								break;
								//	
								case(5): 
								{
									if(LoadAmpSet[0] <= '0')
									{
										if(data_8 <= '8') data_8 = '9';
										else data_8--;
									}
                                    else if(LoadAmpSet[0] >= '1')
									{
										if(data_8 <= '0') data_8 = '2';
                                        else if(data_8 > '2') data_8 = '2';
										else data_8--;
									}
									else 
									{
										if(data_8 <= '0') data_8 = '9';
										else data_8--;
									}
								}
								break;
								//
								case(6):
								{
                                    if(LoadAmpSet[0] >= '1' && LoadAmpSet[1] >= '2')
                                    {
                                        data_8 = '0';
                                    }
                                    else 
                                    {
                                        if(data_8 <= '0') data_8 = '9';
                                        else data_8--;
                                    }
								}
							break;
							}
                            LoadAmpSet[poss_x-4] = data_8;
							data_16 = hex1_16;
							thnd = ((data_16/1000)+0x30);data_16 %= 1000;								
							hund = ((data_16/100)+0x30);data_16 %= 100;								
							ten = ((data_16/10)+0x30);data_16 %= 10;								
							one = (data_16+0x30);
							outSetNew[0] = thnd;outSetNew[1] = hund;outSetNew[2] = ten;outSetNew[3] = one;
							Set_VRef = ((outSetNew[0]-0x30)*100)+((outSetNew[1]-0x30)*10)+((outSetNew[2]-0x30)*1)+((outSetNew[3]-0x30)*0.1);                             
//							WDT = HI;
//                            Set_OK=1;
					}
                    else if(enter) 
                    {
                        //outSetNew[poss_x] = temp_buffer[poss_x];
                        if(poss_x >= 6) 
                        {
                            //outSetNew[poss_x] = temp_buffer[poss_x];
                            modi = LO;modi_end = HI;fEqFl=1;
                            data_16 = hex1_16;
                            thnd = ((data_16/1000)+0x30);data_16 %= 1000;								
                            hund = ((data_16/100)+0x30);data_16 %= 100;								
                            ten = ((data_16/10)+0x30);data_16 %= 10;								
                            one = (data_16+0x30);
                            outSetNew[0] = thnd; outSetNew[1] = hund; outSetNew[2] = ten; outSetNew[3] = one;
                            data_16 = 0;
                            data_16 +=((LoadAmpSet[0] & 0x0f) * 100);
                            data_16 +=((LoadAmpSet[1] & 0x0f) * 10);
                            data_16 += (LoadAmpSet[2] & 0x0f);
                            if(data_16 > OCL) 
                            {
                                LoadAmpSet[0] = (OCL / 100) + 0x30;
                                LoadAmpSet[1] = ((OCL - ((LoadAmpSet[0]-0x30) * 100))/10) + 0x30;
                                LoadAmpSet[2] = (OCL - ((LoadAmpSet[0]-0x30) * 100) - ((LoadAmpSet[1] - 0x30) * 10)) + 0x30;
                            }
                            else if(data_16 < UCL) 
                            {
                                LoadAmpSet[0] = (UCL / 100) + 0x30;
                                LoadAmpSet[1] = ((UCL - ((LoadAmpSet[0]-0x30) * 100))/10) + 0x30;
                                LoadAmpSet[2] = (UCL - ((LoadAmpSet[0]-0x30) * 100) - ((LoadAmpSet[1] - 0x30) * 10)) + 0x30;
                            }

                            for(cnt=0;cnt<4;cnt++) write_EE(LOADAMP_0+cnt, LoadAmpSet[cnt]);
                            Set_VRef = ((outSetNew[0] & 0x0F)*100)+((outSetNew[1] & 0x0F)*10)+((outSetNew[2] & 0x0F)*1)+((outSetNew[3] & 0x0F)*0.1);
                            Set_CL = ((LoadAmpSet[0] & 0x0F)*100)+((LoadAmpSet[1] & 0x0F)*10)+(LoadAmpSet[2] & 0x0F);
                            CHG_Check();
                            WDT = HI;Set_OK=1;
                        }
                        else 
                        {
                            if(poss_x==0) poss_x=3;
                            else poss_x++;
                        }
                    }
					else if(esc) {
						modi = LO;fEqFl=1;
						data_16 = hex1_16;
						thnd = ((data_16/1000)+0x30);data_16 %= 1000;								
						hund = ((data_16/100)+0x30);data_16 %= 100;								
						ten = ((data_16/10)+0x30);data_16 %= 10;								
						one = (data_16+0x30);
						outSetNew[0] = thnd;outSetNew[1] = hund;outSetNew[2] = ten;outSetNew[3] = one;
						Set_VRef = ((outSetNew[0] & 0x0F)*100)+((outSetNew[1] & 0x0F)*10)+((outSetNew[2] & 0x0F)*1)+((outSetNew[3] & 0x0F)*0.1);
                        Set_CL = ((LoadAmpSet[0] & 0x0F)*100)+((LoadAmpSet[1] & 0x0F)*10)+(LoadAmpSet[2] & 0x0F);
                        CHG_Check();
						WDT = HI;Set_OK=1;
					}
					key_clr();
				}
			}break;
			//
			case(12): {
				if(!modi)
				{
					if(up)iMenuIndex = 11;
					else 
					{
						if(down) iMenuIndex = 13;
						else if(enter)
						{
							modi = HI;
							poss_x = poss_y = 1;
							for(cnt=0;cnt<4;cnt++)
							{
								battAmpSet[cnt] = read_EE(BATTAMP_0+cnt);
							}
						}
					}
					if(esc) iMenuIndex = 5;
					key_clr();
				}
				else
				{
					if(up) 
					{
						data_8 = battAmpSet[poss_x];
						switch(poss_x) 	
						{
							case(0):
								data_8 = '0';
								break;
								//
							case(1):
								{
									if(data_8 >= '4') data_8= '0';
									else data_8++;
								}break;
							//
							case(2):
								{
                                    if(battAmpSet[1] >= '4')
									{
										data_8 = '0';
									}
									else
									{
										if(data_8 >= '9') data_8 = '0';
										else data_8++;
									}
								}break;
							//
							case(3):
								data_8 = '0';
								break;
						}//end - switch(poss_x)
						battAmpSet[poss_x] = data_8;
					}
					else //down key pressed 
					{
						if(down)
						{
							data_8 = battAmpSet[poss_x];
							switch(poss_x) 
							{
								case(0): 
									data_8 = '0';
									break;
								//
								case(1):
									{
										if(data_8 <= '0') data_8 = '4';
										else data_8--;
									}break;
								//
								case(2):
									{
										if(battAmpSet[1] >= '4' )
										{
											data_8 = '0';
										}
										else
										{
											if(data_8 <= '0') data_8 = '9';
											else data_8--;
										}
									}break;
								//
								case(3):
									data_8 = '0';
									break;
							}//end - switch
							battAmpSet[poss_x] = data_8;
						}
						else if(enter) 
						{
							if(poss_x >= 2)
							{
								modi = LO;
                                data_16 = 0;
                                data_16 += (battAmpSet[0] - 0x30) * 1000;
                                data_16 += (battAmpSet[1] - 0x30) * 100;
                                data_16 += (battAmpSet[2] - 0x30) * 10;
                                data_16 += (battAmpSet[3] - 0x30) * 1;
                                if(data_16 > (MaxBattAmp * 10))
                                {
                                    battAmpSet[0] = ((MaxBattAmp*10) / 1000) + 0x30;
                                    battAmpSet[1] = (((MaxBattAmp*10) - ((battAmpSet[0]-0x30)*1000)) / 100) + 0x30;
                                    battAmpSet[2] = (((MaxBattAmp*10) - ((battAmpSet[0]-0x30)*1000) - ((battAmpSet[1]-0x30)*100)) / 10) + 0x30;
                                    battAmpSet[3] = ((MaxBattAmp*10) - ((battAmpSet[0]-0x30)*1000) - ((battAmpSet[1]-0x30)*100) - ((battAmpSet[2]-0x30)*10)) + 0x30;
                                }
                                else if(data_16 < (MinBattAmp * 10))
                                {
                                     battAmpSet[0] = ((MinBattAmp*10) / 1000) + 0x30;
                                    battAmpSet[1] = (((MinBattAmp*10) - ((battAmpSet[0]-0x30)*1000)) / 100) + 0x30;
                                    battAmpSet[2] = (((MinBattAmp*10) - ((battAmpSet[0]-0x30)*1000) - ((battAmpSet[1]-0x30)*100)) / 10) + 0x30;
                                    battAmpSet[3] = ((MinBattAmp*10) - ((battAmpSet[0]-0x30)*1000) - ((battAmpSet[1]-0x30)*100) - ((battAmpSet[2]-0x30)*10)) + 0x30;
                                }
								for(cnt=0;cnt<4;cnt++) write_EE(BATTAMP_0+cnt, battAmpSet[cnt]);
								Set_BattAmp=((battAmpSet[0] & 0x0F)*100)+((battAmpSet[1] & 0x0F)*10)+((battAmpSet[2] & 0x0F)*1)+((battAmpSet[3] & 0x0F)*0.1);
								cal_error = LO;modi_end = HI;Set_OK=1;
							}
							else poss_x++;
							//============================================
						}
					}					
					if(esc) modi = LO;
					key_clr();
				}
			}break;
            
			//Input Voltage Alram Set
			case(13): 
            {
                if(read_EE(INPUT_VOLT) == 0)
                {
                    if(!modi)
                    {
                        if(up) iMenuIndex = 12;
                        else 
                        {
                            if(down) iMenuIndex = 14;
                            else if(enter) 
                            {
                                modi = HI;
                                poss_x = poss_y = 0;
                                for(cnt=0;cnt<8;cnt++)
                                {
                                    ginValarm[cnt] = read_EE(INVALARM_0+cnt);
                                }
                            }
                        }
                        if(esc) iMenuIndex = 5;
                        key_clr();
                    }
                    else
                    {
                        if(up) 
                        {
                            data_8 = ginValarm[poss_x];
                            switch(poss_x) 	
                            {
                                case(0):
                                {
                                    data_8 = '4';
                                }
                                    break;
                                    //
                                case(1):
                                    {
                                        if(ginValarm[0] >= '4')
                                        {	 
                                            if(data_8>= '1') data_8 = '0';
                                            else data_8++;
                                        }	
                                        else if(ginValarm[0] <= '3')
                                        {
                                            if(data_8>= '9') data_8 = '8';
                                            else data_8++;
                                        }
                                        else
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                    }break;
                                //
                                case(2):
                                    {
                                        if(ginValarm[0] >= '4' && ginValarm[1] >= '1')
                                        {
                                            if(data_8 >= '8') data_8 = '0';
                                            else data_8++;
                                        }
                                        else if(ginValarm[0] <= '3' && ginValarm[1] <= '8')
                                        {
                                            if(data_8 >= '9') data_8 = '1';
                                            else data_8++;
                                        }
                                        else
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                    }break;
                                //
                                case(3):
                                    {
                                        if(ginValarm[0] >= '4' && ginValarm[1] >= '1' && ginValarm[2] >= '8')
                                        {
                                            data_8 = '0';
                                        }
                                        else
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                    }break;
                                //
                                case(4):
                                    {
                                        data_8 = '3';
                                    }break;
                                //
                                case(5): 				
                                    {
                                        if(data_8>= '7') data_8 = '4';
                                        else data_8++;                                    
                                    }break;
                                //
                                case(6): 
                                    {
                                        if(ginValarm[5] <= '4')
                                        {
                                            if(data_8 >= '9') data_8 = '2';
                                            else data_8++;
                                        }
                                        else
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                    }break;
                                //
                                case(7): 
                                    {
                                        if(ginValarm[4] >= '3' && ginValarm[5] >= '7' && ginValarm[6] >= '9') 
                                        {
                                            data_8 = '0';
                                        }
                                        else
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }		
                                    }break;
                            }//end - switch(poss_x)
                            ginValarm[poss_x]=data_8;
                        }
                        else //down key pressed 
                        {
                            if(down)
                            {
                                data_8 = ginValarm[poss_x];
                                switch(poss_x) 
                                {
                                    case(0): 
                                        data_8 = '3';
                                        break;
                                    //
                                    case(1):
                                        {
                                            if(ginValarm[0] <= '3')
                                            {
                                                if(data_8 <= '8') data_8 = '9';
                                                else data_8--;
                                            }
                                            else if(ginValarm[0] >= '4')
                                            {
                                                if(data_8 <= '0') data_8 = '1';
                                                else data_8--;
                                            }
                                        }break;
                                    //
                                    case(2):
                                        {
                                            if(ginValarm[0] <= '3' && ginValarm[1] <= '8') 
                                            {
                                                if(data_8 <= '1') data_8 = '9';
                                                else data_8--;
                                            }
                                            else if(ginValarm[0] >= '4' && ginValarm[1] >= '1') 
                                            {
                                                if(data_8 <= '0') data_8 = '8';
                                                else data_8--;
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }											
                                        }break;
                                    //
                                    case(3):
                                    {
                                        if(ginValarm[0] >= '4' && ginValarm[1] >= '1' && ginValarm[2] >= '8') 
                                        {
                                            data_8 = '0';
                                        }
                                        else
                                        {
                                            if(data_8 <= '0') data_8 = '9';
                                            else data_8--;
                                        }
                                    }break;
                                    //
                                    case(4):
                                        data_8 = '3';
                                        break;
                                    //	
                                    case(5): 
                                        {
                                            if(data_8 <= '4') data_8 = '7';
                                            else data_8--;
                                        }break;
                                    //
                                    case(6):
                                        {
                                            if(ginValarm[5] <= '4')
                                            {
                                                if(data_8 <= '2') data_8 = '9';
                                                else data_8--;
                                            }
                                            else if(ginValarm[5] >= '8') data_8 = '0';
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                        }break;
                                    //
                                    case(7):
                                    {
                                        if(ginValarm[4] >= '3' && ginValarm[5] >= '7' && ginValarm[6] >= '9') 
                                        {
                                            data_8 = '0';
                                        }
                                        else
                                        {
                                            if(data_8 <= '0') data_8 = '9';
                                            else data_8--;
                                        }
                                    }break;									
                                }//end - switch
                                ginValarm[poss_x] = data_8;
                            }
                            else if(enter) 
                            {
                                if(poss_x >= 7)
                                {
                                    modi = LO;
                                    data_16 = (((ginValarm[0] & 0x0F)*1000)+((ginValarm[1] & 0x0F)*100)+((ginValarm[2] & 0x0F)*10)+(ginValarm[3] & 0x0F));

                                    if(data_16 > (Input_OV_Ref*10))
                                    {
                                        ginValarm[0] = ((MaxInputVoltage_380*10) / 1000) + 0x30;
                                        ginValarm[1] = (((MaxInputVoltage_380*10) - ((ginValarm[0] - 0x30) * 1000)) / 100) + 0x30;
                                        ginValarm[2] = (((MaxInputVoltage_380*10) - ((ginValarm[0] - 0x30) * 1000) - ((ginValarm[1] - 0x30) * 100))/10) + 0x30;
                                        ginValarm[3] = ((MaxInputVoltage_380*10) - ((ginValarm[0] - 0x30) * 1000) - ((ginValarm[1] - 0x30) * 100) - ((ginValarm[2] - 0x30) * 10)) + 0x30;                                       
                                    }
                                    else if(data_16 < ((Input_V_Ref+1)*10))
                                    {
                                        ginValarm[0] = (((RefInputVoltage_380+1)*10) / 1000) + 0x30;
                                        ginValarm[1] = ((((RefInputVoltage_380+1)*10) - ((ginValarm[0] - 0x30) * 1000)) / 100) + 0x30;
                                        ginValarm[2] = ((((RefInputVoltage_380+1)*10) - ((ginValarm[0] - 0x30) * 1000) - ((ginValarm[1] - 0x30) * 100))/10) + 0x30;
                                        ginValarm[3] = (((RefInputVoltage_380+1)*10) - ((ginValarm[0] - 0x30) * 1000) - ((ginValarm[1] - 0x30) * 100) - ((ginValarm[2] - 0x30) * 10)) + 0x30; 
                                    }

                                    data_16=(((ginValarm[4] & 0x0F)*1000)+((ginValarm[5] & 0x0F)*100)+((ginValarm[6] & 0x0F)*10)+(ginValarm[7] & 0x0F));
                                    if(data_16 < Input_UV_Ref*10)
                                    {
                                        ginValarm[4] = ((MinInputVoltage_380*10) / 1000) + 0x30;
                                        ginValarm[5] = (((MinInputVoltage_380*10) - ((ginValarm[4] - 0x30) * 1000)) / 100) + 0x30;
                                        ginValarm[6] = (((MinInputVoltage_380*10) - ((ginValarm[4] - 0x30) * 1000) - ((ginValarm[5] - 0x30) * 100))/10) + 0x30;
                                        ginValarm[7] = ((MinInputVoltage_380*10) - ((ginValarm[4] - 0x30) * 1000) - ((ginValarm[5] - 0x30) * 100) - ((ginValarm[6] - 0x30) * 10)) + 0x30;
                                    }
                                    else if(data_16 > ((Input_V_Ref-1)*10))
                                    {
                                        ginValarm[4] = (((RefInputVoltage_380-1)*10) / 1000) + 0x30;
                                        ginValarm[5] = ((((RefInputVoltage_380-1)*10) - ((ginValarm[4] - 0x30) * 1000)) / 100) + 0x30;
                                        ginValarm[6] = ((((RefInputVoltage_380-1)*10) - ((ginValarm[4] - 0x30) * 1000) - ((ginValarm[5] - 0x30) * 100))/10) + 0x30;
                                        ginValarm[7] = (((RefInputVoltage_380-1)*10) - ((ginValarm[4] - 0x30) * 1000) - ((ginValarm[5] - 0x30) * 100) - ((ginValarm[6] - 0x30) * 10)) + 0x30; 
                                    }
                                    for(cnt=0;cnt<8;cnt++) write_EE(INVALARM_0+cnt, ginValarm[cnt]);
                                    Set_Input_OV=((ginValarm[0] & 0x0F)*100) + ((ginValarm[1] & 0x0F)*10)+ ((ginValarm[2] & 0x0F)*1) + ((ginValarm[3] & 0x0F)*0.1);
                                    Set_Input_UV=((ginValarm[4] & 0x0F)*100)+((ginValarm[5] & 0x0F)*10)+ ((ginValarm[6] & 0x0F)*1) + ((ginValarm[7] & 0x0F)*0.1);
                                    cal_error = LO;modi_end = HI;Set_OK=1;
                                }
                                else poss_x++;
                            }
                        }					
                        if(esc) modi = LO;
                        key_clr();
                    }
                }
                else                    
                {
                    if(!modi)
                    {
                        if(up) iMenuIndex = 12;
                        else 
                        {
                            if(down) iMenuIndex = 14;
                            else if(enter) 
                            {
                                modi = HI;
                                poss_x = poss_y = 0;
                                for(cnt=0;cnt<8;cnt++)
                                {
                                    ginValarm[cnt] = read_EE(INVALARM_0+cnt);
                                }
                            }
                        }
                        if(esc) iMenuIndex = 5;
                        key_clr();
                    }
                    else
                    {
                        if(up) 
                        {
                            data_8 = ginValarm[poss_x];
                            switch(poss_x)
                            {
                                case(0):
                                    {
                                        data_8 = '2';
                                    }
                                    break;
                                    //
                                case(1):
                                    {
                                        if(data_8>= '4') data_8 = '2';
                                        else data_8++;
                                    }break;
                                //
                                case(2):
                                    {
                                        if(ginValarm[1] == '4')
                                         {
                                            if(data_8 >= '2') data_8 = '0';
                                            else data_8++;
                                         }
                                         else if(ginValarm[1] == '2')
                                         {
                                            if(data_8 >= '9') data_8 = '1';
                                            else data_8++;
                                         }
                                         else 
                                         {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                         }
                                    }break;
                                //
                                case(3):
                                    {
                                        if(ginValarm[0] == '2' && ginValarm[1] == '4' && ginValarm[2] == '2')
                                        {
                                            data_8 = '0';
                                        }
                                        else 
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                    }break;
                                //
                                case(4):
                                    {
                                        if(data_8 >= '2') data_8 = '1';
                                        else data_8++;
                                    }break;
                                //
                                case(5): 				
                                    {
                                        if(ginValarm[4] == '1')
                                        {
                                            data_8 = '9';
                                        }
                                        else if(ginValarm[4] == '2')
                                        {
                                            if(data_8 >= '1') data_8 = '0';
                                            else data_8++;
                                        }
                                        else 
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                    }break;
                                //
                                case(6): 
                                    {
                                        if(ginValarm[4] == '1' && ginValarm[5] == '9')
                                        {
                                            if(data_8 >= '9') data_8 = '8';
                                            else data_8++;
                                        }
                                        else
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                    }break;
                                //
                                case(7): 
                                    {
                                        if(ginValarm[4] == '2' && ginValarm[5] == '1' && ginValarm[6] == '9')
                                        {
                                            data_8 = '0';
                                        }	
                                        else
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                    }break;
                            }//end - switch(poss_x)
                            ginValarm[poss_x]=data_8;
                        }
                        else //down key pressed 
                        {
                            if(down)
                            {
                                data_8 = ginValarm[poss_x];
                                switch(poss_x) 
                                {
                                    case(0): 
                                    {
                                        data_8 = '2';
                                    }break;
                                    //
                                    case(1):
                                        {
                                            if(data_8 <= '2') data_8 = '4';
                                            else data_8--;
                                        }break;
                                    //
                                    case(2):
                                        {
                                            if(ginValarm[1] == '2') 
                                            {
                                                if(data_8 <= '1') data_8 = '9';
                                                else data_8--;
                                            }  
                                            else if(ginValarm[1] == '4')
                                            {
                                                if(data_8 <= '0') data_8 = '2';
                                                else data_8--;
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }											
                                        }break;
                                    //
                                    case(3):
                                        {
                                            if(ginValarm[0] == '2' && ginValarm[1] == '4' && ginValarm[2] == '2') 
                                            {
                                                data_8 = '0';
                                            }  
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                    }
                                        break;
                                    //
                                    case(4):
                                        {
                                            if(data_8 <= '1') data_8 = '2';
                                            else data_8--;
                                    }
                                        break;
                                    //	
                                    case(5): 
                                        {
                                            if(ginValarm[4] == '1') 
                                            {
                                                data_8 = '9';
                                            }
                                            else if(ginValarm[4] == '2')
                                            {
                                                if(data_8 <= '0') data_8 = '1';
                                                else data_8--;
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                        }break;
                                    //
                                    case(6):
                                        {
                                            if(ginValarm[4] == '1' && ginValarm[5] == '9')
                                            {
                                                if(data_8 <= '8') data_8 = '9';
                                                else data_8--;
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                        }break;
                                    //
                                    case(7):
                                        {
                                            if(ginValarm[4] == '2' && ginValarm[5] == '1' && ginValarm[6] == '9')
                                            {
                                                data_8 = '0';                                                
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                    }
                                        break;									
                                }//end - switch
                                ginValarm[poss_x] = data_8;
                            }
                            else if(enter) 
                            {
                                if(poss_x >= 7)
                                {
                                    modi = LO;
                                    data_16=(((ginValarm[0] & 0x0F)*1000)+((ginValarm[1] & 0x0F)*100)+((ginValarm[2] & 0x0F)*10)+(ginValarm[3] & 0x0F));

                                    if(data_16 > (Input_OV_Ref * 10))
                                    {
                                        ginValarm[0] = ((MaxInputVoltage_220*10) / 1000) + 0x30;
                                        ginValarm[1] = (((MaxInputVoltage_220*10) - ((ginValarm[0] - 0x30) * 1000)) / 100) + 0x30;
                                        ginValarm[2] = (((MaxInputVoltage_220*10) - ((ginValarm[0] - 0x30) * 1000) - ((ginValarm[1] - 0x30) * 100))/10) + 0x30;
                                        ginValarm[3] = ((MaxInputVoltage_220*10) - ((ginValarm[0] - 0x30) * 1000) - ((ginValarm[1] - 0x30) * 100) - ((ginValarm[2] - 0x30) * 10)) + 0x30;
                                    }
                                    else if(data_16 < ((Input_V_Ref + 1)*10))
                                    {
                                        ginValarm[0] = (((RefInputVoltage_220+1)*10) / 1000) + 0x30;
                                        ginValarm[1] = ((((RefInputVoltage_220+1)*10) - ((ginValarm[0] - 0x30) * 1000)) / 100) + 0x30;
                                        ginValarm[2] = ((((RefInputVoltage_220+1)*10) - ((ginValarm[0] - 0x30) * 1000) - ((ginValarm[1] - 0x30) * 100))/10) + 0x30;
                                        ginValarm[3] = (((RefInputVoltage_220+1)*10) - ((ginValarm[0] - 0x30) * 1000) - ((ginValarm[1] - 0x30) * 100) - ((ginValarm[2] - 0x30) * 10)) + 0x30;
                                    }

                                    data_16=(((ginValarm[4] & 0x0F)*1000)+((ginValarm[5] & 0x0F)*100)+((ginValarm[6] & 0x0F)*10)+(ginValarm[7] & 0x0F));
                                    if(data_16 < (Input_UV_Ref * 10))
                                    {
                                        ginValarm[4] = ((MinInputVoltage_220*10) / 1000) + 0x30;
                                        ginValarm[5] = (((MinInputVoltage_220*10) - ((ginValarm[4] - 0x30) * 1000)) / 100) + 0x30;
                                        ginValarm[6] = (((MinInputVoltage_220*10) - ((ginValarm[4] - 0x30) * 1000) - ((ginValarm[5] - 0x30) * 100))/10) + 0x30;
                                        ginValarm[7] = ((MinInputVoltage_220*10) - ((ginValarm[4] - 0x30) * 1000) - ((ginValarm[5] - 0x30) * 100) - ((ginValarm[6] - 0x30) * 10)) + 0x30;
                                    }
                                    else if(data_16 > ((Input_V_Ref - 1)*10))
                                    {
                                        ginValarm[4] = (((RefInputVoltage_220-1)*10) / 1000) + 0x30;
                                        ginValarm[5] = ((((RefInputVoltage_220-1)*10) - ((ginValarm[4] - 0x30) * 1000)) / 100) + 0x30;
                                        ginValarm[6] = ((((RefInputVoltage_220-1)*10) - ((ginValarm[4] - 0x30) * 1000) - ((ginValarm[5] - 0x30) * 100))/10) + 0x30;
                                        ginValarm[7] = (((RefInputVoltage_220-1)*10) - ((ginValarm[4] - 0x30) * 1000) - ((ginValarm[5] - 0x30) * 100) - ((ginValarm[6] - 0x30) * 10)) + 0x30;
                                    }

                                    for(cnt=0;cnt<8;cnt++) write_EE(INVALARM_0+cnt, ginValarm[cnt]);
                                    Set_Input_OV=((ginValarm[0] & 0x0F)*100) + ((ginValarm[1] & 0x0F)*10)+ (ginValarm[2] & 0x0F) + ((ginValarm[3] & 0x0F)*0.1);
                                    Set_Input_UV=((ginValarm[4] & 0x0F)*100)+((ginValarm[5] & 0x0F)*10)+ (ginValarm[6] & 0x0F) + ((ginValarm[7] & 0x0F)*0.1);
                                    cal_error = LO;modi_end = HI;Set_OK=1;
                                }
                                else poss_x++;
                            }
                        }					
                        if(esc) modi = LO;
                        key_clr();
                    }
                }
                
			}break;
            
			//
			case(14): {
				if(!modi)
				{
					if(up)iMenuIndex = 13;
					else 
					{
						if(down) iMenuIndex = 15;
						else if(enter) 
						{
							modi = HI;
							poss_x = poss_y = 0;
							for(cnt=0;cnt<8;cnt++)
							{
								goutValarm[cnt] = read_EE(OUTVALARM_0+cnt);
							}
						}
					}
					if(esc) iMenuIndex = 5;
					key_clr();
				}
				else
				{
                    if(read_EE(BATT_TYPE) == 0)
                    {
                        if(up) 
                        {
                            data_8 = goutValarm[poss_x];
                            switch(poss_x) 	
                            {
                                case(0):
                                {
                                    data_8 = '1';
                                }
                                break;
                                //
                                case(1):
                                {
                                    if (goutValarm[0] >= '1')
                                    {
                                        if (data_8 >= '5') data_8 = '2';
                                        else if (data_8 < '2') data_8 = '5';
                                        else data_8++;
                                    }
                                    else
                                    {
                                        if (data_8 >= '9') data_8 = '0';
                                        else data_8++;
                                    }
                                }break;
                                //
                                case(2):
                                {
                                    if (goutValarm[0] <= '1' && goutValarm[1] <= '2')
                                    {
                                        if (data_8 >= '9') data_8 = '7';
                                        else if (data_8 < '7') data_8 = '9';
                                        else data_8++;
                                    }                                    
                                    else if (goutValarm[0] >= '1' && goutValarm[1] >= '5')
                                    {
                                        data_8 = '0';
                                    }
                                    else
                                    {
                                        if (data_8 >= '9') data_8 = '0';
                                        else data_8++;
                                    }
                                }break;
                                //
                                case(3):
                                {
                                    if (goutValarm[0] <= '1' && goutValarm[1] <= '2' && goutValarm[2] <= '7')
                                    {
                                        if (data_8 >= '9') data_8 = '6';
                                        else if (data_8 < '6') data_8 = '9';
                                        else data_8++;
                                    }
                                    else if (goutValarm[0] >= '1' && goutValarm[1] >= '5' && goutValarm[2] >= '0')
                                    {
                                        data_8 = '0';
                                    }
                                    else
                                    {
                                        if (data_8 >= '9') data_8 = '0';
                                        else data_8++;
                                    }
                                }break;
                                //
                                case(4):
                                {
                                    if (data_8 >= '1') data_8 = '0';
                                    else data_8++;
                                }break;
                                //
                                case(5):
                                {
                                    if(goutValarm[4] <= '0')
                                    {
                                        data_8 = '9';
                                    }
                                    else if(goutValarm[4] >= '1')
                                    {
                                        if (data_8 >= '3') data_8 = '0';
                                        else if (data_8 < '0') data_8 = '3';
                                        else data_8++;
                                    }
                                    else
                                    {
                                        if(data_8 >= '9') data_8 = '0';
                                        else data_8++;
                                    }
                                }break;
                                //
                                case(6):
                                {
                                    if(goutValarm[4] <= '0' && goutValarm[5] <= '9')
                                    {
                                        if(data_8 >= '9') data_8 = '8';
                                        else if(data_8 < '8') data_8 = '9';
                                        else data_8++;
                                    }
                                    else if(goutValarm[4] >= '1' && goutValarm[5] >= '3')
                                    {
                                        if(data_8 >= '2') data_8 = '0';
                                        else if (data_8 < '0') data_8 = '2';
                                        else data_8++;
                                    }
                                    else
                                    {
                                        if(data_8 >= '9') data_8 = '0';
                                        else data_8++;
                                    }
                                }break;
                                //
                                case(7):
                                {
                                    if (goutValarm[4] <= '0' && goutValarm[5] <= '9' && goutValarm[6] <= '8')
                                    {
                                        if (data_8 >= '9') data_8 = '6';
                                        else if (data_8 < '6') data_8 = '9';
                                        else data_8++;
                                    }
                                    else if (goutValarm[4] >= '1' && goutValarm[5] >= '3' && goutValarm[6] >= '2')
                                    {
                                        data_8 = '0';
                                    }
                                    else
                                    {
                                        if (data_8 >= '9') data_8 = '0';
                                        else data_8++;
                                    }
                                }break;
                            }//end - switch(poss_x)
                            goutValarm[poss_x] = data_8;
                        }
                        else //down key pressed 
                        {
                            if(down)
                            {
                                data_8 = goutValarm[poss_x];
                                switch(poss_x) 
                                {
                                    case(0): 
                                        data_8 = '1';
                                        break;
                                    //
                                    case(1):
                                        {
                                            if(goutValarm[0] >= '1')
                                            {
                                                if(data_8 <= '2') data_8= '5';
                                                else if (data_8 > '5') data_8 = '2';
                                                else data_8--;
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8= '9';
                                                else data_8--;
                                            }
                                        }break;
                                    //
                                    case(2):
                                        {
                                            if(goutValarm[0] <= '1' && goutValarm[1] <= '2')
                                            {
                                                if(data_8 <= '7') data_8= '9';
                                                else if (data_8 > '9') data_8 = '7';
                                                else data_8--;
                                            }
                                            else if(goutValarm[0] >= '1' && goutValarm[1] >= '5')
                                            {
                                                data_8= '0';
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                        }break;
                                    //
                                    case(3):
                                        {
                                            if(goutValarm[0] <= '1' && goutValarm[1] <= '2' && goutValarm[2] <= '7')
                                            {
                                                if(data_8 <= '6') data_8 = '9';
                                                else if (data_8 > '9') data_8 = '6';
                                                else data_8--;
                                            }	
                                            else if(goutValarm[0] >= '1' && goutValarm[1] >= '5' && goutValarm[2] >= '0')
                                            {
                                                data_8 = '0';
                                            }	
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;									
                                            }
                                        }break;
                                    //
                                    case(4):
                                        {
                                            if(data_8 <= '0') data_8 = '1';
                                            else data_8--;
                                        }                                        
                                        break;
                                //	
                                    case(5):
                                        {
                                            if(goutValarm[4] <= '0')
                                            {
                                                data_8 = '9';
                                            }
                                            else if(goutValarm[4] >= '1')
                                            {
                                                if(data_8 <= '0') data_8 = '3';
                                                else if(data_8 > '3') data_8 = '0';
                                                else data_8--;
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                        }break;
                                    //
                                    case(6):
                                        {
                                            if(goutValarm[4] <= '0' && goutValarm[5] <= '9')
                                            {
                                                if(data_8 <= '8') data_8 = '9';
                                                else if (data_8 > '9') data_8 = '8';
                                                else data_8--;
                                            }
                                            else if(goutValarm[4] >= '1' && goutValarm[5] >= '3')
                                            {
                                                if(data_8 <= '0') data_8 = '2';
                                                else if(data_8 > '2') data_8 = '0';
                                                else data_8--;
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                        }break;
                                    //
                                    case(7):
                                        {
                                            if(goutValarm[4] <= '0' && goutValarm[5] <= '9' && goutValarm[6] <= '8')
                                            {
                                                if(data_8 <= '6') data_8 = '9';
                                                else if (data_8 > '9') data_8 = '6';
                                                else data_8--;
                                            }
                                            else if(goutValarm[4] >= '1' && goutValarm[5] >= '3' && goutValarm[6] >= '2')
                                            {
                                                data_8 = '0';
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                    }break;
                                }//end - switch
                                goutValarm[poss_x] = data_8;
                            }
                            else if(enter) 
                            {
                                if(poss_x >= 7)
                                {
                                    data_16=(((goutValarm[0] & 0x0F)*1000)+((goutValarm[1] & 0x0F)*100)+((goutValarm[2] & 0x0F)*10)+(goutValarm[3] & 0x0F));
                                    if(data_16 > (MaxOutputVoltage_PB * 10))
                                    {
                                        goutValarm[0] = ((MaxOutputVoltage_PB*10) / 1000) + 0x30;
                                        goutValarm[1] = (((MaxOutputVoltage_PB*10) - ((goutValarm[0] & 0x0f) * 1000))/100) + 0x30;
                                        goutValarm[2] = (((MaxOutputVoltage_PB*10) - ((goutValarm[0] & 0x0f) * 1000) - ((goutValarm[1] & 0x0f) * 100))/10) + 0x30;
                                        goutValarm[3] = ((MaxOutputVoltage_PB*10) - ((goutValarm[0] & 0x0f) * 1000) - ((goutValarm[1] & 0x0f) * 100) - ((goutValarm[2] & 0x0f)*10)) + 0x30;  
                                    }
                                    else if(data_16 < (MinOutputVoltage_PB2 * 10))
                                    {
                                        goutValarm[0] = ((MinOutputVoltage_PB2*10) / 1000) + 0x30;
                                        goutValarm[1] = (((MinOutputVoltage_PB2*10) - ((goutValarm[0] & 0x0f) * 1000))/100) + 0x30;
                                        goutValarm[2] = (((MinOutputVoltage_PB2*10) - ((goutValarm[0] & 0x0f) * 1000) - ((goutValarm[1] & 0x0f) * 100))/10) + 0x30;
                                        goutValarm[3] = ((MinOutputVoltage_PB2*10) - ((goutValarm[0] & 0x0f) * 1000) - ((goutValarm[1] & 0x0f) * 100) - ((goutValarm[2] & 0x0f)*10)) + 0x30;  
                                    }

                                    data_16=(((goutValarm[4] & 0x0F)*1000)+((goutValarm[5] & 0x0F)*100)+((goutValarm[6] & 0x0F)*10)+(goutValarm[7] & 0x0F));
                                    if(data_16 < (MinOutputVoltage_PB * 10))
                                    {
                                        goutValarm[4] = ((MinOutputVoltage_PB*10) / 1000) + 0x30;
                                        goutValarm[5] = (((MinOutputVoltage_PB*10) - ((goutValarm[4] & 0x0f) * 1000))/100) + 0x30;
                                        goutValarm[6] = (((MinOutputVoltage_PB*10) - ((goutValarm[4] & 0x0f) * 1000) - ((goutValarm[5] & 0x0f) * 100))/10) + 0x30;
                                        goutValarm[7] = ((MinOutputVoltage_PB*10) - ((goutValarm[4] & 0x0f) * 1000) - ((goutValarm[5] & 0x0f) * 100) - ((goutValarm[6] & 0x0f)*10)) + 0x30;  
                                    }
                                    else if(data_16 > (MidOutputVoltage_PB * 10))
                                    {
                                        goutValarm[4] = ((MidOutputVoltage_PB*10) / 1000) + 0x30;
                                        goutValarm[5] = (((MidOutputVoltage_PB*10) - ((goutValarm[4] & 0x0f) * 1000))/100) + 0x30;
                                        goutValarm[6] = (((MidOutputVoltage_PB*10) - ((goutValarm[4] & 0x0f) * 1000) - ((goutValarm[5] & 0x0f) * 100))/10) + 0x30;
                                        goutValarm[7] = ((MidOutputVoltage_PB*10) - ((goutValarm[4] & 0x0f) * 1000) - ((goutValarm[5] & 0x0f) * 100) - ((goutValarm[6] & 0x0f)*10)) + 0x30;  
                                    }

                                    modi = LO;
                                    for(cnt=0;cnt<8;cnt++) write_EE(OUTVALARM_0+cnt, goutValarm[cnt]);
                                    cal_error = LO;modi_end = HI;
                                    Set_Output_OV=((goutValarm[0]-0x30)*100)+((goutValarm[1]-0x30)*10)+((goutValarm[2]-0x30)*1)+((goutValarm[3]-0x30)*0.1);
                                    Set_Output_UV=((goutValarm[4]-0x30)*100)+((goutValarm[5]-0x30)*10)+((goutValarm[6]-0x30)*1)+((goutValarm[7]-0x30)*0.1);
                                }
                                else poss_x++;
                                //============================================	
                            }
                        }
                    }
                    else if(read_EE(BATT_TYPE) == 1)
                    {
                        if(up) 
                        {
                            data_8 = goutValarm[poss_x];
                            switch(poss_x) 	
                            {
                                case(0):
                                {
                                    data_8 = '1';
                                }
                                break;
                                //
                                case(1):
                                {
                                    if (goutValarm[0] >= '1')
                                    {
                                        if (data_8 >= '5') data_8 = '2';
                                        else if (data_8 < '2') data_8 = '5';
                                        else data_8++;
                                    }
                                    else
                                    {
                                        if (data_8 >= '9') data_8 = '0';
                                        else data_8++;
                                    }
                                }break;
                                //
                                case(2):
                                {
                                    if (goutValarm[0] <= '1' && goutValarm[1] <= '2')
                                    {
                                        if (data_8 >= '9') data_8 = '0';
                                        else if (data_8 < '0') data_8 = '9';
                                        else data_8++;
                                    }                                    
                                    else if (goutValarm[0] >= '1' && goutValarm[1] >= '5')
                                    {
                                        data_8 = '0';
                                    }
                                    else
                                    {
                                        if (data_8 >= '9') data_8 = '0';
                                        else data_8++;
                                    }
                                }break;
                                //
                                case(3):
                                {
                                    if (goutValarm[0] <= '1' && goutValarm[1] <= '2' && goutValarm[2] <= '0')
                                    {
                                        if (data_8 >= '9') data_8 = '0';
                                        else if (data_8 < '0') data_8 = '9';
                                        else data_8++;
                                    }
                                    else if (goutValarm[0] >= '1' && goutValarm[1] >= '5' && goutValarm[2] >= '0')
                                    {
                                        data_8 = '0';
                                    }
                                    else
                                    {
                                        if (data_8 >= '9') data_8 = '0';
                                        else data_8++;
                                    }
                                }break;
                                //
                                case(4):
                                {
                                    data_8 = '1';
                                }break;
                                //
                                case(5):
                                {
                                   if(goutValarm[4] >= '1')
                                    {
                                        if (data_8 >= '2') data_8 = '0';
                                        else if (data_8 < '0') data_8 = '2';
                                        else data_8++;
                                    }
                                    else
                                    {
                                        if(data_8 >= '9') data_8 = '0';
                                        else data_8++;
                                    }
                                }break;
                                //
                                case(6):
                                {
                                    if(goutValarm[4] <= '1' && goutValarm[5] <= '0')
                                    {
                                        if(data_8 >= '9') data_8 = '5';
                                        else if (data_8 < '5') data_8 = '0';
                                        else data_8++;
                                    }
                                    else if(goutValarm[4] >= '1' && goutValarm[5] >= '2')
                                    {
                                        data_8 = '0';
                                    }
                                    else
                                    {
                                        if(data_8 >= '9') data_8 = '0';
                                        else data_8++;
                                    }
                                }break;
                                //
                                case(7):
                                {
                                    if (goutValarm[4] <= '1' && goutValarm[5] <= '0' && goutValarm[6] <= '5')
                                    {
                                        if (data_8 >= '9') data_8 = '0';
                                        else if (data_8 < '0') data_8 = '9';
                                        else data_8++;
                                    }
                                    else if (goutValarm[4] >= '1' && goutValarm[5] >= '2' && goutValarm[6] >= '0')
                                    {
                                        data_8 = '0';
                                    }
                                    else
                                    {
                                        if (data_8 >= '9') data_8 = '0';
                                        else data_8++;
                                    }
                                }break;
                            }//end - switch(poss_x)
                            goutValarm[poss_x] = data_8;
                        }
                        else //down key pressed 
                        {
                            if(down)
                            {
                                data_8 = goutValarm[poss_x];
                                switch(poss_x) 
                                {
                                    case(0): 
                                        data_8 = '1';
                                        break;
                                    //
                                    case(1):
                                        {
                                            if(goutValarm[0] >= '1')
                                            {
                                                if(data_8 <= '2') data_8= '5';
                                                else if (data_8 > '5') data_8 = '2';
                                                else data_8--;
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8= '9';
                                                else data_8--;
                                            }
                                        }break;
                                    //
                                    case(2):
                                        {
                                            if(goutValarm[0] <= '1' && goutValarm[1] <= '2')
                                            {
                                                if(data_8 <= '0') data_8= '9';
                                                else if (data_8 > '9') data_8 = '0';
                                                else data_8--;
                                            }
                                            else if(goutValarm[0] >= '1' && goutValarm[1] >= '5')
                                            {
                                                data_8= '0';
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                        }break;
                                    //
                                    case(3):
                                        {
                                            if(goutValarm[0] <= '1' && goutValarm[1] <= '2' && goutValarm[2] <= '0')
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else if (data_8 > '9') data_8 = '0';
                                                else data_8--;
                                            }	
                                            else if(goutValarm[0] >= '1' && goutValarm[1] >= '5' && goutValarm[2] >= '0')
                                            {
                                                data_8 = '0';
                                            }	
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;									
                                            }
                                        }break;
                                    //
                                    case(4):
                                        {
                                            data_8 = '1';
                                        }                                        
                                        break;
                                //	
                                    case(5):
                                        {
                                            if(goutValarm[4] >= '1')
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                        }break;
                                    //
                                    case(6):
                                        {
                                            if(goutValarm[4] <= '1' && goutValarm[5] <= '0')
                                            {
                                                 if(data_8 <= '5') data_8 = '9';
                                                 else if (data_8 > '9') data_8 = '5';
                                                else data_8--;
                                            }
                                            else if(goutValarm[4] >= '1' && goutValarm[5] >= '2')
                                            {
                                                data_8 = '0';
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                        }break;
                                    //
                                    case(7):
                                        {
                                            if(goutValarm[4] <= '1' && goutValarm[5] <= '0' && goutValarm[6] <= '5')
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                            else if(goutValarm[4] >= '1' && goutValarm[5] >= '2' && goutValarm[6] >= '0')
                                            {
                                                data_8 = '0';
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                    }break;
                                }//end - switch
                                goutValarm[poss_x] = data_8;
                            }
                            else if(enter) 
                            {
                                if(poss_x >= 7)
                                {
                                    data_16=(((goutValarm[0] & 0x0F)*1000)+((goutValarm[1] & 0x0F)*100)+((goutValarm[2] & 0x0F)*10)+(goutValarm[3] & 0x0F));
                                    if(data_16 > (MaxOutputVoltage2_PB * 10))
                                    {
                                        goutValarm[0] = ((MaxOutputVoltage2_PB*10) / 1000) + 0x30;
                                        goutValarm[1] = (((MaxOutputVoltage2_PB*10) - ((goutValarm[0] & 0x0f) * 1000))/100) + 0x30;
                                        goutValarm[2] = (((MaxOutputVoltage2_PB*10) - ((goutValarm[0] & 0x0f) * 1000) - ((goutValarm[1] & 0x0f) * 100))/10) + 0x30;
                                        goutValarm[3] = ((MaxOutputVoltage2_PB*10) - ((goutValarm[0] & 0x0f) * 1000) - ((goutValarm[1] & 0x0f) * 100) - ((goutValarm[2] & 0x0f)*10)) + 0x30;  
                                    }
                                    else if(data_16 < (MinOutputVoltage2_PB2 * 10))
                                    {
                                        goutValarm[0] = ((MinOutputVoltage2_PB2*10) / 1000) + 0x30;
                                        goutValarm[1] = (((MinOutputVoltage2_PB2*10) - ((goutValarm[0] & 0x0f) * 1000))/100) + 0x30;
                                        goutValarm[2] = (((MinOutputVoltage2_PB2*10) - ((goutValarm[0] & 0x0f) * 1000) - ((goutValarm[1] & 0x0f) * 100))/10) + 0x30;
                                        goutValarm[3] = ((MinOutputVoltage2_PB2*10) - ((goutValarm[0] & 0x0f) * 1000) - ((goutValarm[1] & 0x0f) * 100) - ((goutValarm[2] & 0x0f)*10)) + 0x30;  
                                    }

                                    data_16=(((goutValarm[4] & 0x0F)*1000)+((goutValarm[5] & 0x0F)*100)+((goutValarm[6] & 0x0F)*10)+(goutValarm[7] & 0x0F));
                                    if(data_16 < (MinOutputVoltage2_PB * 10))
                                    {
                                        goutValarm[4] = ((MinOutputVoltage2_PB*10) / 1000) + 0x30;
                                        goutValarm[5] = (((MinOutputVoltage2_PB*10) - ((goutValarm[4] & 0x0f) * 1000))/100) + 0x30;
                                        goutValarm[6] = (((MinOutputVoltage2_PB*10) - ((goutValarm[4] & 0x0f) * 1000) - ((goutValarm[5] & 0x0f) * 100))/10) + 0x30;
                                        goutValarm[7] = ((MinOutputVoltage2_PB*10) - ((goutValarm[4] & 0x0f) * 1000) - ((goutValarm[5] & 0x0f) * 100) - ((goutValarm[6] & 0x0f)*10)) + 0x30;  
                                    }
                                    else if(data_16 > (MidOutputVoltage2_PB * 10))
                                    {
                                        goutValarm[4] = ((MidOutputVoltage2_PB*10) / 1000) + 0x30;
                                        goutValarm[5] = (((MidOutputVoltage2_PB*10) - ((goutValarm[4] & 0x0f) * 1000))/100) + 0x30;
                                        goutValarm[6] = (((MidOutputVoltage2_PB*10) - ((goutValarm[4] & 0x0f) * 1000) - ((goutValarm[5] & 0x0f) * 100))/10) + 0x30;
                                        goutValarm[7] = ((MidOutputVoltage2_PB*10) - ((goutValarm[4] & 0x0f) * 1000) - ((goutValarm[5] & 0x0f) * 100) - ((goutValarm[6] & 0x0f)*10)) + 0x30;  
                                    }                                 
                                                                        
                                    modi = LO;
                                    for(cnt=0;cnt<8;cnt++) write_EE(OUTVALARM_0+cnt, goutValarm[cnt]);
                                    cal_error = LO;modi_end = HI;
                                    Set_Output_OV=((goutValarm[0]-0x30)*100)+((goutValarm[1]-0x30)*10)+((goutValarm[2]-0x30)*1)+((goutValarm[3]-0x30)*0.1);
                                    Set_Output_UV=((goutValarm[4]-0x30)*100)+((goutValarm[5]-0x30)*10)+((goutValarm[6]-0x30)*1)+((goutValarm[7]-0x30)*0.1);
                                }
                                else poss_x++;
                                //============================================	
                            }
                        }
                    }
                    
                    else if(read_EE(BATT_TYPE) == 2)
                    {
                        if(up)
                        {
                            data_8 = goutValarm[poss_x];
                            switch(poss_x)
                            {
                                case(0):
                                {
                                    data_8 = '1';
                                }
                                break;
                                //
                                case(1):
                                {
                                    if (data_8 >= '3') data_8 = '1';
                                    else if (data_8 < '1') data_8 = '3';
                                    else data_8++;
                                }break;
                                //
                                case(2):
                                {
                                    if (goutValarm[0] >= '1' && goutValarm[1] >= '3')
                                    {
                                        if (data_8 >= '5') data_8 = '0';
                                        else if (data_8 < '0') data_8 = '5';
                                        else data_8++;
                                    }
                                    else if (goutValarm[0] <= '1' && goutValarm[1] <= '1')
                                    {
                                        if (data_8 >= '9') data_8 = '8';
                                        else if (data_8 < '8') data_8 = '9';
                                        else data_8++;
                                    }
                                    else
                                    {
                                        if (data_8 >= '9') data_8 = '0';
                                        else data_8++;
                                    }
                                }break;
                                //
                                case(3):
                                {
                                    if (goutValarm[0] >= '1' && goutValarm[1] >= '3' && goutValarm[2] >= '5')
                                    {
                                        data_8 = '0';
                                    }
                                    else if (goutValarm[0] <= '1' && goutValarm[1] <= '1' && goutValarm[2] <= '8')
                                    {
                                        if (data_8 >= '9') data_8 = '4';
                                        else if (data_8 < '4') data_8 = '9';
                                        else data_8++;
                                    }
                                    else
                                    {
                                        if (data_8 >= '9') data_8 = '0';
                                        else data_8++;
                                    }
                                }break;
                                //
                                case(4):
                                {
                                    if (data_8 >= '1') data_8 = '0';
                                    else if (data_8 < '0') data_8 = '1';
                                    else data_8++;
                                }break;
                                //
                                case(5):
                                {
                                    if(goutValarm[4] == '0')
                                    {
                                        data_8 = '9';
                                    }
                                    else if(goutValarm[4] >= '1')
                                    {
                                        if (data_8 >= '1') data_8 = '0';
                                        else if (data_8 < '0') data_8 = '1';
                                        else data_8++;
                                    }
                                    else
                                    {
                                        if(data_8 >= '9') data_8 = '0';
                                        else data_8++;
                                    }
                                }break;
                                //
                                case(6):
                                {
                                    if(goutValarm[4] >= '1' && goutValarm[5] >= '1')
                                    {
                                        if (data_8 >= '8') data_8 = '0';
                                        else if (data_8 < '0') data_8 = '8';
                                        else data_8++;
                                    }
                                    else if(goutValarm[4] <= '0' && goutValarm[5] <= '9')
                                    {
                                        if(data_8 >= '9') data_8 = '6';
                                        else if (data_8 < '6') data_8 = '9';
                                        else data_8++;
                                    }
                                    else
                                    {
                                        if(data_8 >= '9') data_8 = '0';
                                        else data_8++;
                                    }
                                }break;
                                //
                                case(7):
                                {
                                    if (goutValarm[4] >= '1' && goutValarm[5] >= '1' && goutValarm[6] >= '8')
                                    {
                                        if (data_8 >= '4') data_8 = '0';
                                        else if (data_8 < '0') data_8 = '4';
                                        else data_8++;
                                    }
                                    else
                                    {
                                        if (data_8 >= '9') data_8 = '0';
                                        else data_8++;
                                    }
                                }break;
                            }//end - switch(poss_x)
                            goutValarm[poss_x] = data_8;
                        }
                        else //down key pressed 
                        {
                            if(down)
                            {
                                data_8 = goutValarm[poss_x];
                                switch(poss_x) 
                                {
                                    case(0): 
                                        data_8= '1';
                                        break;
                                    //
                                    case(1):
                                        {
                                            if(data_8 <= '1') data_8 = '3';
                                            else if (data_8 > '3') data_8 = '1';
                                            else data_8--;
                                        }break;
                                    //
                                    case(2):
                                        {
                                            if(goutValarm[0] <= '1' && goutValarm[1] <= '1')
                                            {
                                                if(data_8 <= '8') data_8= '9';
                                                else if (data_8 > '9') data_8 = '8';
                                                else data_8--;
                                            }
                                            else if(goutValarm[0] >= '1' && goutValarm[1] >= '3')
                                            {
                                                if(data_8 <= '0') data_8= '5';
                                                else if (data_8 > '5') data_8 = '0';
                                                else data_8--;
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                        }break;
                                    //
                                    case(3):
                                        {
                                            if(goutValarm[0] >= '1' && goutValarm[1] >= '3' && goutValarm[2] >= '5')
                                            {
                                                data_8 = '0';
                                            }	
                                            else if(goutValarm[0] <= '1' && goutValarm[1] <= '1' && goutValarm[2] <= '8')
                                            {
                                                if(data_8 <= '4') data_8 = '9';
                                                else if (data_8 > '9') data_8 = '4';
                                                else data_8--;	
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;									
                                            }
                                        }break;
                                    //
                                    case(4):
                                    {
                                        if(data_8 <= '0') data_8 = '1';
                                        else data_8--;
                                    }
                                        break;
                                    //	
                                    case(5):
                                        {
                                            if(goutValarm[4] <= '0')
                                            {
                                                data_8 = '9';
                                            }
                                            else if(goutValarm[4] >= '1')
                                            {
                                                if(data_8 <= '0') data_8 = '1';
                                                else if (data_8 > '1') data_8 = '0';
                                                else data_8--;
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                        }break;
                                    //
                                    case(6):
                                        {
                                            if(goutValarm[4] >= '1' && goutValarm[5] >= '1')
                                            {
                                                if(data_8 <= '0') data_8 = '8';
                                                else if (data_8 > '8') data_8 = '0';
                                                else data_8--;
                                            }
                                            else if(goutValarm[4] <= '0' && goutValarm[5] <= '9')
                                            {
                                                if(data_8 <= '6') data_8 = '9';
                                                else if (data_8 > '9') data_8 = '6';
                                                else data_8--;
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                        }break;
                                    //
                                    case(7):
                                        {
                                            if(goutValarm[4] >= '1' && goutValarm[5] >= '1' && goutValarm[6] >= '8')
                                            {
                                                if(data_8 <= '0') data_8 = '4';
                                                else if (data_8 > '4') data_8 = '0';
                                                else data_8--;
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                    }break;
                                }//end - switch
                                goutValarm[poss_x] = data_8;
                            }
                            else if(enter) 
                            {
                                if(poss_x >= 7)
                                {
                                    data_16=(((goutValarm[0] & 0x0F)*1000)+((goutValarm[1] & 0x0F)*100)+((goutValarm[2] & 0x0F)*10)+(goutValarm[3] & 0x0F));
                                    if(data_16 > (MaxOutputVoltage_LI * 10))
                                    {
                                        goutValarm[0] = ((MaxOutputVoltage_LI*10) / 1000) + 0x30;
                                        goutValarm[1] = (((MaxOutputVoltage_LI*10) - ((goutValarm[0] & 0x0f) * 1000))/100) + 0x30;
                                        goutValarm[2] = (((MaxOutputVoltage_LI*10) - ((goutValarm[0] & 0x0f) * 1000) - ((goutValarm[1] & 0x0f) * 100))/10) + 0x30;
                                        goutValarm[3] = ((MaxOutputVoltage_LI*10) - ((goutValarm[0] & 0x0f) * 1000) - ((goutValarm[1] & 0x0f) * 100) - ((goutValarm[2] & 0x0f)*10)) + 0x30;  
                                    }
                                    else if(data_16 < (MidOutputVoltage_LI * 10))
                                    {
                                        goutValarm[0] = ((MidOutputVoltage_LI*10) / 1000) + 0x30;
                                        goutValarm[1] = (((MidOutputVoltage_LI*10) - ((goutValarm[0] & 0x0f) * 1000))/100) + 0x30;
                                        goutValarm[2] = (((MidOutputVoltage_LI*10) - ((goutValarm[0] & 0x0f) * 1000) - ((goutValarm[1] & 0x0f) * 100))/10) + 0x30;
                                        goutValarm[3] = ((MidOutputVoltage_LI*10) - ((goutValarm[0] & 0x0f) * 1000) - ((goutValarm[1] & 0x0f) * 100) - ((goutValarm[2] & 0x0f)*10)) + 0x30;  
                                    }

                                    data_16=(((goutValarm[4] & 0x0F)*1000)+((goutValarm[5] & 0x0F)*100)+((goutValarm[6] & 0x0F)*10)+(goutValarm[7] & 0x0F));
                                    if(data_16 < (MinOutputVoltage_LI * 10))
                                    {
                                        goutValarm[4] = ((MinOutputVoltage_LI*10) / 1000) + 0x30;
                                        goutValarm[5] = (((MinOutputVoltage_LI*10) - ((goutValarm[4] & 0x0f) * 1000))/100) + 0x30;
                                        goutValarm[6] = (((MinOutputVoltage_LI*10) - ((goutValarm[4] & 0x0f) * 1000) - ((goutValarm[5] & 0x0f) * 100))/10) + 0x30;
                                        goutValarm[7] = ((MinOutputVoltage_LI*10) - ((goutValarm[4] & 0x0f) * 1000) - ((goutValarm[5] & 0x0f) * 100) - ((goutValarm[6] & 0x0f)*10)) + 0x30;  
                                    }
                                    else if(data_16 > (MidOutputVoltage_LI * 10))
                                    {
                                        goutValarm[4] = ((MidOutputVoltage_LI*10) / 1000) + 0x30;
                                        goutValarm[5] = (((MidOutputVoltage_LI*10) - ((goutValarm[4] & 0x0f) * 1000))/100) + 0x30;
                                        goutValarm[6] = (((MidOutputVoltage_LI*10) - ((goutValarm[4] & 0x0f) * 1000) - ((goutValarm[5] & 0x0f) * 100))/10) + 0x30;
                                        goutValarm[7] = ((MidOutputVoltage_LI*10) - ((goutValarm[4] & 0x0f) * 1000) - ((goutValarm[5] & 0x0f) * 100) - ((goutValarm[6] & 0x0f)*10)) + 0x30;  
                                    }

                                    modi = LO;
                                    for(cnt=0;cnt<8;cnt++) write_EE(OUTVALARM_0+cnt, goutValarm[cnt]);
                                    cal_error = LO;modi_end = HI;
                                    Set_Output_OV=((goutValarm[0]-0x30)*100)+((goutValarm[1]-0x30)*10)+((goutValarm[2]-0x30)*1)+((goutValarm[3]-0x30)*0.1);
                                    Set_Output_UV=((goutValarm[4]-0x30)*100)+((goutValarm[5]-0x30)*10)+((goutValarm[6]-0x30)*1)+((goutValarm[7]-0x30)*0.1);
                                }
                                else poss_x++;
                                //============================================	
                            }
                        }
                    }
                    
				
					if(esc) modi = LO;
					key_clr();
				}
			}break;
			//
			case(15): {
				if(!modi)
				{
					if(up)iMenuIndex = 14;
					else 
					{
						if(down) iMenuIndex = 16;
						else if(enter) 
						{
							modi = HI;
							poss_x = poss_y = 0;
							for(cnt=0;cnt<3;cnt++) goutAalarm[cnt] = read_EE(OUTAALARM_0+cnt);//get float volt,Eq. Volt//
						}
					}
					if(esc) iMenuIndex = 5;
					key_clr();
				}
				else
				{
					if(up) 
					{
						data_8 = goutAalarm[poss_x];
						switch(poss_x) 	
						{
							case(0):
							{
								data_8 = '1';
							}break;
							//
							case(1): 				
							{
								if(goutAalarm[0] == '0')
								{
									if(data_8 >= '9' || data_8 < '8') data_8 = '8';
									else data_8++;
								}
								else 
								{
									if(data_8 >= '2') data_8 = '0';
									else data_8++;
								}
							}break;
							//
							case(2): 
							{
								if(goutAalarm[0] >= '1' && goutAalarm[1] <= '1')
								{
									if(data_8 >= '9') data_8 = '0';
									else data_8++;
								}
								else if((goutAalarm[0] <= '0') && (goutAalarm[1] >= '8') && (temp_buffer[2] <= '9'))
								{
									if(data_8 >= '9') data_8 = '0';
									else data_8++;
								}
								else data_8 = '0';
							}break;
							//
						}//end - switch(poss_x)
						goutAalarm[poss_x] = data_8;
					}
					else //down key pressed 
					{
						if(down) 						//case(n) => fl [0][01][2].[3]  eq [4][5][6].[7]
						{
							data_8 = goutAalarm[poss_x];
							switch(poss_x)
							{
								case(0):
									data_8 = '0';
								break;
							//	
								case(1): 
								{
									if(goutAalarm[0] == '0')
									{
										if(data_8 <= '8') data_8 = '9';
										else data_8--;
									}
									else 
									{
										if(data_8 <= '0') data_8 = '2';
										else data_8--;
									}
								}break;
								//
								case(2):
								{
								if(goutAalarm[0] >= '1' && goutAalarm[1] <= '1')
								{
									if(data_8 <= '0') data_8 = '9';
									else data_8--;
								}
								else if((goutAalarm[0] <= '0') && (goutAalarm[1] >= '8') && (goutAalarm[2] <= '9'))
								{
									if(data_8 <= '0') data_8 = '9';
									else data_8--;
								}
								else data_8 = '0';
								}break;
								//
							}//end - switch
							goutAalarm[poss_x] = data_8;
						}
						else if(enter) 
						{
							if(poss_x >= 2)
							{
								modi = LO;
								data_16=((goutAalarm[0]-0x30)*100)+((goutAalarm[1]-0x30)*10)+((goutAalarm[2]-0x30));
								if(data_16 > OCL)
								{
									goutAalarm[0] = (OCL / 100) + 0x30;
									goutAalarm[1] = ((OCL - ((goutAalarm[0]-0x30) * 100)) / 10) + 0x30;
									goutAalarm[2] = (OCL - ((goutAalarm[0]-0x30) * 100) - ((goutAalarm[1]-0x30) * 10)) + 0x30;
								}
								if(data_16 < UCL)
								{
									goutAalarm[0] = (UCL / 100) + 0x30;
									goutAalarm[1] = ((UCL - ((goutAalarm[0]-0x30) * 100)) / 10) + 0x30;
									goutAalarm[2] = (UCL - ((goutAalarm[0]-0x30) * 100) - ((goutAalarm[1]-0x30) * 10)) + 0x30;
								}
								for(cnt=0;cnt<4;cnt++) write_EE(OUTAALARM_0+cnt, goutAalarm[cnt]);
								Set_ARef=(((goutAalarm[0]-0x30)*100)+((goutAalarm[1]-0x30)*10)+((goutAalarm[2]-0x30)));
								Set_OT = 95;
								cal_error = LO;modi_end = HI;Set_OK=1;
							}
							else poss_x++;
							//============================================
							switch(poss_x)
							{
								case 2:
									if(temp_buffer[1]=='2')	temp_buffer[2]='0';
									break;					
							}	
						}
					}					
					if(esc) modi = LO;
					key_clr();
				}
			}break;
			//
			case(16): {//set shutdown volt(Max, Min)
				if(!modi)
				{
					if(up)iMenuIndex = 15;
					else 
					{
						if(down) iMenuIndex = 17;
						else if(enter) 
						{
							modi = HI;
							poss_x = poss_y = 0;
							for(cnt=0;cnt<8;cnt++)
							{
								shutDownOutV[cnt] = read_EE(SHUTDOWNOUTV_0+cnt);
							}
						}
					}
					if(esc) iMenuIndex = 5;
					key_clr();
				}
				else
				{
                    if(read_EE(BATT_TYPE) == 0)//PB125V
                    {
                        if(up) 
                        {
                            data_8 = shutDownOutV[poss_x];

                            switch(poss_x) 	
                            {
                                case(0):        //max 1xx
                                    data_8 = '1';
                                    break;
                                    //
                                case(1):
                                    {
                                        if(data_8 >= '5') data_8= '2';//max 12x~15x
                                        else if (data_8 < '2') data_8 = '5';
                                        else data_8++;
                                    }break;
                                //
                                case(2):
                                    {
                                        if(shutDownOutV[0] <= '1' && shutDownOutV[1] <= '2')
                                        {
                                            if(data_8 >= '9') data_8 = '7';
                                            else if (data_8 < '7') data_8 = '9';
                                            else data_8++;
                                        }
                                        else if(shutDownOutV[0] >= '1' && shutDownOutV[1] >= '5')
                                        {
                                            data_8 = '0';
                                        }
                                        else
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                    }break;
                                //
                                case(3):
                                    {
                                        if(shutDownOutV[0] <= '1' && shutDownOutV[1] <= '2' && shutDownOutV[2] <= '7')
                                        {
                                            if(data_8 >= '9') data_8 = '6';
                                            else if (data_8 < '6') data_8 = '9';
                                            else data_8++;
                                        }
                                        else if(shutDownOutV[0] >= '1' && shutDownOutV[1] >= '5' && shutDownOutV[2] >= '0')
                                        {
                                            data_8 = '0';
                                        }
                                        else
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;									
                                        }
                                        //
                                    }break;
                                //
                                case(4)://min 1xx.x ~ 0xx.x
                                    {
                                        if(data_8 >= '1') data_8 = '0';
                                        else data_8++;
                                    }
                                    break;
                                //
                                case(5)://min x
                                    {
                                        // if(shutDownOutV[4] <= '0')
                                        // {
                                        //     data_8 = '9';
                                        // }
                                        // else if(shutDownOutV[4] >= '1')
                                        // {
                                        //     if(data_8 >= '3') data_8 = '0';//10x.x ~ 13x.x
                                        //     else if(data_8 < '0') data_8 = '3';
                                        //     else data_8++;
                                        // }
                                        // else
                                        // {
                                        //     if(data_8 >= '9') data_8 = '0';
                                        //     else data_8++;									
                                        // }                                        
                                        if(data_8 >= '9') data_8 = '0';
                                        else data_8++;									
                                    }
                                    break;
                                //
                                case(6): 
                                    {
                                        // if(shutDownOutV[4] <= '0' && shutDownOutV[5] <= '9')
                                        // {
                                        //     if(data_8 >= '9') data_8 = '8';
                                        //     else if (data_8 < '8') data_8 = '9';
                                        //     else data_8++;
                                        // }
                                        // else if(shutDownOutV[4] >= '1' && shutDownOutV[5] >= '3')
                                        // {
                                        //     if(data_8 >= '2') data_8 = '0';
                                        //     else if (data_8 < '0') data_8 = '2';
                                        //     else data_8++;
                                        // }
                                        // else
                                        // {
                                        //     if(data_8 >= '9') data_8 = '0';
                                        //     else data_8++;
                                        // }
                                        if(data_8 >= '9') data_8 = '0';
                                        else data_8++;									
                                    }break;
                                //
                                case(7):
                                    {
                                        // if(shutDownOutV[4] <= '0' && shutDownOutV[5] <= '9' && shutDownOutV[6] <= '8')
                                        // {
                                        //     if(data_8 >= '9') data_8 = '6';
                                        //     else if (data_8 < '6') data_8 = '9';
                                        //     else data_8++;
                                        // }
                                        // else if(shutDownOutV[4] >= '1' && shutDownOutV[5] >= '3' && shutDownOutV[6] >= '2')
                                        // {
                                        //     data_8 = '0';
                                        // }
                                        // else
                                        // {
                                        //     if(data_8 >= '9') data_8 = '0';
                                        //     else data_8++;
                                        // }
                                        if(data_8 >= '9') data_8 = '0';
                                        else data_8++;									
                                }break;
                            }//end - switch(poss_x)
                            shutDownOutV[poss_x] = data_8;
                        }
                        else //down key pressed 
                        {
                            if(down)
                            {
                                data_8 = shutDownOutV[poss_x];
                                switch(poss_x) 
                                {
                                    case(0): 
                                        data_8 = '1';
                                        break;
                                    //
                                    case(1):
                                        {
                                            if(data_8 <= '2') data_8 = '5';
                                            else if (data_8 > '5') data_8 = '2';
                                            else data_8--;
                                        }break;
                                    //
                                    case(2):
                                        {
                                            if(shutDownOutV[0] <= '1' && shutDownOutV[1] <= '2')
                                            {
                                                if(data_8 <= '7') data_8 = '9';
                                                else if (data_8 > '9') data_8 = '7';
                                                else data_8--;
                                            }
                                            else if(shutDownOutV[0] >= '1' && shutDownOutV[1] >= '5')
                                            {
                                                data_8 = '0';
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                        }break;
                                    //
                                    case(3):
                                        {
                                            if(shutDownOutV[0] <= '1' && shutDownOutV[1] <= '2' && shutDownOutV[2] <= '7')
                                            {
                                                if(data_8 <= '6') data_8 = '9';
                                                else if (data_8 > '9') data_8 = '6';
                                                else data_8--;			
                                            }		
                                            else if(shutDownOutV[0] >= '1' && shutDownOutV[1] >= '5' && shutDownOutV[2] >= '0')
                                            {
                                                data_8 = '0';
                                            }										
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;									
                                            }
                                        }break;
                                    //
                                    case(4)://min 1xx.x ~ 0xx.x
                                        {
                                            if(data_8 <= '0') data_8 = '1';
                                            else data_8--;
                                        }
                                        break;
                                //	
                                    case(5):
                                        {
                                            // if(shutDownOutV[4] <= '0')
                                            // {                                                
                                            //     data_8 = '9'; //org//
                                            // }
                                            // else if(shutDownOutV[4] >= '1')
                                            // {
                                            //     if(data_8 <= '0') data_8 = '3';
                                            //     else if(data_8 > '3') data_8 = '0';
                                            //     else data_8--;
                                            // }
                                            // else
                                            // {
                                            //     if(data_8 <= '0') data_8 = '9';
                                            //     else data_8--;
                                            // }
                                            if(data_8 <= '0') data_8 = '9';
                                            else data_8--;
                                        }break;
                                    //
                                    case(6):
                                        {
                                            // if(shutDownOutV[4] <= '0' && shutDownOutV[5] <= '9')
                                            // {
                                            //     if(data_8 <= '8') data_8 = '9';
                                            //     else if (data_8 > '9') data_8 = '8';
                                            //     else data_8--;
                                            // }
                                            // else if(shutDownOutV[4] >= '1' && shutDownOutV[5] >= '3')
                                            // {
                                            //     if(data_8 <= '0') data_8 = '2';
                                            //     else if(data_8 > '2') data_8 = '0';
                                            //     else data_8--;
                                            // }                                            
                                            // else
                                            // {
                                            //     if(data_8 <= '0') data_8 = '9';
                                            //     else data_8--;
                                            // }
                                            if(data_8 <= '0') data_8 = '9';
                                            else data_8--;
                                        }break;
                                    //
                                    case(7):
                                        {
                                            // if(shutDownOutV[4] <= '0' && shutDownOutV[5] <= '9' && shutDownOutV[6] <= '8')
                                            // {
                                            //     if(data_8 <= '6') data_8 = '9';
                                            //     else if (data_8 > '9') data_8 = '6';
                                            //     else data_8--;
                                            // }
                                            // else if(shutDownOutV[4] >= '1' && shutDownOutV[5] >= '3' && shutDownOutV[6] >= '2')
                                            // {
                                            //     data_8 = '0';
                                            // }
                                            // else
                                            // {
                                            //     if(data_8 <= '0') data_8 = '9';
                                            //     else data_8--;
                                            // }
                                            if(data_8 <= '0') data_8 = '9';
                                            else data_8--;
                                    }break;
                                }//end - switch
                                shutDownOutV[poss_x] = data_8;
                            }
                            else if(enter) 
                            {
                                if(poss_x >= 7)
                                {
                                    data_16=(((shutDownOutV[0]-0x30)*1000)+((shutDownOutV[1]-0x30)*100)+((shutDownOutV[2]-0x30)*10)+(shutDownOutV[3]-0x30));
                                    if(data_16 > (MaxOutputVoltage_PB * 10))
                                    {
                                        shutDownOutV[0] = ((MaxOutputVoltage_PB*10) / 1000) + 0x30;
                                        shutDownOutV[1] = (((MaxOutputVoltage_PB*10) - ((shutDownOutV[0] - 0x30) * 1000)) / 100) + 0x30;
                                        shutDownOutV[2] = (((MaxOutputVoltage_PB*10) - ((shutDownOutV[0] - 0x30) * 1000) - ((shutDownOutV[1] - 0x30) * 100))/10) + 0x30;
                                        shutDownOutV[3] = ((MaxOutputVoltage_PB*10) - ((shutDownOutV[0] - 0x30) * 1000) - ((shutDownOutV[1] - 0x30) * 100) - ((shutDownOutV[2] - 0x30) * 10)) + 0x30;
                                    }
                                    else if(data_16 < (MinOutputVoltage_PB2 * 10))
                                    {
                                        shutDownOutV[0] = ((MinOutputVoltage_PB2*10) / 1000) + 0x30;
                                        shutDownOutV[1] = (((MinOutputVoltage_PB2*10) - ((shutDownOutV[0] - 0x30) * 1000)) / 100) + 0x30;
                                        shutDownOutV[2] = (((MinOutputVoltage_PB2*10) - ((shutDownOutV[0] - 0x30) * 1000) - ((shutDownOutV[1] - 0x30) * 100))/10) + 0x30;
                                        shutDownOutV[3] = ((MinOutputVoltage_PB2*10) - ((shutDownOutV[0] - 0x30) * 1000) - ((shutDownOutV[1] - 0x30) * 100) - ((shutDownOutV[2] - 0x30) * 10)) + 0x30;
                                    }
                                    
                                    data_16=(((shutDownOutV[4]-0x30)*1000)+((shutDownOutV[5]-0x30)*100)+((shutDownOutV[6]-0x30)*10)+(shutDownOutV[7]-0x30));
                                    if(data_16 < (MinOutputVoltage_PB * 10))
                                    {
                                        shutDownOutV[4] = ((MinOutputVoltage_PB*10) / 1000) + 0x30;
                                        shutDownOutV[5] = (((MinOutputVoltage_PB*10) - ((shutDownOutV[4] - 0x30) * 1000)) / 100) + 0x30;
                                        shutDownOutV[6] = (((MinOutputVoltage_PB*10) - ((shutDownOutV[4] - 0x30) * 1000) - ((shutDownOutV[5] - 0x30) * 100))/10) + 0x30;
                                        shutDownOutV[7] = ((MinOutputVoltage_PB*10) - ((shutDownOutV[4] - 0x30) * 1000) - ((shutDownOutV[5] - 0x30) * 100) - ((shutDownOutV[6] - 0x30) * 10)) + 0x30;
                                    }
                                    else if(data_16 > (MidOutputVoltage_PB * 10))
                                    {
                                        shutDownOutV[4] = ((MidOutputVoltage_PB*10) / 1000) + 0x30;
                                        shutDownOutV[5] = (((MidOutputVoltage_PB*10) - ((shutDownOutV[4] - 0x30) * 1000)) / 100) + 0x30;
                                        shutDownOutV[6] = (((MidOutputVoltage_PB*10) - ((shutDownOutV[4] - 0x30) * 1000) - ((shutDownOutV[5] - 0x30) * 100))/10) + 0x30;
                                        shutDownOutV[7] = ((MidOutputVoltage_PB*10) - ((shutDownOutV[4] - 0x30) * 1000) - ((shutDownOutV[5] - 0x30) * 100) - ((shutDownOutV[6] - 0x30) * 10)) + 0x30;
                                    }
                                    for(cnt=0;cnt<8;cnt++) write_EE(SHUTDOWNOUTV_0+cnt, shutDownOutV[cnt]);
                                    modi = LO;
                                    Set_OV=((shutDownOutV[0]-0x30)*100)+((shutDownOutV[1]-0x30)*10)+((shutDownOutV[2]-0x30)*1)+((shutDownOutV[3]-0x30)*0.1);
                                    Set_UV=((shutDownOutV[4]-0x30)*100)+((shutDownOutV[5]-0x30)*10)+((shutDownOutV[6]-0x30)*1)+((shutDownOutV[7]-0x30)*0.1);
                                    cal_error = LO;modi_end = HI;Set_OK=1;
                                }
                                else poss_x++;
                                //============================================
                            }
                        }
                        if(esc) modi = LO;
                        key_clr();
                    }
                    
                    else if(read_EE(BATT_TYPE) == 1)//pb110v
                    {
                        if(up) 
                        {
                            data_8 = shutDownOutV[poss_x];

                            switch(poss_x) 	
                            {
                                case(0):
                                    data_8 = '1';
                                    break;
                                    //
                                case(1):
                                    {
                                        if(data_8 >= '5') data_8= '2';
                                        else if (data_8 < '2') data_8 = '5';
                                        else data_8++;
                                    }break;
                                //
                                case(2):
                                    {
                                        if(shutDownOutV[0] <= '1' && shutDownOutV[1] <= '2')
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else if (data_8 < '0') data_8 = '9';
                                            else data_8++;
                                        }
                                        else if(shutDownOutV[0] >= '1' && shutDownOutV[1] >= '5')
                                        {
                                            data_8 = '0';
                                        }
                                        else
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                    }break;
                                //
                                case(3):
                                    {
                                        if(shutDownOutV[0] <= '1' && shutDownOutV[1] <= '2' && shutDownOutV[2] <= '0')
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else if (data_8 < '0') data_8 = '9';
                                            else data_8++;
                                        }
                                        else if(shutDownOutV[0] >= '1' && shutDownOutV[1] >= '5' && shutDownOutV[2] >= '0')
                                        {
                                            data_8 = '0';
                                        }
                                        else
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;									
                                        }
                                        //
                                    }break;
                                //
                                case(4): //min 1xx.x ~ 0xx.x
                                    {
                                        if(data_8 == '1')data_8 = '0';
                                        else data_8 = '1';
                                    }
                                    break;
                                //
                                case(5):
                                    {
                                        // if(shutDownOutV[4] >= '1')
                                        // {
                                        //     if(data_8 >= '2') data_8 = '0';
                                        //     else if (data_8 < '0') data_8 = '2';
                                        //     else data_8++;
                                        // }
                                        // else
                                        // {
                                        //     if(data_8 >= '9') data_8 = '0';
                                        //     else data_8++;									
                                        // }                                        
                                        if(data_8 >= '9') data_8 = '0';
                                            else data_8++;									
                                    }
                                    break;
                                //
                                case(6): 
                                    {
                                        // if(shutDownOutV[4] <= '1' && shutDownOutV[5] <= '0')
                                        // {
                                        //     if(data_8 >= '9') data_8 = '5';
                                        //     else if (data_8 < '5') data_8 = '9';
                                        //     else data_8++;
                                        // }
                                        // else if(shutDownOutV[4] >= '1' && shutDownOutV[5] >= '2')
                                        // {
                                        //     data_8 = '0';
                                        // }
                                        // else
                                        // {
                                        //     if(data_8 >= '9') data_8 = '0';
                                        //     else data_8++;
                                        // }
                                        if(data_8 >= '9') data_8 = '0';
                                        else data_8++;									
                                    }break;
                                //
                                case(7):
                                    {
                                        // if(shutDownOutV[4] <= '1' && shutDownOutV[5] <= '0' && shutDownOutV[6] <= '5')
                                        // {
                                        //     if(data_8 >= '9') data_8 = '0';
                                        //     else if (data_8 < '0') data_8 = '9';
                                        //     else data_8++;
                                        // }
                                        // else if(shutDownOutV[4] >= '1' && shutDownOutV[5] >= '2' && shutDownOutV[6] >= '0')
                                        // {
                                        //     data_8 = '0';
                                        // }
                                        // else
                                        // {
                                        //     if(data_8 >= '9') data_8 = '0';
                                        //     else data_8++;
                                        // }
                                        if(data_8 >= '9') data_8 = '0';
                                        else data_8++;									
                                }break;
                            }//end - switch(poss_x)
                            shutDownOutV[poss_x] = data_8;
                        }
                        else //down key pressed 
                        {
                            if(down)
                            {
                                data_8 = shutDownOutV[poss_x];
                                switch(poss_x) 
                                {
                                    case(0): 
                                        data_8 = '1';
                                        break;
                                    //
                                    case(1):
                                        {
                                            if(data_8 <= '2') data_8 = '5';
                                            else if (data_8 > '5') data_8 = '2';
                                            else data_8--;
                                        }break;
                                    //
                                    case(2):
                                        {
                                            if(shutDownOutV[0] <= '1' && shutDownOutV[1] <= '2')
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else if (data_8 > '9') data_8 = '0';
                                                else data_8--;
                                            }
                                            else if(shutDownOutV[0] >= '1' && shutDownOutV[1] >= '5')
                                            {
                                                data_8 = '0';
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                        }break;
                                    //
                                    case(3):
                                        {
                                            if(shutDownOutV[0] <= '1' && shutDownOutV[1] <= '2' && shutDownOutV[2] <= '0')
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else if (data_8 > '9') data_8 = '0';
                                                else data_8--;			
                                            }		
                                            else if(shutDownOutV[0] >= '1' && shutDownOutV[1] >= '5' && shutDownOutV[2] >= '0')
                                            {
                                                data_8 = '0';
                                            }										
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;									
                                            }
                                        }break;
                                    //
                                    case(4):
                                        {
                                            if(data_8 == '1')data_8 = '0';
                                            else data_8 = '1';
                                        }
                                        break;
                                //	
                                    case(5):
                                        {
                                            // if(shutDownOutV[4] >= '1')
                                            // {
                                            //     if(data_8 <= '0') data_8 = '2';
                                            //     else if (data_8 > '2') data_8 = '0';
                                            //     else data_8--;
                                            // }
                                            // else
                                            // {
                                            //     if(data_8 <= '0') data_8 = '9';
                                            //     else data_8--;
                                            // }
                                            if(data_8 <= '0') data_8 = '9';
                                            else data_8--;
                                        }break;
                                    //
                                    case(6):
                                        {
                                            // if(shutDownOutV[4] <= '1' && shutDownOutV[5] <= '0')
                                            // {
                                            //     if(data_8 <= '5') data_8 = '9';
                                            //     else if (data_8 > '9') data_8 = '5';
                                            //     else data_8--;
                                            // }
                                            // else if(shutDownOutV[4] >= '1' && shutDownOutV[5] >= '2')
                                            // {
                                            //     data_8 = '0';
                                            // }                                            
                                            // else
                                            // {
                                            //     if(data_8 <= '0') data_8 = '9';
                                            //     else data_8--;
                                            // }
                                            if(data_8 <= '0') data_8 = '9';
                                            else data_8--;
                                        }break;
                                    //
                                    case(7):
                                        {
                                            // if(shutDownOutV[4] <= '1' && shutDownOutV[5] <= '0' && shutDownOutV[6] <= '5')
                                            // {
                                            //     if(data_8 <= '0') data_8 = '9';
                                            //     else if (data_8 > '9') data_8 = '0';
                                            //     else data_8--;
                                            // }
                                            // else if(shutDownOutV[4] >= '1' && shutDownOutV[5] >= '2' && shutDownOutV[6] >= '0')
                                            // {
                                            //     data_8 = '0';
                                            // }
                                            // else
                                            // {
                                            //     if(data_8 <= '0') data_8 = '9';
                                            //     else data_8--;
                                            // }
                                            if(data_8 <= '0') data_8 = '9';
                                            else data_8--;
                                    }break;
                                }//end - switch
                                shutDownOutV[poss_x] = data_8;
                            }
                            else if(enter) 
                            {
                                if(poss_x >= 7)
                                {
                                    data_16=(((shutDownOutV[0]-0x30)*1000)+((shutDownOutV[1]-0x30)*100)+((shutDownOutV[2]-0x30)*10)+(shutDownOutV[3]-0x30));
                                    if(data_16 > (MaxOutputVoltage2_PB * 10))
                                    {
                                        shutDownOutV[0] = ((MaxOutputVoltage2_PB*10) / 1000) + 0x30;
                                        shutDownOutV[1] = (((MaxOutputVoltage2_PB*10) - ((shutDownOutV[0] - 0x30) * 1000)) / 100) + 0x30;
                                        shutDownOutV[2] = (((MaxOutputVoltage2_PB*10) - ((shutDownOutV[0] - 0x30) * 1000) - ((shutDownOutV[1] - 0x30) * 100))/10) + 0x30;
                                        shutDownOutV[3] = ((MaxOutputVoltage2_PB*10) - ((shutDownOutV[0] - 0x30) * 1000) - ((shutDownOutV[1] - 0x30) * 100) - ((shutDownOutV[2] - 0x30) * 10)) + 0x30;
                                    }
                                    else if(data_16 < (MinOutputVoltage2_PB2 * 10))//90v
                                    {
                                        shutDownOutV[0] = ((MinOutputVoltage2_PB2*10) / 1000) + 0x30;
                                        shutDownOutV[1] = (((MinOutputVoltage2_PB2*10) - ((shutDownOutV[0] - 0x30) * 1000)) / 100) + 0x30;
                                        shutDownOutV[2] = (((MinOutputVoltage2_PB2*10) - ((shutDownOutV[0] - 0x30) * 1000) - ((shutDownOutV[1] - 0x30) * 100))/10) + 0x30;
                                        shutDownOutV[3] = ((MinOutputVoltage2_PB2*10) - ((shutDownOutV[0] - 0x30) * 1000) - ((shutDownOutV[1] - 0x30) * 100) - ((shutDownOutV[2] - 0x30) * 10)) + 0x30;
                                    }
                                    
                                    data_16=(((shutDownOutV[4]-0x30)*1000)+((shutDownOutV[5]-0x30)*100)+((shutDownOutV[6]-0x30)*10)+(shutDownOutV[7]-0x30));
                                    if(data_16 < (MinOutputVoltage2_PB * 10))
                                    {
                                        shutDownOutV[4] = ((MinOutputVoltage2_PB*10) / 1000) + 0x30;
                                        shutDownOutV[5] = (((MinOutputVoltage2_PB*10) - ((shutDownOutV[4] - 0x30) * 1000)) / 100) + 0x30;
                                        shutDownOutV[6] = (((MinOutputVoltage2_PB*10) - ((shutDownOutV[4] - 0x30) * 1000) - ((shutDownOutV[5] - 0x30) * 100))/10) + 0x30;
                                        shutDownOutV[7] = ((MinOutputVoltage2_PB*10) - ((shutDownOutV[4] - 0x30) * 1000) - ((shutDownOutV[5] - 0x30) * 100) - ((shutDownOutV[6] - 0x30) * 10)) + 0x30;
                                    }
                                    else if(data_16 > (MidOutputVoltage2_PB * 10))
                                    {
                                        shutDownOutV[4] = ((MidOutputVoltage2_PB*10) / 1000) + 0x30;
                                        shutDownOutV[5] = (((MidOutputVoltage2_PB*10) - ((shutDownOutV[4] - 0x30) * 1000)) / 100) + 0x30;
                                        shutDownOutV[6] = (((MidOutputVoltage2_PB*10) - ((shutDownOutV[4] - 0x30) * 1000) - ((shutDownOutV[5] - 0x30) * 100))/10) + 0x30;
                                        shutDownOutV[7] = ((MidOutputVoltage2_PB*10) - ((shutDownOutV[4] - 0x30) * 1000) - ((shutDownOutV[5] - 0x30) * 100) - ((shutDownOutV[6] - 0x30) * 10)) + 0x30;
                                    }
                                    for(cnt=0;cnt<8;cnt++) write_EE(SHUTDOWNOUTV_0+cnt, shutDownOutV[cnt]);
                                    modi = LO;
                                    Set_OV=((shutDownOutV[0]-0x30)*100)+((shutDownOutV[1]-0x30)*10)+((shutDownOutV[2]-0x30)*1)+((shutDownOutV[3]-0x30)*0.1);
                                    Set_UV=((shutDownOutV[4]-0x30)*100)+((shutDownOutV[5]-0x30)*10)+((shutDownOutV[6]-0x30)*1)+((shutDownOutV[7]-0x30)*0.1);
                                    cal_error = LO;modi_end = HI;Set_OK=1;
                                }
                                else poss_x++;
                                //============================================
                            }
                        }
                        if(esc) modi = LO;
                        key_clr();
                    }
                    else if(read_EE(BATT_TYPE) == 2)
                    {
                        if(up) 
                        {
                            data_8 = shutDownOutV[poss_x];

                            switch(poss_x) 	
                            {
                                case(0):
                                    {
                                        data_8= '1';
                                    }break;
                                    //
                                case(1):
                                    {
                                        if(shutDownOutV[0] == '1')
                                        {
                                            if(data_8 >= '3') data_8= '1';
                                            else if (data_8 < '1') data_8 = '3';
                                            else data_8++;
                                        }
                                        else
                                        {
                                            if(data_8 >= '9') data_8= '0';
                                            else data_8++;
                                        }
                                    }break;
                                //
                                case(2):
                                    {
                                        if(shutDownOutV[0] == '1' && shutDownOutV[1] == '3')
                                        {
                                            if(data_8 >= '5') data_8 = '0';
                                            else if (data_8 < '0') data_8 = '5';
                                            else data_8++;
                                        }
                                        else if(shutDownOutV[0] == '1' && shutDownOutV[1] == '1')
                                        {
                                            if(data_8 >= '9') data_8 = '8';
                                            else if (data_8 < '8') data_8 = '9';
                                            else data_8++;
                                        }
                                        else
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;
                                        }
                                    }break;
                                //
                                case(3):
                                    {
                                        if(shutDownOutV[1] == '3' && shutDownOutV[2] == '5')
                                        {
                                            data_8 = '0';
                                        }		
                                        else if(shutDownOutV[1] == '1' && shutDownOutV[2] == '8')
                                        {
                                            if(data_8 >= '9') data_8 = '4';
                                            else if (data_8 < '4') data_8 = '9';
                                            else data_8++;
                                        }
                                        else
                                        {
                                            if(data_8 >= '9') data_8 = '0';
                                            else data_8++;									
                                        }
                                        //
                                    }break;
                                //
                                case(4):
                                    {
                                        if(data_8 >= '1') data_8 = '0';
                                        else data_8 ='1';
                                    }
                                    break;
                                //
                                case(5):
                                    {
                                        // if(shutDownOutV[4] <= '0')
                                        // {
                                        //     data_8 = '9';
                                        // }
                                        // else
                                        // {
                                        //     if(data_8 >= '1') data_8 = '0';
                                        //     else if (data_8 < '0') data_8 = '1';
                                        //     else data_8++;
                                        // }
                                        if(data_8 >= '9') data_8 = '0';
                                        else data_8++;
                                    }
                                    break;
                                //
                                case(6): 
                                    {
                                        // if(shutDownOutV[4] == '0' && shutDownOutV[5] == '9')
                                        // {
                                        //     if(data_8 >= '9') data_8 = '6';
                                        //     else if (data_8 < '6') data_8 = '9';
                                        //     else data_8++;
                                        // }
                                        // else if(shutDownOutV[4] == '1' && shutDownOutV[5] == '1')
                                        // {
                                        //     if(data_8 >= '8') data_8 = '0';
                                        //     else if (data_8 < '0') data_8 = '8';
                                        //     else data_8++;
                                        // }
                                        // else
                                        // {
                                        //     if(data_8 >= '9') data_8 = '0';
                                        //     else data_8++;
                                        // }
                                        if(data_8 >= '9') data_8 = '0';
                                        else data_8++;
                                    }break;
                                //
                                case(7):
                                    {
                                        // if(shutDownOutV[4] == '1' && shutDownOutV[5] == '1' && shutDownOutV[6] == '8')
                                        // {
                                        //     if(data_8 >= '4') data_8 = '0';
                                        //     else if (data_8 < '0') data_8 = '4';
                                        //     else data_8++;
                                        // }
                                        // else
                                        // {
                                        //     if(data_8 >= '9') data_8 = '0';
                                        //     else data_8++;
                                        // }
                                        if(data_8 >= '9') data_8 = '0';
                                        else data_8++;
                                }break;
                            }//end - switch(poss_x)
                            shutDownOutV[poss_x] = data_8;
                        }
                        else //down key pressed 
                        {
                            if(down)
                            {
                                data_8 = shutDownOutV[poss_x];
                                switch(poss_x) 
                                {
                                    case(0): 
                                        data_8 = '1';
                                        break;
                                    //
                                    case(1):
                                        {
                                            if(data_8 <= '1') data_8= '3';
                                            else if (data_8 > '3') data_8 = '1';
                                            else data_8--;
                                        }break;
                                    //
                                    case(2):
                                        {
                                            if(shutDownOutV[1] == '1')
                                            {
                                                if(data_8 <= '8') data_8 = '9';
                                                else if (data_8 > '9') data_8 = '8';
                                                else data_8--;
                                            }
                                            else if(shutDownOutV[1] == '3')
                                            {
                                                if(data_8 <= '0') data_8 = '5';
                                                else if (data_8 > '5') data_8 = '0';
                                                else data_8--;
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;
                                            }
                                        }break;
                                    //
                                    case(3):
                                        {
                                            if(shutDownOutV[1] == '3' && shutDownOutV[2] == '5')
                                            {
                                                data_8 = '0';
                                            }	
                                            else if(shutDownOutV[1] == '1' && shutDownOutV[2] == '8')
                                            {
                                                if(data_8 <= '4') data_8 = '9';
                                                else if (data_8 > '9') data_8 = '4';
                                                else data_8--;
                                            }
                                            else
                                            {
                                                if(data_8 <= '0') data_8 = '9';
                                                else data_8--;									
                                            }
                                        }break;
                                    //
                                    case(4):
                                        {
                                                // if(data_8 <= '0') data_8 = '1';
                                                // else if (data_8 > '1') data_8 = '0';
                                                // else data_8--;
                                            if(data_8 <= '0')data_8 = '1';
                                            else data_8 = '0';
                                        }break;
                                //	
                                    case(5):
                                        {
                                            // if(shutDownOutV[4] == '0')
                                            // {
                                            //     data_8 = '9';
                                            // }
                                            // else if(shutDownOutV[4] == '1')
                                            // {
                                            //     if(data_8 <= '0') data_8 = '1';
                                            //     else if (data_8 > '1') data_8 = '0';
                                            //     else data_8--;
                                            // }
                                            // else
                                            // {
                                            //     if(data_8 <= '0') data_8 = '9';
                                            //     else data_8--;
                                            // }
                                            if(data_8 <= '0') data_8 = '9';
                                            else data_8--;
                                        }break;
                                    //
                                    case(6):
                                        {
                                            // if(shutDownOutV[4] == '0' && shutDownOutV[5] == '9')
                                            // {
                                            //     if(data_8 <= '6') data_8 = '9';
                                            //     else if (data_8 > '9') data_8 = '6';
                                            //     else data_8--;
                                            // }
                                            // else if(shutDownOutV[4] == '1' && shutDownOutV[5] == '1')
                                            // {
                                            //     if(data_8 <= '0') data_8 = '8';
                                            //     else if (data_8 > '8') data_8 = '0';
                                            //     else data_8--;
                                            // }
                                            // else
                                            // {
                                            //     if(data_8 <= '0') data_8 = '9';
                                            //     else data_8--;
                                            // }
                                            if(data_8 <= '0') data_8 = '9';
                                            else data_8--;
                                            
                                        }break;
                                    //
                                    case(7):
                                        {
                                            // if(shutDownOutV[4] == '1' && shutDownOutV[5] == '1' && shutDownOutV[6] == '8')
                                            // {
                                            //     if(data_8 <= '0') data_8 = '4';
                                            //     else if (data_8 > '4') data_8 = '0';
                                            //     else data_8--;
                                            // }
                                            // else
                                            // {
                                            //     if(data_8 <= '0') data_8 = '9';
                                            //     else data_8--;
                                            // }
                                            if(data_8 <= '0') data_8 = '9';
                                            else data_8--;
                                    }break;
                                }//end - switch
                                shutDownOutV[poss_x] = data_8;
                            }
                            else if(enter) 
                            {
                                if(poss_x >= 7)
                                {
                                    data_16=(((shutDownOutV[0]-0x30)*1000)+((shutDownOutV[1]-0x30)*100)+((shutDownOutV[2]-0x30)*10)+(shutDownOutV[3]-0x30));
                                    if(data_16 > (MaxOutputVoltage_LI * 10))
                                    {
                                        shutDownOutV[0] = ((MaxOutputVoltage_LI*10) / 1000) + 0x30;
                                        shutDownOutV[1] = (((MaxOutputVoltage_LI*10) - ((shutDownOutV[0] - 0x30) * 1000)) / 100) + 0x30;
                                        shutDownOutV[2] = (((MaxOutputVoltage_LI*10) - ((shutDownOutV[0] - 0x30) * 1000) - ((shutDownOutV[1] - 0x30) * 100))/10) + 0x30;
                                        shutDownOutV[3] = ((MaxOutputVoltage_LI*10) - ((shutDownOutV[0] - 0x30) * 1000) - ((shutDownOutV[1] - 0x30) * 100) - ((shutDownOutV[2] - 0x30) * 10)) + 0x30;
                                    }
                                    else if(data_16 < ((MidOutputVoltage_LI) * 10))
                                    {
                                        shutDownOutV[0] = ((MidOutputVoltage_LI*10) / 1000) + 0x30;
                                        shutDownOutV[1] = (((MidOutputVoltage_LI*10) - ((shutDownOutV[0] - 0x30) * 1000)) / 100) + 0x30;
                                        shutDownOutV[2] = (((MidOutputVoltage_LI*10) - ((shutDownOutV[0] - 0x30) * 1000) - ((shutDownOutV[1] - 0x30) * 100))/10) + 0x30;
                                        shutDownOutV[3] = ((MidOutputVoltage_LI*10) - ((shutDownOutV[0] - 0x30) * 1000) - ((shutDownOutV[1] - 0x30) * 100) - ((shutDownOutV[2] - 0x30) * 10)) + 0x30;
                                    }
                                    data_16=(((shutDownOutV[4]-0x30)*1000)+((shutDownOutV[5]-0x30)*100)+((shutDownOutV[6]-0x30)*10)+(shutDownOutV[7]-0x30));
                                    if(data_16 < (MinOutputVoltage_LI * 10))
                                    {
                                        shutDownOutV[4] = ((MinOutputVoltage_LI*10) / 1000) + 0x30;
                                        shutDownOutV[5] = (((MinOutputVoltage_LI*10) - ((shutDownOutV[4] - 0x30) * 1000)) / 100) + 0x30;
                                        shutDownOutV[6] = (((MinOutputVoltage_LI*10) - ((shutDownOutV[4] - 0x30) * 1000) - ((shutDownOutV[5] - 0x30) * 100))/10) + 0x30;
                                        shutDownOutV[7] = ((MinOutputVoltage_LI*10) - ((shutDownOutV[4] - 0x30) * 1000) - ((shutDownOutV[5] - 0x30) * 100) - ((shutDownOutV[6] - 0x30) * 10)) + 0x30;
                                    }
                                    else if(data_16 > (MidOutputVoltage_LI * 10))
                                    {
                                        shutDownOutV[4] = ((MidOutputVoltage_LI*10) / 1000) + 0x30;
                                        shutDownOutV[5] = (((MidOutputVoltage_LI*10) - ((shutDownOutV[4] - 0x30) * 1000)) / 100) + 0x30;
                                        shutDownOutV[6] = (((MidOutputVoltage_LI*10) - ((shutDownOutV[4] - 0x30) * 1000) - ((shutDownOutV[5] - 0x30) * 100))/10) + 0x30;
                                        shutDownOutV[7] = ((MidOutputVoltage_LI*10) - ((shutDownOutV[4] - 0x30) * 1000) - ((shutDownOutV[5] - 0x30) * 100) - ((shutDownOutV[6] - 0x30) * 10)) + 0x30;
                                    }

                                    modi = LO;
                                    Set_OV=((shutDownOutV[0]-0x30)*100)+((shutDownOutV[1]-0x30)*10)+((shutDownOutV[2]-0x30)*1)+((shutDownOutV[3]-0x30)*0.1);
                                    Set_UV=((shutDownOutV[4]-0x30)*100)+((shutDownOutV[5]-0x30)*10)+((shutDownOutV[6]-0x30)*1)+((shutDownOutV[7]-0x30)*0.1);
                                    for(cnt=0;cnt<8;cnt++) write_EE(SHUTDOWNOUTV_0+cnt, shutDownOutV[cnt]);
                                    cal_error = LO;modi_end = HI;Set_OK=1;
                                }
                                else poss_x++;
                                //============================================
                            }
                        }					
                        if(esc) modi = LO;
                        key_clr();
                    }
				}
			}break;
			//
			case(17):
			{
				if(!modi)
				{
					if(up) iMenuIndex = 16;
					else
					{
						if(down) iMenuIndex = 18;
						else if(enter)
						{
							modi = HI;
							poss_x = poss_y = 0;
							fAlarmStop = read_EE(ALARM_STOP);
						}
					}
					if(esc) iMenuIndex = 5;
					key_clr();
				}
				else
				{
					if(up)
					{
						data_8 = fAlarmStop;
						if(data_8 == 'N') data_8 = 'Y';
						else data_8 = 'N';
						fAlarmStop = data_8;
					}
					else
					{
						if(down)
						{
							data_8 = fAlarmStop;
							if(data_8 == 'Y') data_8 = 'N';
							else data_8 = 'Y';
							fAlarmStop = data_8;
						}
						else if(enter)
						{
                            write_EE(ALARM_STOP,fAlarmStop);
							modi = LO;
							err_menu = iMenuIndex;
							iMenuIndex = 17;
							cal_error = HI;
							//modi_end = HI;
						}
					}
					if(esc) modi = LO;
					key_clr();
				}
			}break;
			//
			case(18):
			{
				if(!modi)
				{
					if(up) iMenuIndex = 17;
					else
					{
						if(down) iMenuIndex = 19;
						else if(enter)
						{
							rtc_get_date();
							rtc_get_time();
							modi = HI;
							poss_x = 0;
							bar_poss=0;
						}
					}
					if(esc) iMenuIndex = 5;
					key_clr();
				}
				else
				{
					if(up)
					{
						data_8 = timeBuf[poss_x];
						switch(poss_x)
						{
							case 0: {
								if(data_8 >= '9') data_8 = '0';
								else data_8++;
							}
							timeBuf[0]=data_8;
							break;
							//
							case 1: {
								if(data_8 >= '9') data_8 = '0';
								else data_8++;
							}
							timeBuf[1]=data_8;
							break;
							case 2: {
								if(data_8 >= '1') data_8 = '0';
								else data_8++;
							}
							timeBuf[2]=data_8;
							break;
							//
							case 3: {												// 10month data check
								if(timeBuf[2] >= '1') {
									if(data_8 >= '2') data_8 = '0';
									else data_8++;
								}
								else {
									if(data_8 >= '9') data_8 = '1';
									else data_8++;
								}
							}
							timeBuf[3]=data_8;
							break;
							//
							case 4: {
								if(data_8 >= '3') data_8 = '0';
								else data_8++;
							}
							timeBuf[4]=data_8;break;
							//
							case 5: {												// 10hour data check
								if(timeBuf[4] != '3') {
									if(data_8 >= '9') data_8 = '0';
									else data_8++;
								}
								else {
									if(data_8 >= '1') data_8 = '0';
									else data_8++;
								}
							}
							timeBuf[5]=data_8;
							break;
							//
							case 6: {
								if(data_8 >= '2') data_8 = '0';
								else data_8++;
							}
							timeBuf[6]=data_8;
							break;
							//
							case 7: {
								if(timeBuf[6] != '2') {
									if(data_8 >= '9') data_8 = '0';
									else data_8++;
								}
								else {
									if(data_8 >= '3') data_8 = '0';
									else data_8++;
								}
							}
							timeBuf[7]=data_8;
							break;
							//
							case 8:	{
								if(data_8 >= '5') data_8 = '0';
								else data_8++;
							}
							timeBuf[8]=data_8;
							break;
							case 9: {
								if(data_8 >= '9') data_8 = '0';
								else data_8++;
							}
							timeBuf[9]=data_8;
							break;
							case 10: {
								if(data_8 >= '5') data_8 = '0';
								else data_8++;
							}
							timeBuf[10]=data_8;
							break;
							case 11: {
								if(data_8 >= '9') data_8 = '0';
								else data_8++;
							}
							timeBuf[11]=data_8;
							break;
						}
						//temp_buffer[poss_x] = data_8;
						year = ((timeBuf[0]-0x30)*10)+(timeBuf[1]-0x30);
						mth = ((timeBuf[2]-0x30)*10)+(timeBuf[3]-0x30);
						day = ((timeBuf[4]-0x30)*10)+(timeBuf[5]-0x30);
						hour = ((timeBuf[6]-0x30)*10)+(timeBuf[7]-0x30);
						min = ((timeBuf[8]-0x30)*10)+(timeBuf[9]-0x30);
						sec = ((timeBuf[10]-0x30)*10)+(timeBuf[11]-0x30);
					}
					else {
						if(down) {
							data_8 = timeBuf[poss_x];
							switch(poss_x) {
								case 0:	{
									if(data_8 <= '0') data_8 = '9';
									else data_8--;
								}
								timeBuf[0]=data_8;
								break;
								//
								case 1:	{
									if(data_8 <= '0') data_8 = '9';
									else data_8--;
								}
								timeBuf[1]=data_8;
								break;
								//
								case 2: {
									if(data_8 <= '0') data_8 = '1';
									else data_8--;
								}
								timeBuf[2]=data_8;
								break;
								//
								case 3: {
									if(timeBuf[2] <= '0') {						// 10month data check
										if(data_8 <= '1') data_8 = '9';
										else data_8--;
									}
									else if(timeBuf[2] >=1)
									{
										if(data_8 <= '0')	data_8 = '2';
										else data_8--;										
									}								
								}
								timeBuf[3]=data_8;
								break;
								//
								case 4: {
									if(data_8 <= '0') data_8 = '3';
									else data_8--;
								}
								timeBuf[4]=data_8;
								break;
								//
								case 5: {
									if(timeBuf[4] >= '3') {						// 10hour data check
										if(data_8 <= '0') data_8 = '1';
										else data_8--;
									}
									else {
										if(data_8 <= '0') data_8 = '9';
										else data_8--;
									}
								}
								timeBuf[5]=data_8;
								break;
								//
								case 6: {
									if(data_8 <= '0') data_8 = '2';
									else data_8--;
								}
								timeBuf[6]=data_8;
								break;
								//
								case 7: {
									if(timeBuf[6] >= '2') {
										if(data_8 <= '0') data_8 = '3';
										else data_8--;
									}
									else {
										if(data_8 <= '0') data_8 = '9';
										else data_8--;
									}
								}
								timeBuf[7]=data_8;
								break;
								//
								case 8: {
									if(data_8 <= '0') data_8 = '5';
									else data_8--;
								}
								timeBuf[8]=data_8;
								break;
								//
								case 9:	{
									if(data_8 <= '0') data_8 = '9';
									else data_8--;
								}
								timeBuf[9]=data_8;
								break;
								//
								case 10: {
									if(data_8 <= '0') data_8 = '5';
									else data_8--;
								}
								timeBuf[10]=data_8;
								break;
								//
								case 11:{
									if(data_8 <= '0') data_8 = '9';
									else data_8--;
								}
								timeBuf[11]=data_8;
								break;
							}
							//temp_buffer[poss_x] = data_8;
							year = ((timeBuf[0]-0x30)*10)+(timeBuf[1]-0x30);
							mth = ((timeBuf[2]-0x30)*10)+(timeBuf[3]-0x30);
							day = ((timeBuf[4]-0x30)*10)+(timeBuf[5]-0x30);
							hour = ((timeBuf[6]-0x30)*10)+(timeBuf[7]-0x30);
							min = ((timeBuf[8]-0x30)*10)+(timeBuf[9]-0x30);
							sec = ((timeBuf[10]-0x30)*10)+(timeBuf[11]-0x30);
						}
						else if(enter) {
							if(poss_x >= 11) {
								modi = LO;modi_end = HI;
								rtc_set_datetime(day, mth, year, 0, hour, min);
							}
							else poss_x++;
						}
					}
					switch(poss_x) {
						case 0: bar_poss = 0;break;					// 10year
						case 1: bar_poss = 1;break;					// 1year
						case 2: bar_poss = 3;break;					// 10month
						case 3: bar_poss = 4;break;					// 1month
						case 4: bar_poss = 6;break;					// 10day
						case 5: bar_poss = 7;break;					// 1day
						case 6: bar_poss = 8;break;					// 10hour
						case 7: bar_poss = 9;break;				// 1hour
						case 8: bar_poss = 11;break;				// 10minite
						case 9: bar_poss = 12;break;				// 1minite
						case 10: bar_poss = 14;break;				// 10sec
						case 11: bar_poss = 15;break;				// 1sec
					}
					if(esc) modi = LO;
					key_clr();
				}
			}break;
			//	
			case(19): {
				if(!modi) {
					if(up) iMenuIndex=18;
					else {
						if(down) iMenuIndex = 20;
						else if(enter) {
							modi = HI;
							poss_x = 0;
							for(cnt=0;cnt<6;cnt++) gPin[cnt] = '0';
						}
					}
					if(esc) iMenuIndex = 5;
					key_clr();
				}
				else {
					if(up) {
						data_8 = gPin[poss_x];
						if(data_8 >= '9') data_8 = '0';							
						else data_8++;
						gPin[poss_x] = data_8;

					}
					else {
						if(down) {
							data_8 = gPin[poss_x];
							if(data_8 == '0') data_8 = '9';
							else data_8--;
							gPin[poss_x] = data_8;
						}
						else if(enter) {
							if(poss_x >= 5) {
								modi = LO;modi_end = HI;
								for(cnt=0;cnt<6;cnt++) write_EE((PASSWD_0+cnt),gPin[cnt]);
							}						
							else poss_x++;
						}
					}
					if(esc) modi = LO;
					key_clr();
				}
			}break;
			//
			case(20):
			{
				if(!modi)
				{
					if(up) iMenuIndex = 19;
					else
					{
						if(down) iMenuIndex = 21;
						else if(enter)
						{
							modi = HI;
							poss_x = poss_y = 0;
							temp_buffer[poss_x]='N';
						}
					}
					if(esc) iMenuIndex = 5;
					key_clr();
				}
				else
				{
					if(up)
					{
						data_8 = temp_buffer[poss_x];
						if(data_8 == 'N') data_8 = 'Y';
						else data_8 = 'N';
						temp_buffer[poss_x] = data_8;
					}
					else
					{
						if(down)
						{
							data_8 = temp_buffer[poss_x];
							if(data_8 == 'Y') data_8 = 'N';
							else data_8 = 'Y';
							temp_buffer[poss_x] = data_8;
						}
						else if(enter)
						{
							if(temp_buffer[poss_x] == 'Y')
							{
								sysInit();
								ParameterInitialize();
                                CHG_SEL = 0;
                                CHG_Mode = 0;
                                Auto_Flag = 0;
                                CHG_CMD();
                                CHG_Check();
                                Set_OK = 1;
							}
							modi = LO;
							err_menu = iMenuIndex;
							iMenuIndex = 20;
							cal_error = HI;
						}
					}
					if(esc) modi = LO;
					key_clr();
				}
			}break;
            
			case(21):
			{
				if(!modi)
				{
					if(up) iMenuIndex = 20;
					else
					{
						if(down)
                        {
                            if(read_EE(BATT_TYPE) == 2) iMenuIndex = 25;
                            else iMenuIndex = 22;
                        }                        
						else if(enter) 
                        {
                            modi = HI;
                            poss_x = 2;
						}
					}
					if(esc) iMenuIndex = 5;
					key_clr();
				}
				else {//when modi is HIGH//
					if(up) {
						vGain = ADJ_VGain;
						cGain = ADJ_CGain;
						switch(poss_x) {
							case(2): {
								if(vGain >=128) vGain+= 1;
								else if(vGain >= 127) vGain = 127;
								else vGain+= 1;
							}break;
							//
							case(5): {
								if(cGain >=128) cGain+= 1;
								else if(cGain >= 127) cGain = 127;
								else cGain+= 1;
							}break;
							//
						}
						ADJ_VGain = vGain;
						ADJ_CGain = cGain;
					}
					else {
						if(down) {
							vGain = ADJ_VGain;
							cGain = ADJ_CGain;
							switch(poss_x) {
								case(2): {
									if(vGain <=127) vGain-= 1;
									else if(vGain <= 128) vGain = 128;
									else vGain-= 1;
								}break;
								//
								case(5): {
									if(cGain <=127) cGain-= 1;
									else if(cGain <= 128) cGain = 128;
									else cGain-= 1;
								}
								break;
							}
							ADJ_VGain = vGain;
							ADJ_CGain = cGain;
						}
						else if(enter) {
							if(poss_x >= 5) {
								modi = LO;modi_end = HI;
								write_EE(ADJVGain_0, ADJ_VGain);
								write_EE(ADJCGain_0, ADJ_CGain);
								ADJ_VGain = ADJ_VGain;
								ADJ_CGain = ADJ_CGain;
								cal_error = LO;modi_end = HI;Set_OK=1;
							}
							else {
								if(poss_x==2) poss_x=5;
							}
						}
					}
					if(esc) {
						modi = LO;
					}
					key_clr();
				}
			}break;
            case(22):       //CHG MODE
			{
				if(!modi)
				{                    
					if(up) iMenuIndex = 21;
					else
					{
						if(down) iMenuIndex = 23;
						else if(enter)
						{
							modi = HI;
						}
					}
					if(esc) iMenuIndex = 5;
					key_clr();
				}
				else
				{
					if(up)
					{
                        if(poss_x >= 1) 
                        {
                            if(poss_y >= 1)
                            {
                                poss_x = 0;
                                poss_y = 0;
                            }
                            else
                            {
                                poss_y++;
                            }
                        }
                        else poss_x++;
  					}
					else
					{
						if(down)
						{
		                    if(poss_x <= 0) 
                            {
                                if(poss_y <= 0)
                                {
                                    poss_x = 1;
                                    poss_y = 1;
                                }
                                else
                                {
                                    poss_x--;
                                }
                            }
                            else if(poss_x >= 1 && poss_y <= 0)
                            {
                                poss_x = 0;
                            }
                            else poss_y--;
						}
						else if(enter)
						{
                           	modi = LO;
							err_menu = iMenuIndex;
							iMenuIndex = 22;
							cal_error = HI;
                            CHG_SEL = poss_x;
                            CHG_Mode = poss_y;
                            write_EE(CHG_STS_0, CHG_SEL);
                            write_EE(CHG_STS_1, CHG_Mode);
                            if(CHG_SEL == 0 && CHG_Mode == 0)    Auto_Flag = 0;
                            else    Auto_Flag = 2;                                
                            CHG_EN = 0;
                            CHG_CMD();
                            Set_OK = 1;
						}
					}
					if(esc) modi = LO;
					key_clr();
				}
			}break;
            
            case(23):           // CHG TIME
			{
				if(!modi)
				{   
					if(up) iMenuIndex = 22;
					else
					{
						if(down) iMenuIndex = 24;
						else if(enter)
						{
                            modi = HI;
							poss_x = 0; 
						}
					}
					if(esc) iMenuIndex = 5;
					key_clr();
				}
				else
				{
                    if(up)
					{
                        data_8 = Set_time[poss_x];
						switch(poss_x)
						{
                            case 0:
                            {           
								if(data_8 >= '9') data_8 = '0';
								else data_8++;
							}break;
                            
							case 1:
                            {
                                if(data_8 >= '9') data_8 = '0';
								else data_8++;
							}break;
							//
							case 2: 
                            {
								if(data_8 >= '9') data_8 = '0';
								else data_8++;
							}break;
                            
							case 3: 
                            {
								if(data_8 >= '1') data_8 = '0';
								else data_8++;
							}break;
							//
							case 4: 
                            {												// 10month data check
								if(Set_time[3] >= '1') 
                                {
									if(data_8 >= '2') data_8 = '0';
									else data_8++;
								}
								else 
                                {
									if(data_8 >= '9') data_8 = '0';
									else data_8++;
								}
							}break;
							//
							case 5: {
								if(data_8 >= '3') data_8 = '0';
								else data_8++;
							}break;
							
							//
							case 6: {												// 10hour data check
								if(Set_time[5] != '3') {
									if(data_8 >= '9') data_8 = '0';
									else data_8++;
								}
								else {
									if(data_8 >= '1') data_8 = '0';
									else data_8++;
								}
   							}
							
							break;
							//
							case 7: {
								if(data_8 >= '2') data_8 = '0';
								else data_8++;
							}
							
							break;
							//
							case 8: {
								if(Set_time[7] != '2') {
									if(data_8 >= '9') data_8 = '0';
									else data_8++;
								}
								else {
									if(data_8 >= '3') data_8 = '0';
									else data_8++;
								}
							}
							
							break;
							//
							case 9:	{
								if(data_8 >= '5') data_8 = '0';
								else data_8++;
							}
							
							break;
							case 10: {
								if(data_8 >= '9') data_8 = '0';
								else data_8++;
							}
							
							break;
							case 11: {
								if(data_8 >= '5') data_8 = '0';
								else data_8++;
							}
							
							break;
							case 12: {
								if(data_8 >= '9') data_8 = '0';
								else data_8++;
							}
							
							break;
						}
                        Set_time[poss_x] = data_8;
                        if(poss_x == 0)
                        {
                            Set_time[1] = (read_EE(SET_YEAR_0 + ((Set_time[0]-0x30)*6))/10) + 0x30;
                            Set_time[2] = (read_EE(SET_YEAR_0 + ((Set_time[0]-0x30)*6))%10) + 0x30;
                            Set_time[3] = (read_EE(SET_MONTH_0 + ((Set_time[0]-0x30)*6))/10) + 0x30;
                            Set_time[4] = (read_EE(SET_MONTH_0 + ((Set_time[0]-0x30)*6))%10) + 0x30;
                            Set_time[5] = (read_EE(SET_DAY_0 + ((Set_time[0]-0x30)*6))/10) + 0x30;
                            Set_time[6] = (read_EE(SET_DAY_0 + ((Set_time[0]-0x30)*6))%10) + 0x30;
                            Set_time[7] = (read_EE(SET_HOUR_0 + ((Set_time[0]-0x30)*6))/10) + 0x30;
                            Set_time[8] = (read_EE(SET_HOUR_0 + ((Set_time[0]-0x30)*6))%10) + 0x30;
                            Set_time[9] = (read_EE(SET_MIN_0 + ((Set_time[0]-0x30)*6))/10) + 0x30;
                            Set_time[10] = (read_EE(SET_MIN_0 + ((Set_time[0]-0x30)*6))%10) + 0x30;
                            Set_time[11] = (read_EE(SET_SEC_0 + ((Set_time[0]-0x30)*6))/10) + 0x30;
                            Set_time[12] = (read_EE(SET_SEC_0 + ((Set_time[0]-0x30)*6))%10) + 0x30;
                        }
					}
					else 
                    {
						if(down) 
                        {
							data_8 = Set_time[poss_x];
							switch(poss_x) 
                            {
                                case 0: 
                                {
                                    if(data_8 <= '0') data_8 = '9';
                                    else data_8--; 
                                }                         
                                break;
								case 1:	
                                {
									if(data_8 <= '0') data_8 = '9';
									else data_8--;
								}
								
								break;
								//
								case 2:	{
									if(data_8 <= '0') data_8 = '9';
									else data_8--;
								}
								
								break;
								//
								case 3: {
									if(data_8 <= '0') data_8 = '1';
									else data_8--;
								}
								break;
								//
								case 4: {
									if(Set_time[3] <= '0') {						// 10month data check
										if(data_8 <= '0') data_8 = '9';
										else data_8--;
									}
									else if(Set_time[3] >=1)
									{
										if(data_8 <= '0')	data_8 = '2';
										else data_8--;										
									}								
								}
								break;
								//
								case 5: {
									if(data_8 <= '0') data_8 = '3';
									else data_8--;
								}
								break;
								//
								case 6: {
									if(Set_time[5] >= '3') {						// 10hour data check
										if(data_8 <= '0') data_8 = '1';
										else data_8--;
									}
									else {
										if(data_8 <= '0') data_8 = '9';
										else data_8--;
									}
								}
								break;
								//
								case 7: {
									if(data_8 <= '0') data_8 = '2';
									else data_8--;
								}
								break;
								//
								case 8: {
									if(Set_time[7] >= '2') {
										if(data_8 <= '0') data_8 = '3';
										else data_8--;
									}
									else {
										if(data_8 <= '0') data_8 = '9';
										else data_8--;
									}
								}
								break;
								//
								case 9: {
									if(data_8 <= '0') data_8 = '5';
									else data_8--;
								}
								break;
								//
								case 10:	{
									if(data_8 <= '0') data_8 = '9';
									else data_8--;
								}
								break;
								//
								case 11: {
									if(data_8 <= '0') data_8 = '5';
									else data_8--;
								}
								break;
								//
								case 12:{
									if(data_8 <= '0') data_8 = '9';
									else data_8--;
								}
								break;
							}
                            
							Set_time[poss_x] = data_8;
                            if(poss_x == 0)
                            {
                                Set_time[1] = (read_EE(SET_YEAR_0 + ((Set_time[0]-0x30)*6))/10) + 0x30;
                                Set_time[2] = (read_EE(SET_YEAR_0 + ((Set_time[0]-0x30)*6))%10) + 0x30;
                                Set_time[3] = (read_EE(SET_MONTH_0 + ((Set_time[0]-0x30)*6))/10) + 0x30;
                                Set_time[4] = (read_EE(SET_MONTH_0 + ((Set_time[0]-0x30)*6))%10) + 0x30;
                                Set_time[5] = (read_EE(SET_DAY_0 + ((Set_time[0]-0x30)*6))/10) + 0x30;
                                Set_time[6] = (read_EE(SET_DAY_0 + ((Set_time[0]-0x30)*6))%10) + 0x30;
                                Set_time[7] = (read_EE(SET_HOUR_0 + ((Set_time[0]-0x30)*6))/10) + 0x30;
                                Set_time[8] = (read_EE(SET_HOUR_0 + ((Set_time[0]-0x30)*6))%10) + 0x30;
                                Set_time[9] = (read_EE(SET_MIN_0 + ((Set_time[0]-0x30)*6))/10) + 0x30;
                                Set_time[10] = (read_EE(SET_MIN_0 + ((Set_time[0]-0x30)*6))%10) + 0x30;
                                Set_time[11] = (read_EE(SET_SEC_0 + ((Set_time[0]-0x30)*6))/10) + 0x30;
                                Set_time[12] = (read_EE(SET_SEC_0 + ((Set_time[0]-0x30)*6))%10) + 0x30;
                            }
                                    
						}
						else if(enter) 
                        {
							if(poss_x >= 12) 
                            {
								modi = LO;modi_end = HI;
                                year_buf = ((Set_time[1]-0x30)*10)+(Set_time[2]-0x30);
                                mth_buf = ((Set_time[3]-0x30)*10)+(Set_time[4]-0x30);
                                day_buf = ((Set_time[5]-0x30)*10)+(Set_time[6]-0x30);
                                hour_buf = ((Set_time[7]-0x30)*10)+(Set_time[8]-0x30);
                                min_buf = ((Set_time[9]-0x30)*10)+(Set_time[10]-0x30);
                                sec_buf = ((Set_time[11]-0x30)*10)+(Set_time[12]-0x30);
                                write_EE(CHG_SET_NO, Set_time[0]-0x30);
								write_EE(SET_YEAR_0+((Set_time[0]-0x30)*6), year_buf);
                                write_EE(SET_MONTH_0+((Set_time[0]-0x30)*6), mth_buf);
                                write_EE(SET_DAY_0+((Set_time[0]-0x30)*6), day_buf);
                                write_EE(SET_HOUR_0+((Set_time[0]-0x30)*6), hour_buf);
                                write_EE(SET_MIN_0+((Set_time[0]-0x30)*6), min_buf);
                                write_EE(SET_SEC_0+((Set_time[0]-0x30)*6), sec_buf);
                            }
							else poss_x++;
						} 
                    }
                    
                    if(esc) modi = LO;
                    key_clr();
                    
				}
                
                
            }break;
            
            case(24):           //CHG Condition
			{
				if(!modi)
				{   
					if(up) iMenuIndex = 23;
					else
					{
						if(down) iMenuIndex = 25;
						else if(enter)
						{
                            modi = HI;
							poss_x = 0; 
						}
					}
					if(esc) iMenuIndex = 5;
					key_clr();
				}
				else
				{
                    if(up)
					{
                        data_8 = Set_COND[poss_x];
						switch(poss_x)
						{
                            case 0:
                            {           
								if(data_8 >= '9') data_8 = '0';
								else data_8++;
							}break;
                            
							case 1:
                            {
                                if(Set_COND[0] == '0')
                                {
                                    if(data_8 >= '9') data_8 = '1';
                                    else data_8++;
                                }
                                else
                                {
                                    if(data_8 >= '9') data_8 = '0';
                                    else data_8++;
                                }
                                
							}break;
							//
							case 2: 
                            {
								if(data_8 >= '9') data_8 = '0';
								else data_8++;
							}break;
                            
							case 3: 
                            {
                                if(Set_COND[2] == '0')
                                {
                                    if(data_8 >= '9') data_8 = '1';
                                    else data_8++;
                                }
                                else
                                {
                                    if(data_8 >= '9') data_8 = '0';
                                    else data_8++;
                                }
							}break;
                        }
                        Set_COND[poss_x] = data_8;
                    }
                    else 
                    {
						if(down) 
                        {
							data_8 = Set_COND[poss_x];
							switch(poss_x) 
                            {
                                case 0: 
                                {
                                    if(data_8 <= '0') data_8 = '9';
                                    else data_8--; 
                                }                         
                                break;
								case 1:	
                                {
                                    if(Set_COND[0] == '0')
                                    {
                                        if(data_8 <= '1') data_8 = '9';
                                        else data_8--;
                                    }
                                    else
                                    {
                                        if(data_8 <= '0') data_8 = '9';
                                        else data_8--;
                                    }
								}
								
								break;
								//
								case 2:	{
									if(data_8 <= '0') data_8 = '9';
									else data_8--;
								}
								
								break;
								//
								case 3: 
                                {
									if(Set_COND[2] == '0')
                                    {
                                        if(data_8 <= '1') data_8 = '9';
                                        else data_8--;
                                    }
                                    else
                                    {
                                        if(data_8 <= '0') data_8 = '9';
                                        else data_8--;
                                    }
								}
								break;
							}                            
							Set_COND[poss_x] = data_8;
						}
						else if(enter) 
                        {
							if(poss_x >= 3) 
                            {
								modi = LO;modi_end = HI;
                                CHG_Cut_TIME = ((Set_COND[0]-0x30)*10)+(Set_COND[1]-0x30);
                                CHG_Cut_CURR = ((Set_COND[2]-0x30)*10)+(Set_COND[3]-0x30);
                                if(CHG_Cut_TIME == 0)
                                {
                                    Set_COND[0] = '0';
                                    Set_COND[1] = '1';
                                    CHG_Cut_TIME = ((Set_COND[0]-0x30)*10)+(Set_COND[1]-0x30);
                                }
                                if(CHG_Cut_CURR == 0)
                                {
                                    Set_COND[2] = '0';
                                    Set_COND[3] = '1';
                                    CHG_Cut_CURR = ((Set_COND[2]-0x30)*10)+(Set_COND[3]-0x30);
                                }
                                write_EE(CHG_SET_TIME, CHG_Cut_TIME);
                                write_EE(CHG_SET_CURR, CHG_Cut_CURR);
                            }
							else poss_x++;
						} 
                    }
                    if(esc) modi = LO;
                    key_clr();
				}
            }break;
            
            case(25): {
				if(!modi)
				{
					if(up) 
                    {
                        if(read_EE(BATT_TYPE) == 2) iMenuIndex = 21;
                        else iMenuIndex = 24;
                    }
					else
					{
						if(down) 
                        {
                            if(read_EE(BATT_TYPE) == 2) iMenuIndex = 9;
                            else iMenuIndex = 10;
                        }
						else if(enter)
						{		//press Enter key routine -> else is Enter key pressed routine
							modi = HI;
							poss_x = 0;
							for(cnt=0;cnt<6;cnt++) temp_buffer[cnt] = '0';
						}
                        
					}
                    if(esc) iMenuIndex = 5;
					key_clr();
				}
				else
				{	//system setup passwd display Enter Key
					if(up)
					{
						data_8 = temp_buffer[poss_x];
						if(data_8 == '9') data_8 = '0';
						else data_8++;
						temp_buffer[poss_x] = data_8;
					}
					else
					{
						if(down)
						{
							data_8 = temp_buffer[poss_x];
							if(data_8 == '0') data_8 = '9';
							else data_8--;
							temp_buffer[poss_x] = data_8;
						}
						else if(enter)
						{
							if(poss_x >= 5)
							{
								i = 0;modi = LO;
								for(cnt=0;cnt<6;cnt++)
								{
									data_8 = gPin2[cnt];
									data_cal = temp_buffer[cnt];
									i += (data_8 ^ data_cal);
								}
								if(i == 0) iMenuIndex = 26;
								else
								{
									i = 0;
									for(cnt=0;cnt<6;cnt++)
									{
										data_8 = gPin2[cnt];
										data_cal = temp_buffer[cnt];
										i += (data_8 ^ data_cal);
									}
									if(i == 0) iMenuIndex = 26;
								}
							}						
							else poss_x++;
						}
						if(esc) {
							modi = LO;
							iMenuIndex = 5;		//pressed ESC key Return to System Setting Password Menu
						}
					}
					key_clr();
				}
			}break;            
                           
            case(26):
			{
				if(!modi)
				{
					if(up) iMenuIndex = 27;
					else
					{
						if(down) iMenuIndex = 27;
						else if(enter)
						{
							modi = HI;
							poss_x = poss_y = 0;
							temp_buffer[0] = read_EE(OP_MODE);
						}
					}
					if(esc) iMenuIndex = 25;
					key_clr();
				}
				else
				{
					if(up)
					{
						data_8 = temp_buffer[0];
						if(data_8 == 'N') data_8 = 'Y';
						else data_8 = 'N';
						temp_buffer[0] = data_8;
					}
					else
					{
						if(down)
						{
							data_8 = temp_buffer[0];
							if(data_8 == 'Y') data_8 = 'N';
							else data_8 = 'Y';
							temp_buffer[0] = data_8;
						}
						else if(enter)
						{
							modi = LO;
							err_menu = iMenuIndex;
							iMenuIndex = 26;
							cal_error = HI;
							write_EE((OP_MODE),temp_buffer[0]);
                            if(read_EE(OP_MODE) == 'Y') CHG_Sts.b5 = 1;
                            else    CHG_Sts.b5 = 0;
                            Set_OK=1;
						}
					}
					if(esc) modi = LO;
					key_clr();
				}
			}break;           
            
            case(27):
			{
				if(!modi)
				{
					if(up) iMenuIndex = 26;
					else
					{
						if(down) iMenuIndex = 26;
						else if(enter)
						{
							modi = HI;
							poss_x = poss_y = 0;
                            temp_buffer[0] = read_EE(INPUT_VOLT);
							temp_buffer[1] = read_EE(BATT_TYPE);  
						}
					}
					if(esc) iMenuIndex = 25;
					key_clr();
				}
               
				else
				{
					if(up)
					{
                        switch(poss_x) 
                        {
                            case 0:
                            {           
								data_8 = temp_buffer[0];
                                if(data_8 == 0) data_8 = 1;
                                else data_8 = 0;
                                temp_buffer[0] = data_8;
							}break;
                            
                            case 1:
                            {    
                                data_8 = temp_buffer[1];
                                if(data_8 >= 2) data_8 = 0;
                                else data_8++;
                                temp_buffer[1] = data_8;
                            }break;
                        }
					}
					else
					{
						if(down)
						{
                            switch(poss_x) 
                            {
                                case 0:
                                { 
                           			data_8 = temp_buffer[0];
                                    if(data_8 == 1) data_8 = 0;
                                    else data_8 = 1;
                                    temp_buffer[0] = data_8;
                                }break;
                                
                                case 1:
                                { 
                           			data_8 = temp_buffer[1];
                                    if(data_8 <= 0) data_8 = 2;
                                    else data_8--;
                                    temp_buffer[1] = data_8;
                                }break;
                            }
						}
						else if(enter)
						{
                            if(poss_x >= 1) 
                            {
                                modi = LO;modi_end = HI;
                                err_menu = iMenuIndex;
                                iMenuIndex = 27;
                                cal_error = HI;
                                write_EE((INPUT_VOLT),temp_buffer[0]);
                                write_EE((BATT_TYPE),temp_buffer[1]);  
                                                                
                                if(read_EE(INPUT_VOLT) == 0)
                                {
                                    Input_UV_Ref = MinInputVoltage_380;	//18.06.18 -10% - KHJ
                                    Input_OV_Ref = MaxInputVoltage_380;  	//18.06.18 +10% - KHJ
                                    Input_V_Ref = RefInputVoltage_380;
                                    Input_Shutdown_Ref = ShutdownVoltage_380;   //18.06.18 -40% - Order by GJJ 
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
                                    Input_UV_Ref = MinInputVoltage_220;	//18.06.18 -10% - KHJ
                                    Input_OV_Ref = MaxInputVoltage_220;  	//18.06.18 +10% - KHJ
                                    Input_V_Ref = RefInputVoltage_220;
                                    Input_Shutdown_Ref = ShutdownVoltage_220;   //18.06.18 -40% - Order by GJJ 
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
                                
                                for(cnt=0;cnt<8;cnt++)
                                {
                                    ginValarm[cnt] = read_EE(INVALARM_0+cnt);
                                }
                                Set_Input_OV = ((ginValarm[0]-0x30)*100)+((ginValarm[1]-0x30)*10)+((ginValarm[2]-0x30)*1)+((ginValarm[3]-0x30)*0.1);
                                Set_Input_UV = ((ginValarm[4]-0x30)*100)+((ginValarm[5]-0x30)*10)+((ginValarm[6]-0x30)*1)+((ginValarm[7]-0x30)*0.1);
                                
                                if(read_EE(BATT_TYPE) == 0)
                                {
                                    write_EE(SHUTDOWNOUTV_0,    '1');
                                    write_EE(SHUTDOWNOUTV_1,    '5');
                                    write_EE(SHUTDOWNOUTV_2,    '0');
                                    write_EE(SHUTDOWNOUTV_3,    '0');
                                    write_EE(SHUTDOWNOUTV_4,    '0');    
                                    write_EE(SHUTDOWNOUTV_5,    '9');
                                    write_EE(SHUTDOWNOUTV_6,    '8');
                                    write_EE(SHUTDOWNOUTV_7,    '6');
                                    
                                    write_EE(BATTVOLT_0,        '1');
                                    write_EE(BATTVOLT_1,        '3');
                                    write_EE(BATTVOLT_2,        '1');
                                    write_EE(BATTVOLT_3,        '0');
                                    write_EE(BATTVOLT_4,        '1');
                                    write_EE(BATTVOLT_5,        '4');
                                    write_EE(BATTVOLT_6,        '1');
                                    write_EE(BATTVOLT_7,        '0');
                  
                                    write_EE(OUTVALARM_0,		'1');
                                    write_EE(OUTVALARM_1,		'5');
                                    write_EE(OUTVALARM_2,		'0');
                                    write_EE(OUTVALARM_3,		'0');
                                    write_EE(OUTVALARM_4,		'0');
                                    write_EE(OUTVALARM_5,		'9');
                                    write_EE(OUTVALARM_6,		'8');
                                    write_EE(OUTVALARM_7,		'6');
                                    CHG_Sts.b4 = 0;
                                    CHG_Sts.b3 = 0;
                                }
                                else if(read_EE(BATT_TYPE) == 1)
                                {
                                    write_EE(SHUTDOWNOUTV_0,    '1');
                                    write_EE(SHUTDOWNOUTV_1,    '5');
                                    write_EE(SHUTDOWNOUTV_2,    '0');
                                    write_EE(SHUTDOWNOUTV_3,    '0');
                                    write_EE(SHUTDOWNOUTV_4,    '1');    
                                    write_EE(SHUTDOWNOUTV_5,    '0');
                                    write_EE(SHUTDOWNOUTV_6,    '5');
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
                                    write_EE(OUTVALARM_1,		'5');
                                    write_EE(OUTVALARM_2,		'0');
                                    write_EE(OUTVALARM_3,		'0');
                                    write_EE(OUTVALARM_4,		'1');
                                    write_EE(OUTVALARM_5,		'0');
                                    write_EE(OUTVALARM_6,		'5');
                                    write_EE(OUTVALARM_7,		'0');
                                    CHG_Sts.b4 = 0;
                                    CHG_Sts.b3 = 1;
                                }
                                else if(read_EE(BATT_TYPE) == 2)
                                {
                                    write_EE(SHUTDOWNOUTV_0,    '1');
                                    write_EE(SHUTDOWNOUTV_1,    '3');
                                    write_EE(SHUTDOWNOUTV_2,    '5');
                                    write_EE(SHUTDOWNOUTV_3,    '0');
                                    write_EE(SHUTDOWNOUTV_4,    '0');    
                                    write_EE(SHUTDOWNOUTV_5,    '9');
                                    write_EE(SHUTDOWNOUTV_6,    '6');
                                    write_EE(SHUTDOWNOUTV_7,    '0');                                   
      
                                    write_EE(BATTVOLT_0,        '1');
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
                                    write_EE(OUTVALARM_6,		'6');
                                    write_EE(OUTVALARM_7,		'0');
                                    CHG_Sts.b4 = 1;
                                    CHG_Sts.b3 = 0;
                                }
                                for(cnt=0;cnt<8;cnt++)
                                {
                                    shutDownOutV[cnt] = read_EE(SHUTDOWNOUTV_0+cnt);
                                    goutValarm[cnt] = read_EE(OUTVALARM_0+cnt);
                                    battVoltSet[cnt] = read_EE(BATTVOLT_0+cnt);
                                    outSetNew[cnt] = read_EE(BATTVOLT_0+cnt);
                                }
                                Set_Input_OV = ((ginValarm[0]-0x30)*100)+((ginValarm[1]-0x30)*10)+((ginValarm[2]-0x30)*1)+((ginValarm[3]-0x30)*0.1);
                                Set_Input_UV = ((ginValarm[4]-0x30)*100)+((ginValarm[5]-0x30)*10)+((ginValarm[6]-0x30)*1)+((ginValarm[7]-0x30)*0.1);
                                
                                Set_OV = ((shutDownOutV[0]-0x30)*100)+((shutDownOutV[1]-0x30)*10)+((shutDownOutV[2]-0x30)*1)+((shutDownOutV[3]-0x30)*0.1);
                                Set_UV = ((shutDownOutV[4]-0x30)*100)+((shutDownOutV[5]-0x30)*10)+((shutDownOutV[6]-0x30)*1)+((shutDownOutV[7]-0x30)*0.1);
                                Set_Output_OV = ((goutValarm[0]-0x30)*100)+((goutValarm[1]-0x30)*10)+((goutValarm[2]-0x30)*1)+((goutValarm[3]-0x30)*0.1);
                                Set_Output_UV = ((goutValarm[4]-0x30)*100)+((goutValarm[5]-0x30)*10)+((goutValarm[6]-0x30)*1)+((goutValarm[7]-0x30)*0.1);
                                Set_VRef = ((battVoltSet[0]-0x30)*100)+((battVoltSet[1]-0x30)*10)+((battVoltSet[2]-0x30)*1)+((battVoltSet[3]-0x30)*0.1);
                                                                
                                Set_OK = 1;
                            }
                            else poss_x++;
						}
					}
                    if(esc) modi = LO;
					key_clr();
				}
			}break; 
			default:
			break;
		}
	}
}

void digit_disp()
{
	unsigned char data_8,data_cal,x_cnt,y_cnt=0;
	unsigned char cnt1,cnt2;//,/cnt3;
	unsigned int dg[12];//,dg10,dg100,dg1000;
	
	switch(iMenuIndex)
	{
		case(1):{
			dg[0] = ((Voltage_Out*10)/1000);
			dg[1] = (((Voltage_Out*10)-(dg[0]*1000))/100);
			dg[2] = (((Voltage_Out*10)-(dg[0]*1000))-(dg[1]*100))/10;
			dg[3] = ((Voltage_Out*10)-(dg[0]*1000))-(dg[1]*100)-(dg[2]*10);
			dg[4] = ((Current_Out*10)/1000);
			dg[5] = (((Current_Out*10)-(dg[4]*1000))/100);
			dg[6] = (((Current_Out*10)-(dg[4]*1000))-(dg[5]*100))/10;
			dg[7] = ((Current_Out*10)-(dg[4]*1000))-(dg[5]*100)-(dg[6]*10);
			for(x_cnt=0;x_cnt<8;x_cnt++)
			{								
				if(x_cnt >= 4) lcd_poss(((x_cnt+9)-4)*8,6);
				else lcd_poss((x_cnt+9)*8,4);
				data_cal = dg[x_cnt]+0x30;
				for(cnt1=0;cnt1<4;cnt1++)												
				{	
					data_8 = CHAR1[(data_cal-0x20)][cnt1];
					if(((x_cnt == 3) || (x_cnt == 7)) && (cnt1 == 0)) data_8 |= 0x60;
					lcd_inst(LO,HI,data_8);
				}
				for(cnt2=4;cnt2<8;cnt2++)												
				{	
					data_8 = CHAR1[(data_cal-0x20)][cnt2];
					if((x_cnt == 3) && (cnt2 == 0)) data_8 |= 0x60;
					lcd_inst(LO,HI,data_8);
				}
			}
		}break;
		case(2):{
			dg[0] = ((ACV_R)/100);
			dg[1] = (((ACV_R)-(dg[0]*100))/10);
			dg[2] = (((ACV_R)-(dg[0]*100))-(dg[1]*10));
			dg[3] = ((ACV_S)/100);
			dg[4] = (((ACV_S)-(dg[3]*100))/10);
			dg[5] = (((ACV_S)-(dg[3]*100))-(dg[4]*10));
			dg[6] = ((ACV_T)/100);
			dg[7] = (((ACV_T)-(dg[6]*100))/10);
			dg[8] = (((ACV_T)-(dg[6]*100))-(dg[7]*10));
			
			for(cnt2=0;cnt2<3;cnt2++) {									// R-phase volt
				lcd_poss((cnt2+4)*8,4);
				data_cal = dg[cnt2]+0x30;					
				lcd_put_char(data_cal);
			}
			//-----------------------------------------
			for(cnt2=0;cnt2<3;cnt2++) {									// S-phase volt
				lcd_poss((cnt2+8)*8,4);
				data_cal = dg[3+cnt2]+0x30;
				lcd_put_char(data_cal);
			}
			//-----------------------------------------
			for(cnt2=0;cnt2<3;cnt2++) {									// T-phase volt
				lcd_poss((cnt2+12)*8,4);
				data_cal = dg[6+cnt2]+0x30;
				lcd_put_char(data_cal);
			}
		}break;
		case(3):case(18):{
			if(!modi) {	
				rtc_get_date();
				rtc_get_time();
				timeBuf[0]=((year)/10+0x30);
				timeBuf[1]=(year%10)+0x30;
				timeBuf[2]=((mth)/10+0x30);
				timeBuf[3]=(mth%10)+0x30;
				timeBuf[4]=((day)/10+0x30);
				timeBuf[5]=(day%10)+0x30;
				timeBuf[6]=((hour)/10+0x30);
				timeBuf[7]=(hour%10)+0x30;
				timeBuf[8]=((min)/10+0x30);
				timeBuf[9]=(min%10)+0x30;
				timeBuf[10]=((sec)/10+0x30);
				timeBuf[11]=(sec%10)+0x30;

				//
				for(cnt1=0;cnt1<16;cnt1++)
				{
					switch(cnt1)
					{
                        // year
						case 0:	x_cnt = 6;y_cnt = 4;data_8 = timeBuf[0];break;
						case 1: x_cnt = 7;y_cnt = 4;data_8 = timeBuf[1];break;
						//
						case 2:	x_cnt = 8;y_cnt = 4;data_8 = '-';break;
						// 12-month
						case 3: x_cnt = 9;y_cnt = 4;data_8 = timeBuf[2];break;
						case 4: x_cnt = 10;y_cnt = 4;data_8 = timeBuf[3];break;
						// 
						case 5: x_cnt = 11;y_cnt = 4;data_8 = '-';break;
						// 31-day
						case 6:	x_cnt = 12;y_cnt = 4;data_8 = timeBuf[4];break;
						case 7: x_cnt = 13;y_cnt = 4;data_8 = timeBuf[5];break;
						// 23-hour
						case 8:	x_cnt = 6;y_cnt = 6;data_8 = timeBuf[6];break;
						case 9: x_cnt = 7;y_cnt = 6;data_8 = timeBuf[7];break;
						//
						case 10: x_cnt = 8;y_cnt = 6;data_8 = ':';break;
						// 17-minute
						case 11:	x_cnt = 9;y_cnt = 6;data_8 = timeBuf[8];break;
						case 12: x_cnt = 10;y_cnt = 6;data_8 = timeBuf[9];break;
						// 
						case 13: x_cnt = 11;y_cnt = 6;data_8 = ':';break;
						// 30-sec
						case 14:	x_cnt = 12;y_cnt = 6;data_8 = timeBuf[10];break;
						case 15: x_cnt = 13;y_cnt = 6;data_8 = timeBuf[11];break;
					}
					lcd_poss(x_cnt*8,y_cnt);
					lcd_put_char(data_8);
				}
			}
			else {
				for(cnt1=0;cnt1<16;cnt1++) {
					switch(cnt1) {
						// year
						case 0: x_cnt = 6;y_cnt = 4;data_8 = timeBuf[0];break;
						case 1: x_cnt = 7;y_cnt = 4;data_8 = timeBuf[1];break;
						//
						case 2: x_cnt = 8;y_cnt = 4;data_8 = '-';break;
						// month
						case 3: x_cnt = 9;y_cnt = 4;data_8 = timeBuf[2];break;
						case 4: x_cnt = 10;y_cnt = 4;data_8 = timeBuf[3];break;
						// 
						case 5: x_cnt = 11;y_cnt = 4;data_8 = '-';break;
						// day
						case 6: x_cnt = 12;y_cnt = 4;data_8 = timeBuf[4];break;
						case 7: x_cnt = 13;y_cnt = 4;data_8 = timeBuf[5];break;
						// hour
						case 8: x_cnt = 6;y_cnt = 6;data_8 = timeBuf[6];break;
						case 9: x_cnt = 7;y_cnt = 6;data_8 = timeBuf[7];break;
						//
						case 10: x_cnt = 8;y_cnt = 6;data_8 = ':';break;
						// minute
						case 11: x_cnt = 9;y_cnt = 6;data_8 = timeBuf[8];break;
						case 12: x_cnt = 10;y_cnt = 6;data_8 = timeBuf[9];break;
						// 
						case 13: x_cnt = 11;y_cnt = 6;data_8 = ':';break;
						// sec
						case 14: x_cnt = 12;y_cnt = 6;data_8 = timeBuf[10];break;
						case 15: x_cnt = 13;y_cnt = 6;data_8 = timeBuf[11];break;
					}
					lcd_poss(x_cnt*8,y_cnt);
					for(cnt2=0;cnt2<8;cnt2++) {
						data_cal = CHAR1[(data_8-0x20)][cnt2];
						if(cnt1 == bar_poss) data_cal |= 0x80;
						else  data_cal &= 0x7f;
						lcd_inst(LO,HI,data_cal);
					}
				}
			}
		}break;
		//
		case(4): {
			dg[0]=((Temperature*10)/1000);
			dg[1]=(((Temperature*10)-(dg[0]*1000))/100);
			dg[2]=(((Temperature*10)-(dg[0]*1000))-(dg[1]*100))/10;
			dg[3]=((Temperature*10)-(dg[0]*1000))-(dg[1]*100)-(dg[2]*10);
			for(x_cnt=0;x_cnt<4;x_cnt++)
			{								
				lcd_poss((x_cnt+9)*8,4);
				data_cal = dg[x_cnt]+0x30;
				for(cnt1=0;cnt1<8;cnt1++)												
				{	
					data_8 = CHAR1[(data_cal-0x20)][cnt1];
					if((x_cnt == 3) && (cnt1 == 0)) data_8 |= 0x60;
					lcd_inst(LO,HI,data_8);
				}
			}
		}break;
		//
		case(5): {
				if(!modi) {
					for(x_cnt=5;x_cnt<11;x_cnt++) {
						lcd_poss(x_cnt*8,4);
						lcd_put_char('*');
					}
				}
				else {
					for(x_cnt=5;x_cnt<11;x_cnt++) {
						lcd_poss(x_cnt*8,4);
						data_cal = temp_buffer[x_cnt-5];
						data_cal -= 0x20;
						if((x_cnt-5) == poss_x) {
							for(cnt1=0;cnt1<8;cnt1++) {
								data_8 = CHAR1[data_cal][cnt1];
								if(cnt1 == 0) data_8 &= 0x7f;
								else data_8 |= 0x80;
								lcd_inst(LO,HI,data_8);
							}
						}
						else {
							for(cnt1=0;cnt1<8;cnt1++) {
								data_8 = CHAR1[data_cal][cnt1];
								lcd_inst(LO,HI,data_8);
							}
						}						
					}
				}
		}break;
		        
        case(9): {
                if(!modi) {	//Just View Display				
					for(x_cnt=0;x_cnt<4;x_cnt++)
					{								
						lcd_poss((x_cnt+9)*8,4);
						battVoltSet[x_cnt] = read_EE(BATTVOLT_0+x_cnt);
						data_cal = battVoltSet[x_cnt];				
						//
						for(cnt1=0;cnt1<8;cnt1++)												
						{	
							data_8 = CHAR1[(data_cal-0x20)][cnt1];									  
							if((x_cnt == 3)  && (cnt1 == 0)) data_8 |= 0x60;
							lcd_inst(LO,HI,data_8);
						}
					}
				}
				else { 		//modifing Display//
					for(x_cnt=0;x_cnt<4;x_cnt++)
					{
						lcd_poss((x_cnt+9)*8,4);
						data_cal = battVoltSet[x_cnt];						
						for(cnt1=0;cnt1<8;cnt1++)
						{
							data_8 = CHAR1[(data_cal-0x20)][cnt1];
							if(x_cnt == poss_x) data_8 |= 0x80;
							else data_8 &= 0x7f;
							if((x_cnt == 3) && (cnt1 == 0)) data_8 |= 0x60;
							lcd_inst(LO,HI,data_8);
						}
					}
				}
		}break; 
        
		case(10): 
        {
                if(!modi) 
                {	//Just View Display				
					for(x_cnt=0;x_cnt<8;x_cnt++)
					{								
						if(x_cnt >= 4) lcd_poss(((x_cnt+9)-4)*8,6);
						else lcd_poss((x_cnt+9)*8,4);
						battVoltSet[x_cnt] = read_EE(BATTVOLT_0+x_cnt);
						data_cal = battVoltSet[x_cnt];
						//
						for(cnt1=0;cnt1<4;cnt1++)										
						{	
							data_8 = CHAR1[(data_cal-0x20)][cnt1];
							if(((x_cnt == 3) || (x_cnt == 7)) && (cnt1 == 0)) data_8 |= 0x60; //0x0110-0000//				
							lcd_inst(LO,HI,data_8);
						}
						for(cnt2=4;cnt2<8;cnt2++)												
						{	
							data_8 = CHAR1[(data_cal-0x20)][cnt2];
							if(((x_cnt == 3) || (x_cnt == 7)) && (cnt2 == 0)) data_8 |= 0x60; //0x0110-0000//
							lcd_inst(LO,HI,data_8);
						}
					}
				}
				else 
                {
					for(x_cnt=0;x_cnt<8;x_cnt++)
					{
						if(x_cnt >= 4) lcd_poss(((x_cnt+9)-4)*8,6);
						else lcd_poss((x_cnt+9)*8,4);
						data_cal = battVoltSet[x_cnt];						
						for(cnt1=0;cnt1<8;cnt1++)
						{
							data_8 = CHAR1[(data_cal-0x20)][cnt1];
							if(x_cnt == poss_x) data_8 |= 0x80;
							else data_8 &= 0x7f;
							if(((x_cnt == 3) || (x_cnt == 7)) && (cnt1 == 0)) data_8 |= 0x60;
							lcd_inst(LO,HI,data_8);
						}
					}
				}
		}break;
		//
        case(11): {
				if(!modi)
                {
					hex16_4digt(hex1_16);
					outSetNew[0] = thnd;outSetNew[1] = hund;
					outSetNew[2] = ten;outSetNew[3] = one;
					for(cnt1=0;cnt1<4;cnt1++) {
						lcd_poss((cnt1+9)*8,4);
						data_cal = outSetNew[cnt1];
						for(cnt2=0;cnt2<8;cnt2++) {
							data_8 = CHAR1[(data_cal-0x20)][cnt2];
							if((cnt1 == 3) && (cnt2 == 0)) data_8 |= 0x60;
							lcd_inst(LO,HI,data_8);
						}
					}
					//
					for(cnt1=4;cnt1<7;cnt1++) 
                    {
						lcd_poss((cnt1+9-3)*8,6);
						LoadAmpSet[cnt1-4] = read_EE(LOADAMP_0+cnt1-4);
						data_8 = LoadAmpSet[cnt1-4];
						lcd_put_char(data_8);
					}
				}
				else 
                {
					hex16_4digt(hex1_16);
					outSetNew[0] = thnd;outSetNew[1] = hund;
					outSetNew[2] = ten;outSetNew[3] = one;
					for(cnt1=0;cnt1<4;cnt1++) 
                    {
						lcd_poss((cnt1+9)*8,4);
						data_cal = outSetNew[cnt1];
						for(cnt2=0;cnt2<8;cnt2++) 
                        {
							data_8 = CHAR1[(data_cal-0x20)][cnt2];
							if(cnt1 == poss_x) data_8 |= 0x80;
							else  data_8 &= 0x7f;
							if((cnt1 == 3) && (cnt2 == 0)) data_8 |= 0x60;
							lcd_inst(LO,HI,data_8);
						}
					}
					//
																					    
					for(cnt1=4;cnt1<7;cnt1++) 
                    {
						lcd_poss((cnt1+9-3)*8,6);
						data_cal = LoadAmpSet[cnt1-4];
						
						for(cnt2=0;cnt2<8;cnt2++) {
							data_8 = CHAR1[(data_cal-0x20)][cnt2];
							if(cnt1 == poss_x) data_8 |= 0x80;
							else  data_8 &= 0x7f;
							lcd_inst(LO,HI,data_8);
						}
					}
				}
		}break;
		//
		case(12): {
				if(!modi) {
					for(x_cnt=0;x_cnt<3;x_cnt++) {
						lcd_poss((x_cnt+9+1)*8,4);
						battAmpSet[x_cnt] = read_EE(BATTAMP_0+x_cnt);
						data_cal = battAmpSet[x_cnt];
						for(cnt1=0;cnt1<8;cnt1++)												
						{	
							data_8 = CHAR1[(data_cal-0x20)][cnt1];
							lcd_inst(LO,HI,data_8);
						}
					}
				}
				else {																					    
					for(x_cnt=0;x_cnt<3;x_cnt++) {
						lcd_poss((x_cnt+9+1)*8,4);
						data_cal = battAmpSet[x_cnt];
						for(cnt1=0;cnt1<8;cnt1++) {
							data_8 = CHAR1[(data_cal-0x20)][cnt1];
							if(x_cnt == poss_x) data_8 |= 0x80;
							else  data_8 &= 0x7f;
							lcd_inst(LO,HI,data_8);
						}
					}
				}
		}break;
		//
		case(13): {
				if(!modi) {	//Just View Display				
					for(x_cnt=0;x_cnt<8;x_cnt++)
					{								
						if(x_cnt >= 4) lcd_poss(((x_cnt+9)-4)*8,6);
						else lcd_poss((x_cnt+9)*8,4);
						ginValarm[x_cnt] = read_EE(INVALARM_0+x_cnt);
						data_cal = ginValarm[x_cnt];				
						//
						for(cnt1=0;cnt1<4;cnt1++)												
						{	
							data_8 = CHAR1[(data_cal-0x20)][cnt1];
							if(((x_cnt == 3) || (x_cnt == 7)) && (cnt1 == 0)) data_8 |= 0x60; //0x0110-0000//				
							lcd_inst(LO,HI,data_8);
						}
						for(cnt2=4;cnt2<8;cnt2++)												
						{	
							data_8 = CHAR1[(data_cal-0x20)][cnt2];
							if(((x_cnt == 3) || (x_cnt == 7)) && (cnt2 == 0)) data_8 |= 0x60; //0x0110-0000//
							lcd_inst(LO,HI,data_8);
						}
					}
				}
				else {
					for(x_cnt=0;x_cnt<8;x_cnt++)
					{
						if(x_cnt >= 4) lcd_poss(((x_cnt+9)-4)*8,6);
						else lcd_poss((x_cnt+9)*8,4);
						data_cal = ginValarm[x_cnt];						
						for(cnt1=0;cnt1<8;cnt1++)
						{
							data_8 = CHAR1[(data_cal-0x20)][cnt1];
							if(x_cnt == poss_x) data_8 |= 0x80;
							else data_8 &= 0x7f;
							if(((x_cnt == 3) || (x_cnt == 7)) && (cnt1 == 0)) data_8 |= 0x60;
							lcd_inst(LO,HI,data_8);
						}
					}
				}
		}break;
		//
		case(14): {
				if(!modi) {	//Just View Display				
					for(x_cnt=0;x_cnt<8;x_cnt++)
					{								
						if(x_cnt >= 4) lcd_poss(((x_cnt+9)-4)*8,6);
						else lcd_poss((x_cnt+9)*8,4);
						goutValarm[x_cnt] = read_EE(OUTVALARM_0+x_cnt);
						data_cal = goutValarm[x_cnt];				
						//
						for(cnt1=0;cnt1<4;cnt1++)												
						{	
							data_8 = CHAR1[(data_cal-0x20)][cnt1];
							if(((x_cnt == 3) || (x_cnt == 7)) && (cnt1 == 0)) data_8 |= 0x60; //0x0110-0000//				
							lcd_inst(LO,HI,data_8);
						}
						for(cnt2=4;cnt2<8;cnt2++)												
						{	
							data_8 = CHAR1[(data_cal-0x20)][cnt2];
							if(((x_cnt == 3) || (x_cnt == 7)) && (cnt2 == 0)) data_8 |= 0x60; //0x0110-0000//
							lcd_inst(LO,HI,data_8);
						}
					}
				}
				else {
					for(x_cnt=0;x_cnt<8;x_cnt++)
					{
						if(x_cnt >= 4) lcd_poss(((x_cnt+9)-4)*8,6);
						else lcd_poss((x_cnt+9)*8,4);
						data_cal = goutValarm[x_cnt];						
						for(cnt1=0;cnt1<8;cnt1++)
						{
							data_8 = CHAR1[(data_cal-0x20)][cnt1];
							if(x_cnt == poss_x) data_8 |= 0x80;
							else data_8 &= 0x7f;
							if(((x_cnt == 3) || (x_cnt == 7)) && (cnt1 == 0)) data_8 |= 0x60;
							lcd_inst(LO,HI,data_8);
						}
					}
				}
		}break;
		//
		case(15): {
				if(!modi) {	//Just View Display				
					for(x_cnt=0;x_cnt<3;x_cnt++)
					{								
						lcd_poss((x_cnt+9+1)*8,4);
						goutAalarm[x_cnt] = read_EE(OUTAALARM_0+x_cnt);
						data_cal = goutAalarm[x_cnt];				
						//
						for(cnt1=0;cnt1<8;cnt1++)												
						{	
							data_8 = CHAR1[(data_cal-0x20)][cnt1];
							lcd_inst(LO,HI,data_8);
						}
					}
				}
				else {
					for(x_cnt=0;x_cnt<3;x_cnt++)
					{
						lcd_poss((x_cnt+9+1)*8,4);
						data_cal = goutAalarm[x_cnt];						
						for(cnt1=0;cnt1<8;cnt1++)
						{
							data_8 = CHAR1[(data_cal-0x20)][cnt1];
							if(x_cnt == poss_x) data_8 |= 0x80;
							else data_8 &= 0x7f;
							lcd_inst(LO,HI,data_8);
						}
					}
				}
		}break;
		//
		case(16): {
				if(!modi) {	//Just View Display				
					for(x_cnt=0;x_cnt<8;x_cnt++)
					{								
						if(x_cnt >= 4) lcd_poss(((x_cnt+9)-4)*8,6);
						else lcd_poss((x_cnt+9)*8,4);
						shutDownOutV[x_cnt] = read_EE(SHUTDOWNOUTV_0+x_cnt);
						data_cal = shutDownOutV[x_cnt];				
						//
						for(cnt1=0;cnt1<4;cnt1++)												
						{	
							data_8 = CHAR1[(data_cal-0x20)][cnt1];
							if(((x_cnt == 3) || (x_cnt == 7)) && (cnt1 == 0)) data_8 |= 0x60; //0x0110-0000//				
							lcd_inst(LO,HI,data_8);
						}
						for(cnt2=4;cnt2<8;cnt2++)												
						{	
							data_8 = CHAR1[(data_cal-0x20)][cnt2];
							if(((x_cnt == 3) || (x_cnt == 7)) && (cnt2 == 0)) data_8 |= 0x60; //0x0110-0000//
							lcd_inst(LO,HI,data_8);
						}
					}
				}
				else {
					for(x_cnt=0;x_cnt<8;x_cnt++)
					{
						if(x_cnt >= 4) lcd_poss(((x_cnt+9)-4)*8,6);
						else lcd_poss((x_cnt+9)*8,4);
						data_cal = shutDownOutV[x_cnt];						
						for(cnt1=0;cnt1<8;cnt1++)
						{
							data_8 = CHAR1[(data_cal-0x20)][cnt1];
							if(x_cnt == poss_x) data_8 |= 0x80;
							else data_8 &= 0x7f;
							if(((x_cnt == 3) || (x_cnt == 7)) && (cnt1 == 0)) data_8 |= 0x60;
							lcd_inst(LO,HI,data_8);
						}
					}
				}
		}break;
		//
		case(17): {
			if(!modi) {
				data_8 = read_EE(ALARM_STOP);
				if(data_8 == 'Y') {										// buzzer en/disable display
					for(x_cnt=0;x_cnt<3;x_cnt++) {
						lcd_poss((x_cnt+9)*8,4);
						data_8 = BUZZER[0][x_cnt];
						lcd_put_char(data_8);
					}
				}
				else {
					for(x_cnt=0;x_cnt<3;x_cnt++) {
						lcd_poss((x_cnt+9)*8,4);
						data_8 = BUZZER[1][x_cnt];
						lcd_put_char(data_8);
					}
				}
			}
			else {
				for(x_cnt=0;x_cnt<3;x_cnt++) {
					lcd_poss((x_cnt+9)*8,4);
					if(fAlarmStop == 'Y') data_8 = BUZZER[0][x_cnt];
					else data_8 = BUZZER[1][x_cnt];
					for(cnt1=0;cnt1<8;cnt1++) {
						data_cal = CHAR1[(data_8-0x20)][cnt1];
						if(poss_x == 0) data_cal |= 0x80;								
						lcd_inst(LO,HI,data_cal);
					}
				}
					}
		}break;
		//
		case(19): {
				if(!modi) {
					for(cnt1=0;cnt1<6;cnt1++) {
						lcd_poss((cnt1+6)*8,4);
						data_8 = read_EE(PASSWD_0+cnt1);
						lcd_put_char(data_8);
						lcd_poss((cnt1+6)*8,6);
						lcd_put_char('0');
					}
				}
				else {
					for(cnt1=0;cnt1<6;cnt1++) {
						lcd_poss((cnt1+6)*8,4);
						data_8 = read_EE(PASSWD_0+cnt1);
						lcd_put_char(data_8);
						lcd_poss((cnt1+6)*8,6);
						data_8 = gPin[cnt1];
						for(cnt2=0;cnt2<8;cnt2++) {
							data_cal = CHAR1[(data_8-0x20)][cnt2];
							if(cnt1 == poss_x) data_cal |= 0x80;
							else data_cal &= 0x7f;
							lcd_inst(LO,HI,data_cal);
						}
					}
				}
			}break;
			//
		case(20): {
				if(!modi) {
					for(x_cnt=0;x_cnt<3;x_cnt++) {
						lcd_poss((x_cnt+7)*8,4);
						data_8 = INIT_SYS[0][x_cnt];
						lcd_put_char(data_8);
					}
				}
				else {
					for(x_cnt=0;x_cnt<3;x_cnt++) {
						lcd_poss((x_cnt+7)*8,4);
						if(temp_buffer[poss_x] == 'Y') data_8 = INIT_SYS[1][x_cnt];
						else data_8 = INIT_SYS[0][x_cnt];
						for(cnt1=0;cnt1<8;cnt1++) {
							data_cal = CHAR1[(data_8-0x20)][cnt1];
							if(poss_x == 0) data_cal |= 0x80;								
							lcd_inst(LO,HI,data_cal);
						}
					}
				}
			}break;
			
        case(21): 
        {
            if(!modi) 
            {
                ADJ_VGain = read_EE(ADJVGain_0);
                ADJ_CGain = read_EE(ADJCGain_0);
                if(ADJ_VGain >= 0x80)
                {
                    lcd_poss(72,4);
                    data_8 = '-';
                    lcd_put_char(data_8);
                    hex1_8=256-ADJ_VGain;
                    hex16_3digt(hex1_8);
                    gain_buffer[0] = hund;
                    gain_buffer[1] = ten;
                    gain_buffer[2] = one;
                    for(cnt1=0;cnt1<3;cnt1++) 
                    {
                        lcd_poss((cnt1+10)*8,4);
                        data_8 = gain_buffer[cnt1];
                        lcd_put_char(data_8);
                    }
                }
                else
                {
                    hex1_8=ADJ_VGain;
                    hex16_3digt(hex1_8);
                    gain_buffer[0] = hund;
                    gain_buffer[1] = ten;
                    gain_buffer[2] = one;
                    for(cnt1=0;cnt1<3;cnt1++) 
                    {
                        lcd_poss((cnt1+10)*8,4);
                        data_8 = gain_buffer[cnt1];
                        lcd_put_char(data_8);
                    }
                }
                //
                if(ADJ_CGain >= 0x80)
                {
                    lcd_poss(72,6);
                    data_8 = '-';
                    lcd_put_char(data_8);
                    hex1_8=256-ADJ_CGain;
                    hex16_3digt(hex1_8);
                    gain_buffer[3] = hund;
                    gain_buffer[4] = ten;
                    gain_buffer[5] = one;																    
                    for(cnt1=3;cnt1<6;cnt1++) 
                    {
                        lcd_poss((cnt1+7)*8,6);
                        data_8 = gain_buffer[cnt1];						
                        lcd_put_char(data_8);
                    }
                }
                else
                {
                    hex1_8=ADJ_CGain;
                    hex16_3digt(hex1_8);
                    gain_buffer[3] = hund;
                    gain_buffer[4] = ten;
                    gain_buffer[5] = one;																    
                    for(cnt1=3;cnt1<6;cnt1++) 
                    {
                        lcd_poss((cnt1+7)*8,6);
                        data_8 = gain_buffer[cnt1];						
                        lcd_put_char(data_8);
                    }
                }
            }
            else 
            {
                if(ADJ_VGain >= 0x80)
                {
                    hex1_8=256-ADJ_VGain;
                    hex16_3digt(hex1_8);
                    gain_buffer[0] = hund;
                    gain_buffer[1] = ten;
                    gain_buffer[2] = one;
                    for(cnt1=0;cnt1<3;cnt1++) {
                        lcd_poss((cnt1+10)*8,4);
                        data_cal = gain_buffer[cnt1];
                        for(cnt2=0;cnt2<8;cnt2++) {
                            data_8 = CHAR1[(data_cal-0x20)][cnt2];
                            if(cnt1 == poss_x) data_8 |= 0x80;
                            else  data_8 &= 0x7f;
                            lcd_inst(LO,HI,data_8);
                        }
                    }
                    lcd_poss(72,4);
                    data_cal = '-';
                    for(cnt2=0;cnt2<8;cnt2++) {
                        data_8 = CHAR1[(data_cal-0x20)][cnt2];
                        if(poss_x == 0) data_cal |= 0x80;
                        lcd_inst(LO,HI,data_8);
                    }
                }
                else
                {
                    hex1_8=ADJ_VGain;
                    hex16_3digt(hex1_8);
                    gain_buffer[0] = hund;
                    gain_buffer[1] = ten;
                    gain_buffer[2] = one;
                    for(cnt1=0;cnt1<3;cnt1++) {
                        lcd_poss((cnt1+10)*8,4);
                        data_cal = gain_buffer[cnt1];
                        for(cnt2=0;cnt2<8;cnt2++) {
                            data_8 = CHAR1[(data_cal-0x20)][cnt2];
                            if(cnt1 == poss_x) data_8 |= 0x80;
                            else  data_8 &= 0x7f;
                            lcd_inst(LO,HI,data_8);
                        }
                    }
                }
                //
                if(ADJ_CGain >= 0x80)
                {
                    hex1_8=256-ADJ_CGain;
                    hex16_3digt(hex1_8);
                    gain_buffer[3] = hund;
                    gain_buffer[4] = ten;
                    gain_buffer[5] = one;																    
                    for(cnt1=3;cnt1<6;cnt1++) {
                        lcd_poss((cnt1+7)*8,6);
                        data_cal = gain_buffer[cnt1];						
                        for(cnt2=0;cnt2<8;cnt2++) {
                            data_8 = CHAR1[(data_cal-0x20)][cnt2];
                            if(cnt1 == poss_x) data_8 |= 0x80;
                            else  data_8 &= 0x7f;
                            lcd_inst(LO,HI,data_8);
                        }
                    }
                    lcd_poss(72,6);
                    data_cal = '-';
                    for(cnt2=0;cnt2<8;cnt2++) {
                        data_8 = CHAR1[(data_cal-0x20)][cnt2];
                        if(poss_x == 0) data_cal |= 0x80;
                        lcd_inst(LO,HI,data_8);
                    }
                }
                else
                {
                    hex1_8=ADJ_CGain;
                    hex16_3digt(hex1_8);
                    gain_buffer[3] = hund;
                    gain_buffer[4] = ten;
                    gain_buffer[5] = one;																    
                    for(cnt1=3;cnt1<6;cnt1++) {
                        lcd_poss((cnt1+7)*8,6);
                        data_cal = gain_buffer[cnt1];						
                        for(cnt2=0;cnt2<8;cnt2++) {
                            data_8 = CHAR1[(data_cal-0x20)][cnt2];
                            if(cnt1 == poss_x) data_8 |= 0x80;
                            else  data_8 &= 0x7f;
                            lcd_inst(LO,HI,data_8);
                        }
                    }
                }
            }
        }break;
        case(22): 
        {
            if(!modi) 
            {
                poss_x = read_EE(CHG_STS_0);
                poss_y = read_EE(CHG_STS_1);
                for(x_cnt=0;x_cnt<4;x_cnt++) 
                {
                    if(poss_x == 0)
                    {
                        lcd_poss((x_cnt+1)*8,5);
                    }
                    else if(poss_x == 1 && poss_y == 1)
                    {
                        lcd_poss((x_cnt+6)*8,5);
                    }
                    else if(poss_x == 1 && poss_y == 0)
                    {
                        lcd_poss((x_cnt+11)*8,5);
                    }
                    else
                    {
                        lcd_poss((x_cnt+11)*8,6);
                    }
                    data_8 = MODE_SEL[0];
                    lcd_put_char(data_8);
                }
            }
            else 
            {
                for(x_cnt=0;x_cnt<4;x_cnt++) 
                {
                    if(poss_x == 0)
                    {
                        lcd_poss((x_cnt+1)*8,5);
                    }
                    else if(poss_x == 1 && poss_y == 1)
                    {
                        lcd_poss((x_cnt+6)*8,5);
                    }
                    else if(poss_x == 1 && poss_y == 0)
                    {
                        lcd_poss((x_cnt+11)*8,5);
                    }
                    else
                    {
                        lcd_poss((x_cnt+11)*8,6);
                    }
                    data_8 = MODE_SEL[0];
                    for(cnt1=0;cnt1<8;cnt1++) 
                    {
                        data_cal = CHAR1[(data_8-0x20)][cnt1];
                        data_cal |= 0x80;								
                        lcd_inst(LO,HI,data_cal);
                    }
                }
            }
        }break;

        case(23):
        {
            if(!modi) 
            {
                Set_time[0] = read_EE(CHG_SET_NO) + 0x30;
                Set_time[1] = (read_EE(SET_YEAR_0 + ((Set_time[0]-0x30)*6))/10) + 0x30;
                Set_time[2] = (read_EE(SET_YEAR_0 + ((Set_time[0]-0x30)*6))%10) + 0x30;
                Set_time[3] = (read_EE(SET_MONTH_0 + ((Set_time[0]-0x30)*6))/10) + 0x30;
                Set_time[4] = (read_EE(SET_MONTH_0 + ((Set_time[0]-0x30)*6))%10) + 0x30;
                Set_time[5] = (read_EE(SET_DAY_0 + ((Set_time[0]-0x30)*6))/10) + 0x30;
                Set_time[6] = (read_EE(SET_DAY_0 + ((Set_time[0]-0x30)*6))%10) + 0x30;
                Set_time[7] = (read_EE(SET_HOUR_0 + ((Set_time[0]-0x30)*6))/10) + 0x30;
                Set_time[8] = (read_EE(SET_HOUR_0 + ((Set_time[0]-0x30)*6))%10) + 0x30;
                Set_time[9] = (read_EE(SET_MIN_0 + ((Set_time[0]-0x30)*6))/10) + 0x30;
                Set_time[10] = (read_EE(SET_MIN_0 + ((Set_time[0]-0x30)*6))%10) + 0x30;
                Set_time[11] = (read_EE(SET_SEC_0 + ((Set_time[0]-0x30)*6))/10) + 0x30;
                Set_time[12] = (read_EE(SET_SEC_0 + ((Set_time[0]-0x30)*6))%10) + 0x30;

                for(cnt1=0;cnt1<13;cnt1++)
                {
                    switch(cnt1)
                    {
                        case 0: x_cnt = 6;y_cnt = 3;data_8 = Set_time[0];break;  

                        // year
                        case 1: x_cnt = 6;y_cnt = 5;data_8 = Set_time[1];break;
                        case 2: x_cnt = 7;y_cnt = 5;data_8 = Set_time[2];break;

                        // 12-month
                        case 3: x_cnt = 9;y_cnt = 5;data_8 = Set_time[3];break;
                        case 4: x_cnt = 10;y_cnt = 5;data_8 = Set_time[4];break;

                        // 31-day
                        case 5: x_cnt = 12;y_cnt = 5;data_8 = Set_time[5];break;
                        case 6: x_cnt = 13;y_cnt = 5;data_8 = Set_time[6];break;

                        // 23-hour
                        case 7: x_cnt = 6;y_cnt = 6;data_8 = Set_time[7];break;
                        case 8: x_cnt = 7;y_cnt = 6;data_8 = Set_time[8];break;

                        // 17-minute
                        case 9: x_cnt = 9;y_cnt = 6;data_8 = Set_time[9];break;
                        case 10: x_cnt = 10;y_cnt = 6;data_8 = Set_time[10];break;

                        // 30-sec
                        case 11: x_cnt = 12;y_cnt = 6;data_8 = Set_time[11];break;
                        case 12: x_cnt = 13;y_cnt = 6;data_8 = Set_time[12];break;   
                    }
                    lcd_poss(x_cnt*8,y_cnt);
                    lcd_put_char(data_8);
                }
            }       

            else 
            {
                for(cnt1=0;cnt1<13;cnt1++)
                {
                    switch(cnt1)
                    {
                        case 0: 
                        {
                            x_cnt = 6;
                            y_cnt = 3;
                            data_8 = Set_time[0]; 
                        }break; 
                        // year
                        case 1:
                        {
                            x_cnt = 6;
                            y_cnt = 5;
                            data_8 = Set_time[1];
                        }
                        break;
                        case 2:
                        {
                            x_cnt = 7;
                            y_cnt = 5;
                            data_8 = Set_time[2];
                        }
                        break;

                        // 12-month
                        case 3:
                        {
                            x_cnt = 9;
                            y_cnt = 5;
                            data_8 = Set_time[3];
                        }break;

                        case 4:
                        {
                            x_cnt = 10;
                            y_cnt = 5;
                            data_8 = Set_time[4];
                        }break;

                        // 31-day
                        case 5:
                        {
                            x_cnt = 12;
                            y_cnt = 5;
                            data_8 = Set_time[5];
                        }break;
                        case 6:
                        {
                            x_cnt = 13;
                            y_cnt = 5;
                            data_8 = Set_time[6];
                        }break;

                        // 23-hour
                        case 7: 
                        {
                            x_cnt = 6;
                            y_cnt = 6;
                            data_8 = Set_time[7];
                        }break;
                        case 8:
                        {
                            x_cnt = 7;
                            y_cnt = 6;
                            data_8 = Set_time[8];
                        }break;

                        // 17-minute
                        case 9:
                        {
                            x_cnt = 9;
                            y_cnt = 6;
                            data_8 = Set_time[9];
                        }break;
                        case 10:
                        {
                            x_cnt = 10;
                            y_cnt = 6;
                            data_8 = Set_time[10];
                        }break;

                        // 30-sec
                        case 11:
                        {
                            x_cnt = 12;
                            y_cnt = 6;
                            data_8 = Set_time[11];
                        }break;

                        case 12:
                        {
                            x_cnt = 13;
                            y_cnt = 6;
                            data_8 = Set_time[12];
                        }break;   
                    }
                    lcd_poss(x_cnt*8,y_cnt);
                    for(cnt2=0;cnt2<8;cnt2++) 
                    {
                        data_cal = CHAR1[(data_8-0x20)][cnt2];
                        if(cnt1 == poss_x) data_cal |= 0x80;
                        else  data_cal &= 0x7f;
                        lcd_inst(LO,HI,data_cal);
                    }
                }
            }         
        }break;	

        case(24): 
        {
            if(!modi) 
            {	//Just View Display				
                Set_COND[0] = read_EE(CHG_SET_TIME)/10 + 0x30;
                Set_COND[1] = read_EE(CHG_SET_TIME)%10 + 0x30;
                Set_COND[2] = read_EE(CHG_SET_CURR)/10 + 0x30;
                Set_COND[3] = read_EE(CHG_SET_CURR)%10 + 0x30;

                for(cnt1=0;cnt1<4;cnt1++)
                {
                    switch(cnt1)
                    {
                        // Charge Time
                        case 0: x_cnt = 11;y_cnt = 4;data_8 = Set_COND[0];break;  
                        case 1: x_cnt = 12;y_cnt = 4;data_8 = Set_COND[1];break;
                        // Chrage Current
                        case 2: x_cnt = 11;y_cnt = 6;data_8 = Set_COND[2];break;
                        case 3: x_cnt = 12;y_cnt = 6;data_8 = Set_COND[3];break;
                    }
                    lcd_poss(x_cnt*8,y_cnt);
                    lcd_put_char(data_8);
                }
            }
            else 
            {
                for(cnt1=0;cnt1<4;cnt1++)
                {
                    switch(cnt1)
                    {
                        case 0: 
                        {
                            x_cnt = 11;
                            y_cnt = 4;
                            data_8 = Set_COND[0]; 
                        }break; 
                        // year
                        case 1:
                        {
                            x_cnt = 12;
                            y_cnt = 4;
                            data_8 = Set_COND[1];
                        }
                        break;
                        case 2:
                        {
                            x_cnt = 11;
                            y_cnt = 6;
                            data_8 = Set_COND[2];
                        }
                        break;
                        case 3:
                        {
                            x_cnt = 12;
                            y_cnt = 6;
                            data_8 = Set_COND[3];
                        }break;
                    }
                    lcd_poss(x_cnt*8,y_cnt);
                    for(cnt2=0;cnt2<8;cnt2++) 
                    {
                        data_cal = CHAR1[(data_8-0x20)][cnt2];
                        if(cnt1 == poss_x) data_cal |= 0x80;
                        else  data_cal &= 0x7f;
                        lcd_inst(LO,HI,data_cal);
                    }
                }
            }
        }break;     
        
        //
        case(25): {
				if(!modi) {
					for(x_cnt=5;x_cnt<11;x_cnt++) {
						lcd_poss(x_cnt*8,4);
						lcd_put_char('*');
					}
				}
				else {
					for(x_cnt=5;x_cnt<11;x_cnt++) {
						lcd_poss(x_cnt*8,4);
						data_cal = temp_buffer[x_cnt-5];
						data_cal -= 0x20;
						if((x_cnt-5) == poss_x) {
							for(cnt1=0;cnt1<8;cnt1++) {
								data_8 = CHAR1[data_cal][cnt1];
								if(cnt1 == 0) data_8 &= 0x7f;
								else data_8 |= 0x80;
								lcd_inst(LO,HI,data_8);
							}
						}
						else {
							for(cnt1=0;cnt1<8;cnt1++) {
								data_8 = CHAR1[data_cal][cnt1];
								lcd_inst(LO,HI,data_8);
							}
						}						
					}
				}
		}break;
          
        case(26): {
			if(!modi) {
				data_8 = read_EE(OP_MODE);
				if(data_8 == 'Y') {										
					for(x_cnt=0;x_cnt<3;x_cnt++) {
						lcd_poss((x_cnt+9)*8,4);
						data_8 = OP_M[0][x_cnt];
						lcd_put_char(data_8);
					}
				}
				else {
					for(x_cnt=0;x_cnt<3;x_cnt++) {
						lcd_poss((x_cnt+9)*8,4);
						data_8 = OP_M[1][x_cnt];
						lcd_put_char(data_8);
					}
				}
			}
			else {
                    for(x_cnt=0;x_cnt<3;x_cnt++) {
					lcd_poss((x_cnt+9)*8,4);
					if(temp_buffer[0] == 'Y') data_8 = OP_M[0][x_cnt];
					else data_8 = OP_M[1][x_cnt];
					for(cnt1=0;cnt1<8;cnt1++) {
						data_cal = CHAR1[(data_8-0x20)][cnt1];
						if(poss_x == 0) data_cal |= 0x80;								
						lcd_inst(LO,HI,data_cal);
					}
				}
					}
		}break;
        
        case(27): 
        {
            // waiting Display
			if(!modi) 
            {
                //Input Voltage Select display 
                for(x_cnt=0;x_cnt<3;x_cnt++) 
                {
                    data_8 = read_EE(INPUT_VOLT);
                    if(data_8 == 0) 
                    {										
                        data_8 = IN_VOLT[0][x_cnt];
                        lcd_poss((x_cnt+9)*8,4);
                        lcd_put_char(data_8);
                    }
                    else 
                    {
                        data_8 = IN_VOLT[1][x_cnt];
                        lcd_poss((x_cnt+9)*8,4);                                    
                        lcd_put_char(data_8);
                    }
                }
                for(x_cnt=0;x_cnt<3;x_cnt++) 
                {
                    // Batt type select display
                    data_8 = read_EE(BATT_TYPE);
                    if(data_8 == 0) 
                    {										
                        data_8 = BATT_T[0][x_cnt];
                        lcd_poss((x_cnt+9)*8,6);                                    
                        lcd_put_char(data_8);
                    }
                    else if(data_8 == 1) 
                    {
                        data_8 = BATT_T[1][x_cnt];
                        lcd_poss((x_cnt+9)*8,6);                                    
                        lcd_put_char(data_8);
                    } 
                    else if(data_8 == 2) 
                    {
                        data_8 = BATT_T[2][x_cnt];
                        lcd_poss((x_cnt+9)*8,6);                                    
                        lcd_put_char(data_8);
                    }
                }
			}
			else 
            {
                for(x_cnt=0;x_cnt<3;x_cnt++) 
                {
                    //Input Voltage Select display  
                    lcd_poss((x_cnt+9)*8,4);
                    if(temp_buffer[0] == 0) data_8 = IN_VOLT[0][x_cnt];
                    else data_8 = IN_VOLT[1][x_cnt];
                    for(cnt1=0;cnt1<8;cnt1++) 
                    {
                        data_cal = CHAR1[(data_8-0x20)][cnt1];
                        if(poss_x == 0) data_cal |= 0x80;								
                        lcd_inst(LO,HI,data_cal);
                    }

                    //Battery Type Select display
                    lcd_poss((x_cnt+9)*8,6);
                    if(temp_buffer[1] == 0) data_8 = BATT_T[0][x_cnt];
                    else if(temp_buffer[1] == 1) data_8 = BATT_T[1][x_cnt];
                    else if(temp_buffer[1] == 2) data_8 = BATT_T[2][x_cnt];
                    for(cnt1=0;cnt1<8;cnt1++) 
                    {
                        data_cal = CHAR1[(data_8-0x20)][cnt1];
                        if(poss_x == 1) data_cal |= 0x80;								
                        lcd_inst(LO,HI,data_cal);
                    }
                }
            }
        }break;
        default:
        break;
	}
}

void key_clr()
{
	up = down = esc = enter = LO;
}

// LCD x-y loading function
void lcd_poss(unsigned char x_poss,unsigned char y_poss) 
{
    if(x_poss >= 64) {				
        CS2 = HI;cs_bit = HI;
    }
    else {				
        CS1 = HI;cs_bit = LO;
    }
    lcd_inst(LO,LO,(Y_LINE | y_poss));
    lcd_inst(LO,LO,(X_LINE | x_poss));
}//lcd_poss end

void lcd_inst(unsigned int RW,unsigned int DI,unsigned char lcd_data) 
{
    unsigned int buf;

    WDT = HI;_NOP();_NOP();_NOP();WDT = LO;
    R_W = RW;_NOP();_NOP();
    D_I = DI;_NOP();_NOP();
    if(cs_bit) CS2 = HI;
    else CS1 = HI;
    _NOP();_NOP();_NOP();
    buf=LCD_DATA;
    LCD_DATA=buf|(lcd_data&0xFF);

    _NOP();_NOP();_NOP();
    EN = HI;_NOP();_NOP();_NOP();
    EN = LO;_NOP();_NOP();
    CS1 = LO;CS2 = LO;R_W = LO;
    WDT = HI;
    LCD_DATA=buf&0xFF00;//buf|(a&0xFF);
}//lcd_inst end	



void lcd_put_char(unsigned char dchar)
{
	unsigned char	j,c;
	for(j=0;j<=7;j++) {
		
		c = CHAR1[dchar-0x20][j];
		lcd_inst(LO,HI,c);
	}
}//lcd_put_char end







