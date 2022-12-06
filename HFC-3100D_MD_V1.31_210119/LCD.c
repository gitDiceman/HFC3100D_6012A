#include 	<p30f6012A.h>		
#include 	"Definition.h"
#include	"LCD_fc.h"
#include	"LCD_df.h"
#include	"Function.h"
#include 	"eeprom.h"
#include	"RS232.h"

//====================================================================================//
//                          Function Declaration                                      //
//====================================================================================//
//RTC
void rtc_get_date();
void rtc_get_time();
void rtc_set_datetime(UINT8 day, UINT8 mth, UINT8 year, UINT8 dow, UINT8 hour, UINT8 min);
//====================================================================================//
//                          Variable Declaration                                      //
//====================================================================================//

//Global Variable
UINT8 gTmp=0;

//Flag Variable
UINT8 fTimeSet=0;
UINT8 fAlarmStop=0;

INT16 Temp_LCD_DATA;
UINT8 Temp_CMD;
UINT8 Str_Temp;
UINT8 iMenuIndex=0;
UINT8 iMenuCnt=0;

UINT16 Vvalue;
UINT16 Avalue;
UINT8 KeyValue;
//UINT8 uc_key=0;

INT8 inputCash=0;
UINT8 fPassSet=0;
UINT8 passWord[10]={'0','0','0','0','0','0','0','0','0','0'};
UINT8 SpassWord[10]={'1','1','1','1','1','1','1','1','1','1'};

//AlarmHistory
UINT8 gAlarmHistory[200][9];
UINT8 gAlarmHistoryCnt = 0;
UINT8 alarmList[38]={0,};
UINT8 gAlarmStatus[25] = {0,};
UINT8 gC_AlarmHistoryCnt=0;
UINT8 prev_alarm_name;
UINT8 ErrCode=0;
UINT8 ErrCnt;
UINT8 fCheckErr=0;
UINT8 fNfbCheckErr=0;
UINT8 gPacketsize = 0;

//RTC
UINT8 day,mth,year,dow,hour,min,sec,sec_old;

//LED STATUS
UINT8 StatusBuf[20] = {0,};

//Memory Set Data

//NFB STATUS
UINT8 fM1_NFB=0,fM2_NFB=0;

//Buzzer
UINT8 ferrBz[5]={0,};

//Battery Charge,Discharge Flag
UINT8 fCharge=' ';

UINT8 Module1_Sts[8]={0,};
UINT8 Module2_Sts[8]={0,};
UINT8 Battery_Sts[8]={0,};
UINT8 Warning_Sts[8]={0,};
UINT8 ModuleStartup_Sts[2]={0,};
UINT8 Module_CHG_Sts[8]={0,};


UINT8 prev_Module1_Sts[8]={0,};
UINT8 prev_Module2_Sts[8]={0,};
UINT8 prev_Battery_Sts[8]={0,};
UINT8 prev_Warning_Sts[8]={0,};
UINT8 prev_ModuleStartup_Sts[2]={0,};
UINT8 prev_Module_CHG_Sts[8]={0,};
UINT8 fNewErr=0;
UINT8 test[10]={0,};

void DispInit()
{
	if(fAlarmStop == 1)
	{
	setcurLCD(0, 1);
	putrsLCD("Initializing    ");
	InitledAllOn1();
	BZ_TOGGLE();
	Delay500ms();
	InitledAllOff1();
	BZ_TOGGLE();
	Delay500ms();
	setcurLCD(0, 1);
	putrsLCD("Initializing.   ");
	InitledAllOn2();
	BZ_TOGGLE();
	Delay500ms();
	InitledAllOff2();
	BZ_TOGGLE();
	Delay500ms();
	setcurLCD(0, 1);
	putrsLCD("Initializing..  ");
	InitledAllOn3();
	BZ_TOGGLE();
	Delay500ms();
	InitledAllOff3();
	BZ_TOGGLE();
	Delay500ms();
	setcurLCD(0, 1);
	putrsLCD("Initializing... ");
	LedAllOn();
	BZ_TOGGLE();
	Delay500ms();
	LedAllOff();
	BZ_TOGGLE();
	Delay500ms();
	setcurLCD(0, 1);
	putrsLCD("Initializing....");
	LedAllOn();
	BZ_TOGGLE();
	Delay500ms();
	LedAllOff();
	BZ_TOGGLE();
	Delay500ms();
	}
	else
	{
	setcurLCD(0, 1);
	putrsLCD("Initializing    ");
	InitledAllOn1();
	Delay500ms();
	InitledAllOff1();
	Delay500ms();
	setcurLCD(0, 1);
	putrsLCD("Initializing.   ");
	InitledAllOn2();
	Delay500ms();
	InitledAllOff2();
	Delay500ms();
	setcurLCD(0, 1);
	putrsLCD("Initializing..  ");
	InitledAllOn3();
	Delay500ms();
	InitledAllOff3();
	Delay500ms();
	setcurLCD(0, 1);
	putrsLCD("Initializing... ");
	LedAllOn();
	Delay500ms();
	LedAllOff();
	Delay500ms();
	setcurLCD(0, 1);
	putrsLCD("Initializing....");
	LedAllOn();
	Delay500ms();
	LedAllOff();
	Delay500ms();
	}
}

void InitledAllOn1()
{
	M1_NFB=ON;
	M2_NFB=ON;
	DC1_NFB=ON;
	DC2_NFB=ON;
	M1_LINE=ON;
	M2_LINE=ON;
	OUT_LINE=ON;
	BATT_NFB=ON;
	LOAD_NFB=ON;
	
}

void InitledAllOff1()
{
	M1_NFB=OFF;
	M2_NFB=OFF;
	DC1_NFB=OFF;
	DC2_NFB=OFF;
	M1_LINE=OFF;
	M2_LINE=OFF;
	OUT_LINE=OFF;
	BATT_NFB=OFF;
	LOAD_NFB=OFF;
}

void InitledAllOn2()
{
	NORMAL=ON;
	FAIL=ON;
}

void InitledAllOff2()
{
	NORMAL=OFF;
	FAIL=OFF;
}

void InitledAllOn3()
{
	AUTO=ON;
	MANUAL=ON;
	NO_1=ON;
	NO_2=ON;
}

void InitledAllOff3()
{
	AUTO=OFF;
	MANUAL=OFF;
	NO_1=OFF;
	NO_2=OFF;
}

void LedAllOn()
{
	DC1_NFB=ON;
	DC2_NFB=ON;
	BATT_NFB=ON;
	LOAD_NFB=ON;
	M1_NFB=ON;
	M2_NFB=ON;
	M2_LINE=ON;
	M1_LINE=ON;
	NORMAL=ON;
	OUT_LINE=ON;
	FAIL=ON;
	AUTO=ON;
	MANUAL=ON;
	NO_1=ON;
	NO_2=ON;
}

void LedAllOff()
{
	DC1_NFB=OFF;
	DC2_NFB=OFF;
	BATT_NFB=OFF;
	LOAD_NFB=OFF;
	M1_NFB=OFF;
	M2_NFB=OFF;
	M2_LINE=OFF;
	M1_LINE=OFF;
	NORMAL=OFF;
	OUT_LINE=OFF;
	FAIL=OFF;
	AUTO=OFF;
	MANUAL=OFF;
	NO_1=OFF;
	NO_2=OFF;
}

void InitSetValue()
{
	int tmpvalue=0;
	tmpvalue = (gBattVoltSet[0]-0x30)*1000 + (gBattVoltSet[1]-0x30)*100  + (gBattVoltSet[2]-0x30)*10 + (gBattVoltSet[3]-0x30)*1;
    BattHiVoltage = tmpvalue * 0.1;  //  xxxx -> xxx.x
	tmpvalue = (gBattVoltSet[4]-0x30)*1000 + (gBattVoltSet[5]-0x30)*100  + (gBattVoltSet[6]-0x30)*10 + (gBattVoltSet[7]-0x30)*1;
	BattLowVoltage = tmpvalue * 0.1;  //  xxxx -> xxx.x
	tmpvalue = (gRestartTime[0])*10;// + (gBattVoltSet[1]-0x30)*100  + (gBattVoltSet[2]-0x30)*10 + (gBattVoltSet[3]-0x30)*1;
    RestartTimeValue = tmpvalue * 0.1;  //  xxxx -> xxx.x	
}
void InitLCD(void)
{
	Temp_LCD_DATA = LCD_DATA ;				
	LCD_E = 0 ;								
	LCD_DATA &= 0xfff0;						
	DIR_LCD_DATA &= 0xfff0;					
	DIR_LCD_E = 0;							
	DIR_LCD_RS = 0;
	DIR_LCD_RW = 0;

	LCD_DATA &= 0xfff0 ;		
	LCD_DATA |= 0x0003 ;		
	LCD_CMD_W_Timing() ;		
	LCD_L_Delay() ;				
								
	LCD_DATA &= 0xfff0 ;		
	LCD_DATA |= 0x0003 ;		
	LCD_CMD_W_Timing() ;		
	LCD_L_Delay() ;				
								
	LCD_DATA &= 0xfff0 ;		
	LCD_DATA |= 0x0003 ;		
	LCD_CMD_W_Timing() ;		
	LCD_L_Delay() ;				

	LCD_DATA &= 0xfff0 ;		
	LCD_DATA |= 0x0002 ;		
	LCD_CMD_W_Timing() ;
	LCD_L_Delay() ;

	WriteCmdLCD(DISP_2Line_4Bit);
	LCD_S_Delay() ;
	WriteCmdLCD(DISP_ON) ;		
	LCD_S_Delay() ;
	WriteCmdLCD(ENTRY_INC) ;	
	LCD_S_Delay() ;
	WriteCmdLCD(CLR_DISP) ;		
	LCD_L_Delay() ;
	LCD_DATA = Temp_LCD_DATA ;	
}

void WriteCmdLCD( unsigned char LCD_CMD) 
{
	Temp_LCD_DATA = LCD_DATA ;					
                                                
	Temp_CMD = (LCD_CMD & 0xF0)>>4 ;			
	LCD_DATA= (LCD_DATA & 0xfff0)|Temp_CMD ;	
	LCD_CMD_W_Timing () ;                       
                                                
	Temp_CMD = LCD_CMD & 0x0F ;					
	LCD_DATA= (LCD_DATA & 0xfff0)|Temp_CMD ;    
	LCD_CMD_W_Timing () ;                       
                                                
	LCD_DATA = Temp_LCD_DATA ;					
	LCD_S_Delay() ;								

}

void WriteDataLCD(unsigned char LCD_CMD) 
{
	Temp_LCD_DATA = LCD_DATA ;				
                                            
	Temp_CMD = (LCD_CMD & 0xF0)>>4 ;		
	LCD_DATA= (LCD_DATA & 0xfff0)|Temp_CMD ;
	LCD_DAT_W_Timing () ;                   
                                            
	Temp_CMD = LCD_CMD & 0x0F ;				
	LCD_DATA= (LCD_DATA & 0xfff0)|Temp_CMD ;
	LCD_DAT_W_Timing () ;                   
                                            
	LCD_DATA = Temp_LCD_DATA ;				
	LCD_S_Delay() ;
}

void putcLCD(unsigned char LCD_Char)
{
	WriteDataLCD(LCD_Char);
}

void LCD_CMD_W_Timing(void)
{
	LCD_RS = 0;
	Nop();
	LCD_RW = 0;
	Nop();
	LCD_E = 1;
	Nop();
	Nop();
	Nop();
	Nop();
	LCD_E = 0;
}

void LCD_DAT_W_Timing(void)
{
	LCD_RS = 1 ;	// Set for Data Input
	Nop( );
	LCD_RW = 0 ;
	Nop( );
	LCD_E = 1 ;
	Nop( );
	Nop( );
	Nop( );
	Nop( );
	LCD_E = 0 ;
}

void setcurLCD(unsigned char CurX, unsigned char CurY)
{
	//WriteCmdLCD( 0x80 + CurY * 0x40 + CurX) ;
	
	switch(CurY)
	{
		case 0 : WriteCmdLCD( 0x80 + CurX) ;break;
		case 1 : WriteCmdLCD( 0xc0 + CurX) ;break;
		case 2 : WriteCmdLCD( 0x90 + CurX) ;break;
		case 3 : WriteCmdLCD( 0xd0 + CurX) ;break;
	}
	LCD_S_Delay() ;
}

void putrsLCD( const char *Str )
{
	while (1)
	{
		Str_Temp = *Str ;

		if (Str_Temp != 0x00 )
		   {
			WriteDataLCD(Str_Temp) ;
			Str ++ ;
		   }
		else
			return ;
   }
}

void putsLCD(char *Str)
{
	while (1)
	{
		Str_Temp = *Str ;

		if (Str_Temp != 0x00 )
		   {
			WriteDataLCD(Str_Temp) ;
			Str ++ ;
		   }
		else
			return ;
   }
}

void puthexLCD(unsigned char HEX_Val)
{
	unsigned char Temp_HEX ;

	Temp_HEX = (HEX_Val >> 4) & 0x0f ;		
                                            
	if ( Temp_HEX > 9 )Temp_HEX += 0x37 ;	
    else Temp_HEX += 0x30 ;					
                                            
	WriteDataLCD(Temp_HEX) ;                
                                            
	Temp_HEX = HEX_Val  & 0x0f ;			
	if ( Temp_HEX > 9 )Temp_HEX += 0x37 ;
    else Temp_HEX += 0x30 ;

	WriteDataLCD(Temp_HEX) ;
}

void put_Num_LCD( unsigned char The_Number )
{

	unsigned char	Temp_Char_100, Temp_Char_10, Temp_Char ;

		Temp_Char_100 = The_Number/100;
		Temp_Char_10 = (The_Number - Temp_Char_100*100)/10;
		putcLCD( Temp_Char_10 + '0' );
		Temp_Char = The_Number - ( Temp_Char_100*100 + Temp_Char_10 * 10 );
		putcLCD( Temp_Char + '0' ) ;
}

void put_Num3_LCD( unsigned int The_Number )
{

	unsigned char	Temp_Char_100, Temp_Char_10, Temp_Char ;

		Temp_Char_100 = (unsigned char)(The_Number/100);
		putcLCD( Temp_Char_100 + '0' );
		Temp_Char_10 = (unsigned char)(The_Number - Temp_Char_100*100)/10;
		putcLCD( Temp_Char_10 + '0' );
		Temp_Char = (unsigned char) (The_Number - ( Temp_Char_100*100 + Temp_Char_10 * 10 ));
		putcLCD( Temp_Char + '0' ) ;
}

void LCD_L_Delay(void)
{
	int		L_Loop ;

	for 	( L_Loop = 0 ; L_Loop < 100 ; L_Loop ++ )
				LCD_S_Delay( ) ;		
}

void LCD_S_Delay(void)
{
	int		S_Loop ;
	int		Null_Var ;

	for  	( S_Loop = 0 ; S_Loop < 200 ; S_Loop ++ )
				Null_Var += 1 ;
		
}

void LCD_Build_Char(unsigned char loc, unsigned char *p)
{
	unsigned char i;
	if(loc<8)
	{
		WriteCmdLCD(0x40+(loc<<3));
		for(i=0;i<8;i++)WriteDataLCD(p[i]);
	}
	WriteCmdLCD(0x80);
}

void errBz_On()
{
	if(fAlarmStop == 1 && fCheckErr == 1)	// || fNfbCheckErr == 1))
	{
		if(errBz100ms>20)
		{
			BZ_OFF();
		}
		if(errBz100ms>40)
		{
			errBz100ms=0;
			BZ_ON();
		}
		fCheckErr=0;
		//fNfbCheckErr=0;
	}
	else if(fAlarmStop == 0 && fCheckErr == 1)	// || fNfbCheckErr == 1))
	{
		fCheckErr=0;
		//fNfbCheckErr=0;
		BZ_OFF();
	}
	else BZ_OFF();
}

void MainProcess()
{
		switch(iMenuIndex)
		{
			case M0_LOGO:
				DispLogo();
				break;
			case M0_OUTBATT:
				DispOutBatt();
				break;
			case M0_LOAD:
				DispLoad();
				break;
			case M0_SETUP:
				DispSetup();
				break;
			case M1_SETUPSET:
				DispSetupSet();
				break;
			case M1_BATTALARM:
				DispBattAlarm();
				break;
			case M2_BATTALARMSET:
                DispBattAlarmSet();
				break;
			case M1_MODULERESTART:
				DispModuleRestart();
				break;
			case M2_MODULERESTARTSET:
				DispModuleRestartSet();
				break;
			case M1_SYSTEMINIT:
				DispSystemInit();
				break;
			case M2_SYSTEMINITSET:
				DispSystemInitSet();
				break;
			case M1_PASSWORD:
				DispPassword();
				break;
			case M2_PASSWORDSET:
				DispPasswordSet();
				break;
			case M1_ALARMSTOP:
				DispAlarmStop();
				break;
			case M2_ALARMSTOPSET:
				DispAlarmStopSet();
				break;
			case M0_HISTORY:
				DispHistory();
				break;
			case M0_TIME:
				DispTime();
				break;
			case M1_TIMESET:
				DispTimeSet();
				break;	
            case M3_System:
				SysSetup();
				break;
            case M4_SystemSet:
				SysSetupSet();
				break;
            case M3_BATTTYPE:
                DispBattType();
                break;
            case M4_BATTTYPESET:
                DispBattTypeSet();
                break;
            case M3_INPUTVOLT:
                DispInputVoltType();
                break;
            case M4_INPUTVOLTSET:
				DispInputVoltTypeSet();
				break;
            case M3_OPERATIONMODE:
                DispOperationMode();
                break;
            case M4_OPERATIONMODESET:
				DispOperationModeSet();
				break;
            case M3_SYSTYPE:
                DispSysType();
                break;
            case M4_SYSTYPESET:
                DispSysTypeSet();
                break;
                
            
			default: DispLogo();
				break;
		}	// END switch (iMenuIndex)
}

void DispLogo()
{
	setcurLCD(0, 0);
	putrsLCD(" **HFC-3100D**  ");
	setcurLCD(0, 1);		
	putrsLCD("Digital Charger ");
	setcurLCD(0, 2);
	putrsLCD("Seoul Elec.Power");
	setcurLCD(0, 3);
	putrsLCD("Tel)032-681-8411");
	
	switch(hKey())
	{
		case keyKEY_UP:
			iMenuIndex = M0_TIME;
			break;
		case keyKEY_DOWN:
			iMenuIndex = M0_OUTBATT;
			break;
		case keyKEY_LEFT:
			break;
		case keyKEY_RIGHT:
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
		case keyKEY_ENTERfast:
			break;
        case keyKEY_ENTER:
            break;            
		default:
			break;			
	}
	
}

void DispOutBatt()
{
	UINT8 dg[10]={0,};
	dg[0]=((BattVoltage*10)/1000);
	dg[1]=(((BattVoltage*10)-(dg[0]*1000))/100);
	dg[2]=(((BattVoltage*10)-(dg[0]*1000))-(dg[1]*100))/10;
	dg[3]=((BattVoltage*10)-(dg[0]*1000))-(dg[1]*100)-(dg[2]*10);
	dg[4]=((BattCurrent*10)/1000);
	dg[5]=(((BattCurrent*10)-(dg[4]*1000))/100);
	dg[6]=(((BattCurrent*10)-(dg[4]*1000))-(dg[5]*100))/10;
	dg[7]=((BattCurrent*10)-(dg[4]*1000))-(dg[5]*100)-(dg[6]*10);
	setcurLCD(0, 0);
	putrsLCD("*Batt.Volt&Amp* ");
	//
	setcurLCD(0, 1);
	putrsLCD("                ");
	//
	setcurLCD(0, 2);
	putrsLCD("Batt:   . V    A");
	setcurLCD(5, 2);
	putcLCD(dg[0]+0x30);
	putcLCD(dg[1]+0x30);
	putcLCD(dg[2]+0x30);
	setcurLCD(9, 2);
	putcLCD(dg[3]+0x30);
	setcurLCD(11, 2);
	putcLCD(fCharge);
	setcurLCD(12, 2);
	putcLCD(dg[4]+0x30);
	putcLCD(dg[5]+0x30);
	putcLCD(dg[6]+0x30);
	//
	setcurLCD(0, 3);
	putrsLCD("                ");
	//
	switch(hKey())
	{
		case keyKEY_UP:
			iMenuIndex = M0_LOGO;
			break;
		case keyKEY_DOWN:
			iMenuIndex = M0_LOAD;
			break;
		case keyKEY_LEFT:
			break;
		case keyKEY_RIGHT:
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
		default:
			break;
	}
}

void DispLoad()
{
	UINT8 dg[10]={0,};
	dg[0]=((LoadVoltage*10)/1000);
	dg[1]=(((LoadVoltage*10)-(dg[0]*1000))/100);
	dg[2]=(((LoadVoltage*10)-(dg[0]*1000))-(dg[1]*100))/10;
	dg[3]=((LoadVoltage*10)-(dg[0]*1000))-(dg[1]*100)-(dg[2]*10);
	dg[4]=((LoadCurrent*10)/1000);
	dg[5]=(((LoadCurrent*10)-(dg[4]*1000))/100);
	dg[6]=(((LoadCurrent*10)-(dg[4]*1000))-(dg[5]*100))/10;
	dg[7]=((LoadCurrent*10)-(dg[4]*1000))-(dg[5]*100)-(dg[6]*10);
	//Vvalue=261;
	setcurLCD(0, 0);
	putrsLCD("*Load Volt&Amp* ");
	//
	setcurLCD(0, 1);
	putrsLCD("                ");
	//
	setcurLCD(0, 2);
	putrsLCD("Load:   . V    A");
	setcurLCD(5, 2);
	putcLCD(dg[0]+0x30);
	putcLCD(dg[1]+0x30);
	putcLCD(dg[2]+0x30);
	setcurLCD(9, 2);
	putcLCD(dg[3]+0x30);
	setcurLCD(12, 2);
	putcLCD(dg[4]+0x30);
	putcLCD(dg[5]+0x30);
	putcLCD(dg[6]+0x30);
	//
	setcurLCD(0, 3);
	putrsLCD("                ");
	//
	switch(hKey())
	{
		case keyKEY_UP:
			iMenuIndex = M0_OUTBATT;
			break;
		case keyKEY_DOWN:
			iMenuIndex = M0_SETUP;
			break;
		case keyKEY_LEFT:
			break;
		case keyKEY_RIGHT:
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
		default:
			break;
	}
}

void DispSetup()
{
	fTimeSet = 0;
	setcurLCD(0, 0);
	putrsLCD("  ** Setup **   ");
	setcurLCD(0, 1);
	putrsLCD("  [*********]   ");
	setcurLCD(0, 2);	
	putrsLCD("                ");
	setcurLCD(0, 3);
	putrsLCD("                ");
	//
	switch(hKey())
	{
		case keyKEY_UP:
			iMenuIndex = M0_LOAD;
			break;
		case keyKEY_DOWN:
			iMenuIndex = M0_HISTORY;
			break;
		case keyKEY_LEFT:
			break;
		case keyKEY_RIGHT:
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
		case keyKEY_ENTER:
			iMenuIndex = M1_SETUPSET;
			iMenuCnt=0;
			inputCash='0';
			break;	
		default:
			break;
	}
}

void DispSetupSet()
{
	UINT8 i;
	
	setcurLCD(0, 0);
	putrsLCD("  ** Setup **   ");
	setcurLCD(0, 1);
	putrsLCD("  [*********]   ");
	setcurLCD(0, 2);	
	putrsLCD("                ");
	setcurLCD(0, 3);
	putrsLCD("                ");
	
	switch(iMenuCnt)
	{
		case 0 :setcurLCD(3, 1);
				putcLCD(passWord[0]);
				break;
		case 1 :setcurLCD(4, 1);
				putcLCD(passWord[1]);
				break;
		case 2 :setcurLCD(5, 1);
				putcLCD(passWord[2]);
				break;
		case 3 :setcurLCD(6, 1);
				putcLCD(passWord[3]);
				break;
		case 4 :setcurLCD(7, 1);
				putcLCD(passWord[4]);
				break;
		case 5 :setcurLCD(8, 1);
				putcLCD(passWord[5]);
				break;
		case 6 :setcurLCD(9, 1);
				putcLCD(passWord[6]);
				break;	
		case 7 :setcurLCD(10, 1);
				putcLCD(passWord[7]);
				break;
		case 8 :setcurLCD(11, 1);
				putcLCD(passWord[8]);
				break;
		default :
				break;
	}
	switch(hKey())
	{
		case keyKEY_UP:
			inputCash++;
			break;
		case keyKEY_UPfast:
			inputCash++;
			break;	
		case keyKEY_DOWN:
			inputCash--;
			break;
		case keyKEY_DOWNfast:
			inputCash--;
			break;	
		case keyKEY_ENTER:
			if(iMenuCnt == 8)
			{
				fPassSet=1;
			}
			else if(iMenuCnt == 7)
			{
				inputCash=passWord[8];
				iMenuCnt = 8;
			}
			else if(iMenuCnt == 6)
			{
				inputCash=passWord[7];
				iMenuCnt = 7;
			}
			else if(iMenuCnt == 5)
			{
				inputCash=passWord[6];
				iMenuCnt = 6;
			}
			else if(iMenuCnt == 4)
			{
				inputCash=passWord[5];
				iMenuCnt = 5;
			}
			else if(iMenuCnt == 3)
			{
				inputCash=passWord[4];
				iMenuCnt = 4;
			}
			else if(iMenuCnt == 2)
			{
				inputCash=passWord[3];
				iMenuCnt = 3;
			}
			else if(iMenuCnt == 1)
			{
				inputCash=passWord[2];
				iMenuCnt = 2;
			}
			else if(iMenuCnt == 0)
			{
				inputCash=passWord[1];
				iMenuCnt = 1;
			}
			return;
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
	}
	if(iMenuCnt == 0)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		passWord[0] = inputCash;
	}
	else if(iMenuCnt == 1)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		passWord[1] = inputCash;
	}
	else if(iMenuCnt == 2)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		passWord[2] = inputCash;
	}
	else if(iMenuCnt == 3)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		passWord[3] = inputCash;
	}
	else if(iMenuCnt == 4)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		passWord[4] = inputCash;
	}
	else if(iMenuCnt == 5)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		passWord[5] = inputCash;
	}
	else if(iMenuCnt == 6)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		passWord[6] = inputCash;
	}
	else if(iMenuCnt == 7)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		passWord[7] = inputCash;
	}
	else if(iMenuCnt == 8)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		passWord[8] = inputCash;
	}
	if(fPassSet == 1)
	{
		//iMenuCnt=0;
		i = 0;
		for(gCnt=0;gCnt<9;gCnt++)
		{
			data_8 = gPass[gCnt];
			buff_8 = passWord[gCnt];
			i += (data_8 ^ buff_8);
		}
		if(i == 0)
		{
			fTimeSet = 1;
			iMenuCnt=0;
			fPassSet=0;
			iMenuIndex = M1_BATTALARM;
		}
		else if(tmr1s>1)
		{
			tmr1s=0;
			for(gCnt=0;gCnt<9;gCnt++)
			{
				SpassWord[gCnt]='0';
			}
			tmr1s=0;
			fPassSet=0;
			iMenuIndex = M0_SETUP;
			setcurLCD(0, 2);
			putrsLCD("                ");
			setcurLCD(0, 3);
			putrsLCD("                ");
		}
		else
		{
			setcurLCD(0, 1);
			putrsLCD("  [*********]   ");
			setcurLCD(0, 2);
			putrsLCD(" Passwords are  ");
			setcurLCD(0, 3);
			putrsLCD(" not the same.  ");
		}
	//iMenuIndex[0] = M0_SETUP;
	}
	if(cursor1s>1)
	{
		cursor1s=0;
		//fPassSet=0;
	}
}

void SysSetup()
{
	setcurLCD(0, 0);
	putrsLCD("  ** Debug **   ");
	setcurLCD(0, 1);
	putrsLCD("  [*********]   ");
	setcurLCD(0, 2);	
	putrsLCD("                ");
	setcurLCD(0, 3);
	putrsLCD("                ");
	//
	switch(hKey())
	{
		case keyKEY_UP:
            iMenuIndex = M1_ALARMSTOP;
			break;
		case keyKEY_DOWN:
            iMenuIndex = M1_BATTALARM;
			break;
		case keyKEY_LEFT:
            iMenuIndex = M0_SETUP;
			break;
		case keyKEY_RIGHT:
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
		case keyKEY_ENTER:
			iMenuIndex = M4_SystemSet;
			iMenuCnt=0;
			inputCash='0';
			break;	
		default:
			break;
	}
}

void SysSetupSet()
{
	UINT8 i;
	
	setcurLCD(0, 0);
	putrsLCD("  ** Debug **   ");
	setcurLCD(0, 1);
	putrsLCD("  [*********]   ");
	setcurLCD(0, 2);	
	putrsLCD("                ");
	setcurLCD(0, 3);
	putrsLCD("                ");
	
	switch(iMenuCnt)
	{
		case 0 :setcurLCD(3, 1);
				putcLCD(SpassWord[0]);
				break;
		case 1 :setcurLCD(4, 1);
				putcLCD(SpassWord[1]);
				break;
		case 2 :setcurLCD(5, 1);
				putcLCD(SpassWord[2]);
				break;
		case 3 :setcurLCD(6, 1);
				putcLCD(SpassWord[3]);
				break;
		case 4 :setcurLCD(7, 1);
				putcLCD(SpassWord[4]);
				break;
		case 5 :setcurLCD(8, 1);
				putcLCD(SpassWord[5]);
				break;
		case 6 :setcurLCD(9, 1);
				putcLCD(SpassWord[6]);
				break;	
		case 7 :setcurLCD(10, 1);
				putcLCD(SpassWord[7]);
				break;
		case 8 :setcurLCD(11, 1);
				putcLCD(SpassWord[8]);
				break;
		default :
				break;
	}
	switch(hKey())
	{
		case keyKEY_UP:
			inputCash++;
			break;
		case keyKEY_UPfast:
			inputCash++;
			break;	
		case keyKEY_DOWN:
			inputCash--;
			break;
		case keyKEY_DOWNfast:
			inputCash--;
			break;	
		case keyKEY_ENTER:
			if(iMenuCnt == 8)
			{
				fPassSet=1;
			}
			else if(iMenuCnt == 7)
			{
				inputCash=SpassWord[8];
				iMenuCnt = 8;
			}
			else if(iMenuCnt == 6)
			{
				inputCash=SpassWord[7];
				iMenuCnt = 7;
			}
			else if(iMenuCnt == 5)
			{
				inputCash=SpassWord[6];
				iMenuCnt = 6;
			}
			else if(iMenuCnt == 4)
			{
				inputCash=SpassWord[5];
				iMenuCnt = 5;
			}
			else if(iMenuCnt == 3)
			{
				inputCash=SpassWord[4];
				iMenuCnt = 4;
			}
			else if(iMenuCnt == 2)
			{
				inputCash=SpassWord[3];
				iMenuCnt = 3;
			}
			else if(iMenuCnt == 1)
			{
				inputCash=SpassWord[2];
				iMenuCnt = 2;
			}
			else if(iMenuCnt == 0)
			{
				inputCash=SpassWord[1];
				iMenuCnt = 1;
			}
			return;
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
	}
	if(iMenuCnt == 0)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		SpassWord[0] = inputCash;
	}
	else if(iMenuCnt == 1)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		SpassWord[1] = inputCash;
	}
	else if(iMenuCnt == 2)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		SpassWord[2] = inputCash;
	}
	else if(iMenuCnt == 3)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		SpassWord[3] = inputCash;
	}
	else if(iMenuCnt == 4)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		SpassWord[4] = inputCash;
	}
	else if(iMenuCnt == 5)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		SpassWord[5] = inputCash;
	}
	else if(iMenuCnt == 6)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		SpassWord[6] = inputCash;
	}
	else if(iMenuCnt == 7)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		SpassWord[7] = inputCash;
	}
	else if(iMenuCnt == 8)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		SpassWord[8] = inputCash;
	}
	if(fPassSet == 1)
	{
		//iMenuCnt=0;
		i = 0;
		for(gCnt=0;gCnt<9;gCnt++)
		{
			data_8 = SgPass[gCnt];
			buff_8 = SpassWord[gCnt];
			i += (data_8 ^ buff_8);
		}
		if(i == 0)
		{
			iMenuCnt=0;
			fPassSet=0;
			iMenuIndex = M3_BATTTYPE;
		}
		else if(tmr1s>1)
		{
			tmr1s=0;
			for(gCnt=0;gCnt<9;gCnt++)
			{
				SpassWord[gCnt]='0';
			}
			tmr1s=0;
			fPassSet=0;
			iMenuIndex = M3_System;
			setcurLCD(0, 2);
			putrsLCD("                ");
			setcurLCD(0, 3);
			putrsLCD("                ");
		}
		else
		{
			setcurLCD(0, 1);
			putrsLCD("  [*********]   ");
			setcurLCD(0, 2);
			putrsLCD(" Passwords are  ");
			setcurLCD(0, 3);
			putrsLCD(" not the same.  ");
		}
	//iMenuIndex[0] = M0_SETUP;
	}
	if(cursor1s>1)
	{
		cursor1s=0;
		//fPassSet=0;
	}
}

void DispBattAlarm()
{
    if(read_EE(BATT_TYPE) == 0)
    {
        setcurLCD(0, 0);
        putrsLCD("** Batt.Alarm **");
        setcurLCD(0, 1);
        putrsLCD("                ");
        setcurLCD(0, 2);	
        putrsLCD("Max :      V    ");
        setcurLCD(8, 2);
        putcLCD(gBattVoltSet[0]);
        putcLCD(gBattVoltSet[1]);
        putcLCD(gBattVoltSet[2]);
        setcurLCD(0, 3);
        putrsLCD("Min :      V    ");
        setcurLCD(8, 3);
        putcLCD(gBattVoltSet[4]);
        putcLCD(gBattVoltSet[5]);
        putcLCD(gBattVoltSet[6]);
    }
    else if(read_EE(BATT_TYPE) == 1)
    {
        setcurLCD(0, 0);
        putrsLCD("** Batt.Alarm **");
        setcurLCD(0, 1);
        putrsLCD("                ");
        setcurLCD(0, 2);	
        putrsLCD("Max :      V    ");
        setcurLCD(8, 2);
        putcLCD(gBattVoltSet[0]);
        putcLCD(gBattVoltSet[1]);
        putcLCD(gBattVoltSet[2]);
        setcurLCD(0, 3);
        putrsLCD("Min :      V    ");
        setcurLCD(8, 3);
        putcLCD(gBattVoltSet[4]);
        putcLCD(gBattVoltSet[5]);
        putcLCD(gBattVoltSet[6]);
    }
    else if(read_EE(BATT_TYPE) == 2)
    {
        setcurLCD(0, 0);
        putrsLCD("** Batt.Alarm **");
        setcurLCD(0, 1);
        putrsLCD("                ");
        setcurLCD(0, 2);	
        putrsLCD("Max :      .  V ");
        setcurLCD(8, 2);
        putcLCD(gBattVoltSet[0]);
        putcLCD(gBattVoltSet[1]);
        putcLCD(gBattVoltSet[2]);
        setcurLCD(12, 2);
        putcLCD(gBattVoltSet[3]);
        setcurLCD(0, 3);
        putrsLCD("Min :      .  V ");
        setcurLCD(8, 3);
        putcLCD(gBattVoltSet[4]);
        putcLCD(gBattVoltSet[5]);
        putcLCD(gBattVoltSet[6]);
        setcurLCD(12, 3);
        putcLCD(gBattVoltSet[7]);
    }          
   
	//
	switch(hKey())
	{
		case keyKEY_UP:
            iMenuIndex = M3_System;
			break;
		case keyKEY_DOWN:
			iMenuIndex = M1_MODULERESTART;
			break;
		case keyKEY_LEFT:
			iMenuIndex = M0_SETUP;
			break;
		case keyKEY_RIGHT:
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
		case keyKEY_ENTER:
			iMenuIndex = M2_BATTALARMSET;
			iMenuCnt=0;
			inputCash=gBattVoltSet[0];
			break;	
		default:
			break;
	}
}

void DispBattAlarmSet()
{
	UINT8 i;
	int tmpvalue = 0;
    if(read_EE(BATT_TYPE) == 0)
    {
        setcurLCD(0, 0);
        putrsLCD("** Batt.Alarm **");
        setcurLCD(0, 1);
        putrsLCD("                ");
        setcurLCD(0, 2);
        putrsLCD("Max :      V    ");
        setcurLCD(8, 2);
        putcLCD(gBattVoltSet[0]);
        putcLCD(gBattVoltSet[1]);
        putcLCD(gBattVoltSet[2]);
        setcurLCD(0, 3);
        putrsLCD("Min :      V    ");
        setcurLCD(8, 3);
        putcLCD(gBattVoltSet[4]);
        putcLCD(gBattVoltSet[5]);
        putcLCD(gBattVoltSet[6]);

        switch(iMenuCnt)
        {
            case 0 :
                {
                    if(cursor100ms>=1)
                    {
                        setcurLCD(8, 2);
                        putcLCD(gBattVoltSet[0]);
                    }
                    if(cursor100ms>=2)
                    {
                        cursor100ms=0;
                        setcurLCD(8, 2);
                        putrsLCD("_");
                    }
                }
                break;
            case 1 :
                {
                    if(cursor100ms>=1)
                    {
                        setcurLCD(9, 2);
                        putcLCD(gBattVoltSet[1]);
                    }
                    if(cursor100ms>=2)
                    {
                        cursor100ms=0;
                        setcurLCD(9, 2);
                        putrsLCD("_");
                    }
                }
                break;
            case 2 :
                {
                    if(cursor100ms>=1)
                    {
                        setcurLCD(10, 2);
                        putcLCD(gBattVoltSet[2]);
                    }
                    if(cursor100ms>=2)
                    {
                        cursor100ms=0;
                        setcurLCD(10, 2);
                        putrsLCD("_");
                    }
                }
                break;
            case 3 :
                {
                    if(cursor100ms>=1)
                    {
                        setcurLCD(8, 3);
                        putcLCD(gBattVoltSet[4]);
                    }
                    if(cursor100ms>=2)
                    {
                        cursor100ms=0;
                        setcurLCD(8, 3);
                        putrsLCD("_");
                    }
                }
                break;
            case 4 :
                {
                    if(cursor100ms>=1)
                    {
                        setcurLCD(9, 3);
                        putcLCD(gBattVoltSet[5]);
                    }
                    if(cursor100ms>=2)
                    {
                        cursor100ms=0;
                        setcurLCD(9, 3);
                        putrsLCD("_");
                    }
                }
                break;
            case 5 :
                {
                    if(cursor100ms>=1)
                    {
                        setcurLCD(10, 3);
                        putcLCD(gBattVoltSet[6]);
                    }
                    if(cursor100ms>=2)
                    {
                        cursor100ms=0;
                        setcurLCD(10, 3);
                        putrsLCD("_");
                    }
                }
                break;
            default :
                break;
        }
        switch(hKey())
        {
            case keyKEY_UP:
                inputCash++;
                break;
            case keyKEY_UPfast:
                inputCash++;
                break;	
            case keyKEY_DOWN:
                inputCash--;
                break;
            case keyKEY_DOWNfast:
                inputCash--;
                break;
            case keyKEY_LEFT:
                iMenuIndex = M1_BATTALARM;
                break;
            case keyKEY_ENTER:
                if(iMenuCnt == 5)
                {
                    for(i = 0; i < 8; i++)
                    {
                        write_EE(BATTVOLT_0+i, gBattVoltSet[0+i]);
                    }
                    iMenuIndex = M1_BATTALARM;

                    tmpvalue = (gBattVoltSet[0]-0x30)*1000 + (gBattVoltSet[1]-0x30)*100  + (gBattVoltSet[2]-0x30)*10 + (gBattVoltSet[3]-0x30)*1;
                    BattHiVoltage = tmpvalue * 0.1;  //  xxxx -> xxx.x
                    tmpvalue = (gBattVoltSet[4]-0x30)*1000 + (gBattVoltSet[5]-0x30)*100  + (gBattVoltSet[6]-0x30)*10 + (gBattVoltSet[7]-0x30)*1;
                    BattLowVoltage = tmpvalue * 0.1;  //  xxxx -> xxx.x
                    Set_OK=1;
                }
                else if(iMenuCnt == 4)
                {
                    inputCash=gBattVoltSet[6];
                    iMenuCnt = 5;
                }
                else if(iMenuCnt == 3)
                {
                    inputCash=gBattVoltSet[5];
                    iMenuCnt = 4;
                }
                else if(iMenuCnt == 2)
                {
                    inputCash=gBattVoltSet[4];
                    iMenuCnt = 3;
                }
                else if(iMenuCnt == 1)
                {
                    inputCash=gBattVoltSet[2];
                    iMenuCnt = 2;
                }
                else if(iMenuCnt == 0)
                {
                    inputCash=gBattVoltSet[1];
                    iMenuCnt = 1;
                }
                return;
                break;
            case keyKEY_BZ_OFF:
                break;
            case keyKEY_BZ_OFFfast:
                break;
        }
        if(iMenuCnt == 0)
        {
            inputCash = '1';
            gBattVoltSet[0] = inputCash;
        }

        else if(iMenuCnt == 1)
        {
            if(inputCash > '5') inputCash = '2';
            if(inputCash < '2') inputCash = '5';
            gBattVoltSet[1] = inputCash;
        }
        else if(iMenuCnt == 2)
        {
            if(gBattVoltSet[1] <= '2')
            {
                if(inputCash > '9') inputCash = '7';
                if(inputCash < '7') inputCash = '9';
            }
            else if(gBattVoltSet[1] >= '5')
            {
                inputCash = '0';
            }            
            else
            {
                if(inputCash > '9') inputCash = '0';
                if(inputCash < '0') inputCash = '9';
            }
            gBattVoltSet[2] = inputCash;
        }
        else if(iMenuCnt == 3)
        {
            if(inputCash > '1') inputCash = '0';
            if(inputCash < '0') inputCash = '1';
            gBattVoltSet[4] = inputCash;
        }
        else if(iMenuCnt == 4)
        {
            if(gBattVoltSet[4] <= '0')
            {
                inputCash = '9';                 
            }
            else if(gBattVoltSet[4] >= '1')
            {
                if(inputCash > '3') inputCash = '0';
                if(inputCash < '0') inputCash = '3';                 
            }
            gBattVoltSet[5] = inputCash;                
        }
        else if(iMenuCnt == 5)
        {
            if(gBattVoltSet[4] >= '1' && gBattVoltSet[5] >= '3')
            {
                if(inputCash > '2') inputCash = '0';
                if(inputCash < '0') inputCash = '2';
            }
            else if(gBattVoltSet[4] <= '0' && gBattVoltSet[5] <= '9')
            {
                if(inputCash > '9') inputCash = '8';
                if(inputCash < '8') inputCash = '9';
            }
            else
            {
                if(inputCash > '9') inputCash = '0';
                if(inputCash < '0') inputCash = '9';
            }
            gBattVoltSet[6] = inputCash;
        }      
    }
    else if(read_EE(BATT_TYPE) == 1)
    {
        setcurLCD(0, 0);
        putrsLCD("** Batt.Alarm **");
        setcurLCD(0, 1);
        putrsLCD("                ");
        setcurLCD(0, 2);
        putrsLCD("Max :      V    ");
        setcurLCD(8, 2);
        putcLCD(gBattVoltSet[0]);
        putcLCD(gBattVoltSet[1]);
        putcLCD(gBattVoltSet[2]);
        setcurLCD(0, 3);
        putrsLCD("Min :      V    ");
        setcurLCD(8, 3);
        putcLCD(gBattVoltSet[4]);
        putcLCD(gBattVoltSet[5]);
        putcLCD(gBattVoltSet[6]);

        switch(iMenuCnt)
        {
            case 0 :
                {
                    if(cursor100ms>=1)
                    {
                        setcurLCD(8, 2);
                        putcLCD(gBattVoltSet[0]);
                    }
                    if(cursor100ms>=2)
                    {
                        cursor100ms=0;
                        setcurLCD(8, 2);
                        putrsLCD("_");
                    }
                }
                break;
            case 1 :
                {
                    if(cursor100ms>=1)
                    {
                        setcurLCD(9, 2);
                        putcLCD(gBattVoltSet[1]);
                    }
                    if(cursor100ms>=2)
                    {
                        cursor100ms=0;
                        setcurLCD(9, 2);
                        putrsLCD("_");
                    }
                }
                break;
            case 2 :
                {
                    if(cursor100ms>=1)
                    {
                        setcurLCD(10, 2);
                        putcLCD(gBattVoltSet[2]);
                    }
                    if(cursor100ms>=2)
                    {
                        cursor100ms=0;
                        setcurLCD(10, 2);
                        putrsLCD("_");
                    }
                }
                break;
            case 3 :
                {
                    if(cursor100ms>=1)
                    {
                        setcurLCD(8, 3);
                        putcLCD(gBattVoltSet[4]);
                    }
                    if(cursor100ms>=2)
                    {
                        cursor100ms=0;
                        setcurLCD(8, 3);
                        putrsLCD("_");
                    }
                }
                break;
            case 4 :
                {
                    if(cursor100ms>=1)
                    {
                        setcurLCD(9, 3);
                        putcLCD(gBattVoltSet[5]);
                    }
                    if(cursor100ms>=2)
                    {
                        cursor100ms=0;
                        setcurLCD(9, 3);
                        putrsLCD("_");
                    }
                }
                break;
            case 5 :
                {
                    if(cursor100ms>=1)
                    {
                        setcurLCD(10, 3);
                        putcLCD(gBattVoltSet[6]);
                    }
                    if(cursor100ms>=2)
                    {
                        cursor100ms=0;
                        setcurLCD(10, 3);
                        putrsLCD("_");
                    }
                }
                break;
            default :
                break;
        }
        switch(hKey())
        {
            case keyKEY_UP:
                inputCash++;
                break;
            case keyKEY_UPfast:
                inputCash++;
                break;	
            case keyKEY_DOWN:
                inputCash--;
                break;
            case keyKEY_DOWNfast:
                inputCash--;
                break;
            case keyKEY_LEFT:
                iMenuIndex = M1_BATTALARM;
                break;
            case keyKEY_ENTER:
                if(iMenuCnt == 5)
                {
                    for(i = 0; i < 8; i++)
                    {
                        write_EE(BATTVOLT_0+i, gBattVoltSet[0+i]);
                    }
                    iMenuIndex = M1_BATTALARM;

                    tmpvalue = (gBattVoltSet[0]-0x30)*1000 + (gBattVoltSet[1]-0x30)*100  + (gBattVoltSet[2]-0x30)*10 + (gBattVoltSet[3]-0x30)*1;
                    BattHiVoltage = tmpvalue * 0.1;  //  xxxx -> xxx.x
                    tmpvalue = (gBattVoltSet[4]-0x30)*1000 + (gBattVoltSet[5]-0x30)*100  + (gBattVoltSet[6]-0x30)*10 + (gBattVoltSet[7]-0x30)*1;
                    BattLowVoltage = tmpvalue * 0.1;  //  xxxx -> xxx.x
                    Set_OK=1;
                }
                else if(iMenuCnt == 4)
                {
                    inputCash=gBattVoltSet[6];
                    iMenuCnt = 5;
                }
                else if(iMenuCnt == 3)
                {
                    inputCash=gBattVoltSet[5];
                    iMenuCnt = 4;
                }
                else if(iMenuCnt == 2)
                {
                    inputCash=gBattVoltSet[4];
                    iMenuCnt = 3;
                }
                else if(iMenuCnt == 1)
                {
                    inputCash=gBattVoltSet[2];
                    iMenuCnt = 2;
                }
                else if(iMenuCnt == 0)
                {
                    inputCash=gBattVoltSet[1];
                    iMenuCnt = 1;
                }
                return;
                break;
            case keyKEY_BZ_OFF:
                break;
            case keyKEY_BZ_OFFfast:
                break;
        }
        if(iMenuCnt == 0)
        {
            inputCash = '1';
            gBattVoltSet[0] = inputCash;
        }

        else if(iMenuCnt == 1)
        {
            if(inputCash > '5') inputCash = '2';
            if(inputCash < '2') inputCash = '5';
            gBattVoltSet[1] = inputCash;
        }
        else if(iMenuCnt == 2)
        {
            if(gBattVoltSet[1] <= '2')
            {
                if(inputCash > '9') inputCash = '0';
                if(inputCash < '0') inputCash = '9';
            }
            else if(gBattVoltSet[1] >= '5')
            {
                inputCash = '0';
            }            
            else
            {
                if(inputCash > '9') inputCash = '0';
                if(inputCash < '0') inputCash = '9';
            }
            gBattVoltSet[2] = inputCash;
        }
        else if(iMenuCnt == 3)
        {
            inputCash = '1';
            gBattVoltSet[4] = inputCash;
        }
        else if(iMenuCnt == 4)
        {
            if(gBattVoltSet[4] >= '1')
            {
                if(inputCash > '2') inputCash = '0';
                if(inputCash < '0') inputCash = '2';                 
            }
            gBattVoltSet[5] = inputCash;                
        }
        else if(iMenuCnt == 5)
        {
            if(gBattVoltSet[4] >= '1' && gBattVoltSet[5] >= '2')
            {
                inputCash = '0';
            }
            else if(gBattVoltSet[4] <= '1' && gBattVoltSet[5] <= '0')
            {
                if(inputCash > '9') inputCash = '5';
                if(inputCash < '5') inputCash = '5';
            }                
            else
            {
                if(inputCash > '9') inputCash = '0';
                if(inputCash < '0') inputCash = '9';
            }
            gBattVoltSet[6] = inputCash;
        }  
    }
    else if(read_EE(BATT_TYPE) == 2)
    {
        setcurLCD(0, 0);
        putrsLCD("** Batt.Alarm **");
        setcurLCD(0, 1);
        putrsLCD("                ");
        setcurLCD(0, 2);
        putrsLCD("Max :      .  V ");
        setcurLCD(8, 2);
        putcLCD(gBattVoltSet[0]);
        putcLCD(gBattVoltSet[1]);
        putcLCD(gBattVoltSet[2]);
        setcurLCD(12, 2);
        putcLCD(gBattVoltSet[3]);
        setcurLCD(0, 3);
        putrsLCD("Min :      .  V ");
        setcurLCD(8, 3);
        putcLCD(gBattVoltSet[4]);
        putcLCD(gBattVoltSet[5]);
        putcLCD(gBattVoltSet[6]);
        setcurLCD(12, 3);
        putcLCD(gBattVoltSet[7]);

        switch(iMenuCnt)
        {
            case 0 :
                {
                    if(cursor100ms>=1)
                    {
                        setcurLCD(8, 2);
                        putcLCD(gBattVoltSet[0]);
                    }
                    if(cursor100ms>=2)
                    {
                        cursor100ms=0;
                        setcurLCD(8, 2);
                        putrsLCD("_");
                    }
                }
                break;
            case 1 :
                {
                    if(cursor100ms>=1)
                    {
                        setcurLCD(9, 2);
                        putcLCD(gBattVoltSet[1]);
                    }
                    if(cursor100ms>=2)
                    {
                        cursor100ms=0;
                        setcurLCD(9, 2);
                        putrsLCD("_");
                    }
                }
                break;
            case 2 :
                {
                    if(cursor100ms>=1)
                    {
                        setcurLCD(10, 2);
                        putcLCD(gBattVoltSet[2]);
                    }
                    if(cursor100ms>=2)
                    {
                        cursor100ms=0;
                        setcurLCD(10, 2);
                        putrsLCD("_");
                    }
                }
                break;
            case 3 :
                {
                    if(cursor100ms>=1)
                    {
                        setcurLCD(12, 2);
                        putcLCD(gBattVoltSet[3]);
                    }
                    if(cursor100ms>=2)
                    {
                        cursor100ms=0;
                        setcurLCD(12, 2);
                        putrsLCD("_");
                    }
                }
                break;   
            case 4 :
                {
                    if(cursor100ms>=1)
                    {
                        setcurLCD(8, 3);
                        putcLCD(gBattVoltSet[4]);
                    }
                    if(cursor100ms>=2)
                    {
                        cursor100ms=0;
                        setcurLCD(8, 3);
                        putrsLCD("_");
                    }
                }
                break;
            case 5 :
                {
                    if(cursor100ms>=1)
                    {
                        setcurLCD(9, 3);
                        putcLCD(gBattVoltSet[5]);
                    }
                    if(cursor100ms>=2)
                    {
                        cursor100ms=0;
                        setcurLCD(9, 3);
                        putrsLCD("_");
                    }
                }
                break;
            case 6 :
                {
                    if(cursor100ms>=1)
                    {
                        setcurLCD(10, 3);
                        putcLCD(gBattVoltSet[6]);
                    }
                    if(cursor100ms>=2)
                    {
                        cursor100ms=0;
                        setcurLCD(10, 3);
                        putrsLCD("_");
                    }
                }
                break;
            case 7 :
                {
                    if(cursor100ms>=1)
                    {
                        setcurLCD(12, 3);
                        putcLCD(gBattVoltSet[7]);
                    }
                    if(cursor100ms>=2)
                    {
                        cursor100ms=0;
                        setcurLCD(12, 3);
                        putrsLCD("_");
                    }
                }
                break;
            default :
                break;
        }
        switch(hKey())
        {
            case keyKEY_UP:
                inputCash++;
                break;
            case keyKEY_UPfast:
                inputCash++;
                break;	
            case keyKEY_DOWN:
                inputCash--;
                break;
            case keyKEY_DOWNfast:
                inputCash--;
                break;
            case keyKEY_LEFT:
                iMenuIndex = M1_BATTALARM;
                break;
            case keyKEY_ENTER:
                if(iMenuCnt == 7)
                {
                    for(i = 0; i < 8; i++)
                    {
                        write_EE(BATTVOLT_0+i, gBattVoltSet[0+i]);
                    }
                    iMenuIndex = M1_BATTALARM;

                    tmpvalue = (gBattVoltSet[0]-0x30)*1000 + (gBattVoltSet[1]-0x30)*100  + (gBattVoltSet[2]-0x30)*10 + (gBattVoltSet[3]-0x30)*1;
                    BattHiVoltage = tmpvalue * 0.1;  //  xxxx -> xxx.x
                    tmpvalue = (gBattVoltSet[4]-0x30)*1000 + (gBattVoltSet[5]-0x30)*100  + (gBattVoltSet[6]-0x30)*10 + (gBattVoltSet[7]-0x30)*1;
                    BattLowVoltage = tmpvalue * 0.1;  //  xxxx -> xxx.x
                    Set_OK=1;
                }
                else if(iMenuCnt == 6)
                {
                    inputCash=gBattVoltSet[7];
                    iMenuCnt = 7;
                }
                else if(iMenuCnt == 5)
                {
                    inputCash=gBattVoltSet[6];
                    iMenuCnt = 6;
                }
                else if(iMenuCnt == 4)
                {
                    inputCash=gBattVoltSet[5];
                    iMenuCnt = 5;
                }
                else if(iMenuCnt == 3)
                {
                    inputCash=gBattVoltSet[4];
                    iMenuCnt = 4;
                }
                else if(iMenuCnt == 2)
                {
                    inputCash=gBattVoltSet[3];
                    iMenuCnt = 3;
                }
                else if(iMenuCnt == 1)
                {
                    inputCash=gBattVoltSet[2];
                    iMenuCnt = 2;
                }
                else if(iMenuCnt == 0)
                {
                    inputCash=gBattVoltSet[1];
                    iMenuCnt = 1;
                }
                return;
                break;
            case keyKEY_BZ_OFF:
                break;
            case keyKEY_BZ_OFFfast:
                break;
        }
        if(iMenuCnt == 0)
        {
            inputCash = '1';
            gBattVoltSet[0] = inputCash;
        }

        else if(iMenuCnt == 1)
        {
            if(inputCash > '3') inputCash = '1';
            if(inputCash < '1') inputCash = '3';
            gBattVoltSet[1] = inputCash;
        }
        else if(iMenuCnt == 2)
        {
            if(gBattVoltSet[1] <= '1')
            {
                if(inputCash > '9') inputCash = '8';
                if(inputCash < '8') inputCash = '9';
            }
            else if(gBattVoltSet[1] >= '3')
            {
                if(inputCash > '5') inputCash = '0';
                if(inputCash < '0') inputCash = '5';
            }
            else
            {
                if(inputCash > '9') inputCash = '0';
                if(inputCash < '0') inputCash = '9';
            }
            gBattVoltSet[2] = inputCash;
        }
        else if(iMenuCnt == 3)
        {
            if(gBattVoltSet[1] <= '1' && gBattVoltSet[2] <= '8')
            {
                if(inputCash > '9') inputCash = '4';
                if(inputCash < '4') inputCash = '9';
            }
            else if(gBattVoltSet[1] >= '3' && gBattVoltSet[2] >= '5')
            {
                inputCash = '0';
            }
            else
            {
                if(inputCash > '9') inputCash = '0';
                if(inputCash < '0') inputCash = '9';
            }
            gBattVoltSet[3] = inputCash;
        }
        else if(iMenuCnt == 4)
        {
            if(inputCash > '1') inputCash = '0';
            if(inputCash < '0') inputCash = '1';
            gBattVoltSet[4] = inputCash;
        }
        else if(iMenuCnt == 5)
        {
            if(gBattVoltSet[4] == '1')
            {
                if(inputCash > '1') inputCash = '0';
                if(inputCash < '0') inputCash = '1';
            }
            else if(gBattVoltSet[4] == '0')
            {
                inputCash = '9';
            }
            else
            {
                if(inputCash > '9') inputCash = '0';
                if(inputCash < '0') inputCash = '9';
            }
            gBattVoltSet[5] = inputCash;
        }
        else if(iMenuCnt == 6)
        {
            if(gBattVoltSet[4] >= '1' && gBattVoltSet[5] >= '1')
            {
                if(inputCash > '8') inputCash = '0';
                if(inputCash < '0') inputCash = '8';
            }
            else if(gBattVoltSet[4] <= '0' && gBattVoltSet[5] <= '9')
            {
                if(inputCash > '9') inputCash = '6';
                if(inputCash < '6') inputCash = '9';
            }
            else
            {
                if(inputCash > '9') inputCash = '0';
                if(inputCash < '0') inputCash = '9';
            }
            gBattVoltSet[6] = inputCash;
        }
        else if(iMenuCnt == 7)
        {
            if(gBattVoltSet[5] >= '1' && gBattVoltSet[6] >= '8')
            {
                if(inputCash > '4') inputCash = '0';
                if(inputCash < '0') inputCash = '4';
            }
            else
            {
                if(inputCash > '9') inputCash = '0';
                if(inputCash < '0') inputCash = '9';
            }
            gBattVoltSet[7] = inputCash;
        }   
    }
}
/*
void DispAutoManual()
{
	setcurLCD(0, 0);
	putrsLCD("**Auto/Manual** ");
	setcurLCD(0, 1);
	if(gAutoManual == 0)	putrsLCD("Auto         *  ");
	else	putrsLCD("Auto            ");
	setcurLCD(0, 2);	
	putrsLCD("                ");
	setcurLCD(0, 3);
	if(gAutoManual == 0)	putrsLCD("Manual          ");
	else	putrsLCD("Manual       *  ");
	switch(hKey())
	{
		case keyKEY_UP:
			iMenuIndex = M1_BATTALARM;
			break;
		case keyKEY_UPfast:
			break;	
		case keyKEY_DOWN:
			iMenuIndex = M1_MODULERESTART;
			break;
		case keyKEY_DOWNfast:
			inputCash--;
			break;
		case keyKEY_LEFT:
			iMenuIndex = M0_SETUP;
			break;
		case keyKEY_ENTER:
			iMenuIndex = M2_AUTOMANUALSET;
			iMenuCnt = gAutoManual;
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
	}
}
*/
/*
void DispAutoManualSet()
{
	//UINT8 i;
	
	setcurLCD(0, 0);
	putrsLCD("**Auto/Manual** ");
	setcurLCD(0, 1);
	putrsLCD("Auto            ");
	setcurLCD(0, 2);	
	putrsLCD("                ");
	setcurLCD(0, 3);
	putrsLCD("Manual          ");
	
	switch(iMenuCnt)
	{
		case 0 :setcurLCD(13, 1);
				putrsLCD("*<-");
				break;
		case 1 :setcurLCD(13, 3);
				putrsLCD("*<-");
				break;
		default :
				break;
	}
	switch(hKey())
	{
		case keyKEY_UP:			
			iMenuCnt=0;
			break;
		case keyKEY_UPfast:
			break;	
		case keyKEY_DOWN:
			iMenuCnt=1;
			break;
		case keyKEY_DOWNfast:
			inputCash--;
			break;
		case keyKEY_LEFT:
			iMenuIndex = M1_AUTOMANUAL;
			break;
		case keyKEY_ENTER:
			gAutoManual=iMenuCnt;
			Set_OK=1;
			iMenuIndex = M1_AUTOMANUAL;
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
	}
}
*/
void DispModuleRestart()
{
	setcurLCD(0, 0);
	putrsLCD("Restart Time Set");
	setcurLCD(0, 1);
	putrsLCD("                ");
	setcurLCD(0, 2);	
	putrsLCD("        Min     ");
	setcurLCD(5, 2);
	put_Num_LCD(gRestartTime[0]);
	setcurLCD(0, 3);
	putrsLCD("                ");
	//
	switch(hKey())
	{
		case keyKEY_UP:			
			iMenuIndex = M1_BATTALARM;
			break;
        case keyKEY_DOWN:
			iMenuIndex = M0_TIME;
			break;
		case keyKEY_UPfast:
			break;	
		case keyKEY_DOWNfast:
			inputCash--;
			break;
		case keyKEY_LEFT:
			iMenuIndex = M0_SETUP;
			break;
		case keyKEY_ENTER:
			iMenuIndex = M2_MODULERESTARTSET;
			inputCash=gRestartTime[0];
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
	}
}

void DispModuleRestartSet()
{
	int tmpvalue = 0;
	setcurLCD(0, 0);
	putrsLCD("Restart Time Set");
	setcurLCD(0, 1);
	putrsLCD("                ");
	setcurLCD(0, 2);	
	putrsLCD("        Min     ");
	if(cursor100ms>=1)
	{
		setcurLCD(5, 2);
		put_Num_LCD(gRestartTime[0]);
	}
	if(cursor100ms>=2)
	{
		cursor100ms=0;
		setcurLCD(5, 2);
		putrsLCD("  ");
	}
	setcurLCD(0, 3);
	putrsLCD("                ");
	
	switch(hKey())
	{
		case keyKEY_UP:
			inputCash++;
			break;
		case keyKEY_UPfast:
			inputCash++;
			break;	
		case keyKEY_DOWN:
			inputCash--;
			break;
		case keyKEY_DOWNfast:
			inputCash--;
			break;
		case keyKEY_LEFT:
			iMenuIndex = M1_MODULERESTART;
			break;
		case keyKEY_ENTER:
			write_EE(RESTARTTIME_0, gRestartTime[0]);
			Set_OK=1;
			iMenuIndex = M1_MODULERESTART;
			tmpvalue = (gRestartTime[0])*10;// + (gBattVoltSet[1]-0x30)*100  + (gBattVoltSet[2]-0x30)*10 + (gBattVoltSet[3]-0x30)*1;
            RestartTimeValue = tmpvalue * 0.1;  //  xxxx -> xxx.x
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
	}
	//
	if(inputCash > 60) inputCash = 60;
	if(inputCash < 0) inputCash = 0;
	gRestartTime[0] = inputCash;
}

void DispSystemInit()
{
	setcurLCD(0, 0);
	putrsLCD(" **Initialize** ");
	setcurLCD(0, 1);
	putrsLCD("                ");
	setcurLCD(0, 2);	
	putrsLCD("      NO        ");
	setcurLCD(0, 3);
	putrsLCD("                ");
	
	switch(hKey())
	{
		case keyKEY_UP:
			iMenuIndex = M0_TIME;
			break;
        case keyKEY_DOWN:
			iMenuIndex = M1_PASSWORD;
			break;
		case keyKEY_UPfast:
			break;	
		case keyKEY_DOWNfast:
			inputCash--;
			break;
		case keyKEY_LEFT:
			iMenuIndex = M0_SETUP;
			break;			
		case keyKEY_ENTER:
			iMenuIndex = M2_SYSTEMINITSET;
			iMenuCnt = 0;
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
	}
}

void DispSystemInitSet()
{
	setcurLCD(0, 0);
	putrsLCD(" **Initialize** ");
	setcurLCD(0, 1);
	putrsLCD("                ");
	setcurLCD(0, 2);	
	putrsLCD("                ");
	setcurLCD(0, 3);
	putrsLCD("                ");
	
	switch(iMenuCnt)
	{
		case 0 :
			{
				if(cursor100ms>=1)
				{
					setcurLCD(6, 2);
					putrsLCD("NO");
				}
				if(cursor100ms>=2)
				{
					cursor100ms=0;
					setcurLCD(6, 2);
					putrsLCD("   ");
				}
			}
			break;
		case 1 :
			{
				if(cursor100ms>=1)
				{
					setcurLCD(6, 2);
					putrsLCD("YES");
				}
				if(cursor100ms>=2)
				{
					cursor100ms=0;
					setcurLCD(6, 2);
					putrsLCD("   ");
				}
			}
			break;
		default :
				break;
	}
	
	switch(hKey())
	{
		case keyKEY_UP:
			iMenuCnt ^= 1;
			break;
		case keyKEY_UPfast:
			break;	
		case keyKEY_DOWN:
			iMenuCnt ^= 1;
			break;
		case keyKEY_DOWNfast:
			inputCash--;
			break;	
		case keyKEY_ENTER:
			if(iMenuCnt == 0)
			{
				iMenuIndex = M1_SYSTEMINIT;
			}
			else if(iMenuCnt == 1)
			{
				iMenuCnt=0;
				SystemInitialize();
				Set_OK=1;
				iMenuIndex = M1_SYSTEMINIT;
			}
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
	}
}

void DispPassword()
{
	setcurLCD(0, 0);
	putrsLCD(" ** Password ** ");
	setcurLCD(0, 1);
	putrsLCD("                ");
	setcurLCD(0, 2);	
	putrsLCD("OLD :           ");
	setcurLCD(6, 2);
	putcLCD(gPass[0]);
	putcLCD(gPass[1]);
	putcLCD(gPass[2]);
	putcLCD(gPass[3]);
	putcLCD(gPass[4]);
	putcLCD(gPass[5]);
	putcLCD(gPass[6]);
	putcLCD(gPass[7]);
	putcLCD(gPass[8]);
	setcurLCD(0, 3);
	putrsLCD("NEW : ********* ");
	//
	switch(hKey())
	{
		case keyKEY_UP:
			iMenuIndex = M1_SYSTEMINIT;
			break;
		case keyKEY_DOWN:
			iMenuIndex = M1_ALARMSTOP;
			break;
		case keyKEY_LEFT:
			iMenuIndex = M0_SETUP;
			break;
		case keyKEY_RIGHT:
			break;
		case keyKEY_ENTER:
			iMenuIndex = M2_PASSWORDSET;
			iMenuCnt=0;
			inputCash=gPass[0];			
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
		default:
			break;			
	}
}

void DispPasswordSet()
{
	unsigned char i;
	setcurLCD(0, 0);
	putrsLCD(" ** Password ** ");
	setcurLCD(0, 1);
	putrsLCD("                ");
	setcurLCD(0, 3);
	putrsLCD("NEW : ********* ");
	//
	switch(iMenuCnt)
	{
		case 0 :setcurLCD(6, 3);
				putcLCD(gPass[0]);
				break;
		case 1 :setcurLCD(7, 3);
				putcLCD(gPass[1]);
				break;
		case 2 :setcurLCD(8, 3);
				putcLCD(gPass[2]);
				break;
		case 3 :setcurLCD(9, 3);
				putcLCD(gPass[3]);
				break;
		case 4 :setcurLCD(10, 3);
				putcLCD(gPass[4]);
				break;
		case 5 :setcurLCD(11, 3);
				putcLCD(gPass[5]);
				break;
		case 6 :setcurLCD(12, 3);
				putcLCD(gPass[6]);
				break;
		case 7 :setcurLCD(13, 3);
				putcLCD(gPass[7]);
				break;
		case 8 :setcurLCD(14, 3);
				putcLCD(gPass[8]);
				break;
	}
	switch(hKey())
	{
		case keyKEY_UP:
			inputCash++;
			break;
		case keyKEY_UPfast:
			inputCash++;
			break;	
		case keyKEY_DOWN:
			inputCash--;
			break;
		case keyKEY_DOWNfast:
			inputCash--;
			break;	
		case keyKEY_ENTER:
			if(iMenuCnt == 8)
			{
				for(i = 0; i < 9; i++)
				{
					write_EE(PASSWD_0+i, gPass[0+i]);
				}
				iMenuIndex = M1_PASSWORD;
			}
			else if(iMenuCnt == 7)
			{
				inputCash=gPass[8];
				iMenuCnt = 8;
			}
			else if(iMenuCnt == 6)
			{
				inputCash=gPass[7];
				iMenuCnt = 7;
			}
			else if(iMenuCnt == 5)
			{
				inputCash=gPass[6];
				iMenuCnt = 6;
			}
			else if(iMenuCnt == 4)
			{
				inputCash=gPass[5];
				iMenuCnt = 5;
			}
			else if(iMenuCnt == 3)
			{
				inputCash=gPass[4];
				iMenuCnt = 4;
			}
			else if(iMenuCnt == 2)
			{
				inputCash=gPass[3];
				iMenuCnt = 3;
			}
			else if(iMenuCnt == 1)
			{
				inputCash=gPass[2];
				iMenuCnt = 2;
			}
			else if(iMenuCnt == 0)
			{
				inputCash=gPass[1];
				iMenuCnt = 1;
			}
			return;
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
	}
	if(iMenuCnt == 0)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		gPass[0] = inputCash;
	}
	else if(iMenuCnt == 1)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		gPass[1] = inputCash;
	}
	else if(iMenuCnt == 2)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		gPass[2] = inputCash;
	}
	else if(iMenuCnt == 3)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		gPass[3] = inputCash;
	}
	else if(iMenuCnt == 4)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		gPass[4] = inputCash;
	}
	else if(iMenuCnt == 5)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		gPass[5] = inputCash;
	}
	else if(iMenuCnt == 6)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		gPass[6] = inputCash;
	}
	else if(iMenuCnt == 7)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		gPass[7] = inputCash;
	}
	else if(iMenuCnt == 8)
	{
		if(inputCash > '9') inputCash = '0';
		if(inputCash < '0') inputCash = '9';
		gPass[8] = inputCash;
	}
}

void DispAlarmStop()
{
	setcurLCD(0, 0);
	putrsLCD(" **Alarm Stop** ");
	setcurLCD(0, 1);
	putrsLCD("                ");
	setcurLCD(0, 2);
	if(fAlarmStop == 1)
	putrsLCD("  Buzzer : ON   ");
	else
	putrsLCD("  Buzzer : OFF  ");
	setcurLCD(0, 3);
	putrsLCD("                ");
	//
	switch(hKey())
	{
		case keyKEY_UP:
			iMenuIndex = M1_PASSWORD;
			break;
		case keyKEY_DOWN:
            iMenuIndex = M3_System;
			break;
		case keyKEY_LEFT:
			iMenuIndex = M0_SETUP;
			break;
		case keyKEY_RIGHT:
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
		case keyKEY_ENTER:
			iMenuCnt=fAlarmStop;
			iMenuIndex = M2_ALARMSTOPSET;
			break;
		default:
			break;			
	}
}

void DispAlarmStopSet()
{
	//unsigned char fAlarmStop;
	setcurLCD(0, 0);
	putrsLCD(" **Alarm Stop** ");
	setcurLCD(0, 1);
	putrsLCD("                ");
	setcurLCD(0, 2);
	putrsLCD("  Buzzer :      ");
	setcurLCD(0, 3);
	putrsLCD("                ");
	//
	switch(iMenuCnt)
	{
		case 0 :
			{
				if(cursor100ms>=1)
				{
					setcurLCD(0, 2);
					putrsLCD("  Buzzer : OFF  ");
				}
				if(cursor100ms>=2)
				{
					cursor100ms=0;
					setcurLCD(0, 2);
					putrsLCD("  Buzzer :      ");
				}
			}
			break;
		case 1 :
			{
				if(cursor100ms>=1)
				{
					setcurLCD(0, 2);
					putrsLCD("  Buzzer : ON  ");
				}
				if(cursor100ms>=2)
				{
					cursor100ms=0;
					setcurLCD(0, 2);
					putrsLCD("  Buzzer :      ");
				}
			}
			break;
		default :
				break;
	}
	switch(hKey())
	{
		case keyKEY_UP:
			iMenuCnt^=1;
			break;
		case keyKEY_DOWN:
			iMenuCnt^=1;
			break;
		case keyKEY_LEFT:
			iMenuIndex = M1_ALARMSTOP;
			break;
		case keyKEY_RIGHT:
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
		case keyKEY_ENTER:
			fAlarmStop=iMenuCnt;
			write_EE(ALARM_STOP, fAlarmStop);
			iMenuIndex = M1_ALARMSTOP;
			break;
		default:
			break;			
	}
}

void DispHistory()
{
	unsigned char tmpCnt=0,i;	//,alarmNum=0;
	setcurLCD(0, 0);
	putrsLCD("**AlarmHistory**");
	//
	if(gAlarmHistoryCnt>0)	tmpCnt=gAlarmHistoryCnt;
	else tmpCnt=0;
	//
	if(gAlarmHistory[tmpCnt - gC_AlarmHistoryCnt][0])
	{
		AlarmHistoryDisplay(tmpCnt - gC_AlarmHistoryCnt);
	}
	else
	{
		setcurLCD(0, 1);
		putrsLCD("                ");
		setcurLCD(0, 2);
		putrsLCD("    Nothing     ");
		setcurLCD(0, 3);
		putrsLCD("                ");		
	}
	//
	switch(hKey())
	{
		case keyKEY_UP:
			iMenuIndex = M0_SETUP;
			gC_AlarmHistoryCnt=0;
			break;
		case keyKEY_DOWN:
			iMenuIndex = M0_TIME;
			gC_AlarmHistoryCnt=0;
			break;
		case keyKEY_LEFT:
			if((gC_AlarmHistoryCnt+1) < gAlarmHistoryCnt)
			{	
				gC_AlarmHistoryCnt++;
				if(gAlarmHistory[gC_AlarmHistoryCnt][0] == 0) gC_AlarmHistoryCnt--;
			}
			break;
		case keyKEY_RIGHT:
			if(gC_AlarmHistoryCnt>0) gC_AlarmHistoryCnt--;
			break;		
		case keyKEY_LEFTfast:		
			if((gC_AlarmHistoryCnt+1) < gAlarmHistoryCnt)
			{	
				gC_AlarmHistoryCnt++;		
				if(gAlarmHistory[gC_AlarmHistoryCnt][0] == 0) gC_AlarmHistoryCnt--;
			}
			break;
		case keyKEY_RIGHTfast:
			if(gC_AlarmHistoryCnt>0) gC_AlarmHistoryCnt--;
			break;	
		case keyKEY_ENTERfast:
			for(i = 0; i < 200; i++)
			{
				gAlarmHistory[i][0] = 0;
				gAlarmHistoryCnt = 0;
				write_EE(ALARM_STORAGE + (i*10), 0);
				write_EE(ALARM_HISTORY_CNT, gAlarmHistoryCnt);
			}
			for(i = 1; i < 8; i++)
			{
				Module1_Sts[i] = 0;
				prev_Module1_Sts[i] = 0;
				Module2_Sts[i] = 0;
				prev_Module2_Sts[i] = 0;
			}
			for(i = 0; i < 2; i++)
			{
				Battery_Sts[i] = 0;
				prev_Battery_Sts[i] = 0;
			}
			inputCash = 0;
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
		case keyKEY_ENTER:
			break;	
		default:
			break;
	}
}


void DispBattType()
{
	setcurLCD(0, 0);
	putrsLCD(" **Batt  Type** ");
	setcurLCD(0, 1);
    gBattTypeMode = read_EE(BATT_TYPE);
	if(gBattTypeMode == 0)	putrsLCD("PB-125       *  ");
	else	putrsLCD("PB-125          ");
	setcurLCD(0, 2);
    if(gBattTypeMode == 1)	putrsLCD("PB-110       *  ");
	else	putrsLCD("PB-110          ");	
	setcurLCD(0, 3);
	if(gBattTypeMode == 2)	putrsLCD("Li-Ion       *  ");
	else	putrsLCD("Li-Ion          ");
	switch(hKey())
	{
		case keyKEY_UP:
			iMenuIndex = M3_SYSTYPE;
			break;
		case keyKEY_UPfast:
			break;	
		case keyKEY_DOWN:
			iMenuIndex = M3_OPERATIONMODE;
			break;
		case keyKEY_DOWNfast:
			break;
		case keyKEY_LEFT:
			iMenuIndex = M3_System;
			break;
		case keyKEY_ENTER:
			iMenuIndex = M4_BATTTYPESET;
			iMenuCnt = gBattTypeMode;
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
	}
}

void DispBattTypeSet()
{
	UINT8 i;
    int tmpvalue;
	
	setcurLCD(0, 0);
	putrsLCD(" **Batt  Type** ");
	setcurLCD(0, 1);
	putrsLCD("PB-125          ");
	setcurLCD(0, 2);	
	putrsLCD("PB-110          ");
	setcurLCD(0, 3);
	putrsLCD("Li-Ion          ");
	
	switch(iMenuCnt)
	{
		case 0 :setcurLCD(13, 1);
				putrsLCD("*<-");
				break;
        case 1 :setcurLCD(13, 2);
                putrsLCD("*<-");
                break;
		case 2 :setcurLCD(13, 3);
				putrsLCD("*<-");
				break;
		default :
				break;
	}
	switch(hKey())
	{
		case keyKEY_UP:			
			if(iMenuCnt <= 0) iMenuCnt = 2;
            else iMenuCnt--;
			break;
		case keyKEY_UPfast:
			break;	
		case keyKEY_DOWN:
			if(iMenuCnt >= 2) iMenuCnt = 0;
            else iMenuCnt++;
			break;
		case keyKEY_DOWNfast:
			break;
		case keyKEY_LEFT:
			iMenuIndex = M3_BATTTYPE;
			break;
		case keyKEY_ENTER:
			gBattTypeMode=iMenuCnt;
            write_EE(BATT_TYPE, gBattTypeMode);
            if(gBattTypeMode == 0)
            {
                write_EE(BATTVOLT_0,			'1');
                write_EE(BATTVOLT_1,			'5');
                write_EE(BATTVOLT_2,			'0');
                write_EE(BATTVOLT_3,			'0');
                write_EE(BATTVOLT_4,			'0');
                write_EE(BATTVOLT_5,			'9');
                write_EE(BATTVOLT_6,			'8');
                write_EE(BATTVOLT_7,			'6');
            }
            else if(gBattTypeMode == 1)
            {
                write_EE(BATTVOLT_0,			'1');
                write_EE(BATTVOLT_1,			'5');
                write_EE(BATTVOLT_2,			'0');
                write_EE(BATTVOLT_3,			'0');
                write_EE(BATTVOLT_4,			'1');
                write_EE(BATTVOLT_5,			'0');
                write_EE(BATTVOLT_6,			'5');
                write_EE(BATTVOLT_7,			'0');
            }
            else if(gBattTypeMode == 2)
            {
                write_EE(BATTVOLT_0,			'1');
                write_EE(BATTVOLT_1,			'3');
                write_EE(BATTVOLT_2,			'5');
                write_EE(BATTVOLT_3,			'0');
                write_EE(BATTVOLT_4,			'0');
                write_EE(BATTVOLT_5,			'9');
                write_EE(BATTVOLT_6,			'6');
                write_EE(BATTVOLT_7,			'0');
            }
            
            iMenuIndex = M3_BATTTYPE;
            
            for(i = 0; i < 8; i++)
            {
                gBattVoltSet[0+i] = read_EE(BATTVOLT_0+i);
            }

            tmpvalue = (gBattVoltSet[0]-0x30)*1000 + (gBattVoltSet[1]-0x30)*100  + (gBattVoltSet[2]-0x30)*10 + (gBattVoltSet[3]-0x30)*1;
            BattHiVoltage = tmpvalue * 0.1;  //  xxxx -> xxx.x
            tmpvalue = (gBattVoltSet[4]-0x30)*1000 + (gBattVoltSet[5]-0x30)*100  + (gBattVoltSet[6]-0x30)*10 + (gBattVoltSet[7]-0x30)*1;
            BattLowVoltage = tmpvalue * 0.1;  //  xxxx -> xxx.x
                    
			Set_OK = 1;
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
	}
}

void DispInputVoltType()
{
	setcurLCD(0, 0);
	putrsLCD(" **Input Volt** ");
	setcurLCD(0, 1);
	if(gInVoltMode == 0)	putrsLCD("380VAC       *  ");
	else	putrsLCD("380VAC          ");
	setcurLCD(0, 2);	
	putrsLCD("                ");
	setcurLCD(0, 3);
	if(gInVoltMode == 0)	putrsLCD("220VAC          ");
	else	putrsLCD("220VAC       *  ");
	switch(hKey())
	{
		case keyKEY_UP:
			iMenuIndex = M3_BATTTYPE;
			break;
		case keyKEY_UPfast:
			break;	
		case keyKEY_DOWN:
			iMenuIndex = M3_OPERATIONMODE;
			break;
		case keyKEY_DOWNfast:
			inputCash--;
			break;
		case keyKEY_LEFT:
			iMenuIndex = M3_System;
			break;
		case keyKEY_ENTER:
			iMenuIndex = M4_INPUTVOLTSET;
			iMenuCnt = gInVoltMode;
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
	}
}

void DispInputVoltTypeSet()
{
	//UINT8 i;
	
	setcurLCD(0, 0);
	putrsLCD(" **Input Volt** ");
	setcurLCD(0, 1);
	putrsLCD("380VAC          ");
	setcurLCD(0, 2);	
	putrsLCD("                ");
	setcurLCD(0, 3);
	putrsLCD("220VAC          ");
	
	switch(iMenuCnt)
	{
		case 0 :setcurLCD(13, 1);
				putrsLCD("*<-");
				break;
		case 1 :setcurLCD(13, 3);
				putrsLCD("*<-");
				break;
		default :
				break;
	}
	switch(hKey())
	{
		case keyKEY_UP:			
			iMenuCnt=0;
			break;
		case keyKEY_UPfast:
			break;	
		case keyKEY_DOWN:
			iMenuCnt=1;
			break;
		case keyKEY_DOWNfast:
			inputCash--;
			break;
		case keyKEY_LEFT:
			iMenuIndex = M3_INPUTVOLT;
			break;
		case keyKEY_ENTER:
			gInVoltMode=iMenuCnt;
			Set_OK=1;
			iMenuIndex = M3_INPUTVOLT;
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
	}
}

void DispOperationMode()
{
    gOperationMode = read_EE(OP_MODE);
	setcurLCD(0, 0);
	putrsLCD("*Operation Type*");
	setcurLCD(0, 1);
	if(gOperationMode == 0)	putrsLCD("Stand-Alone  *  ");
	else	putrsLCD("Stand-Alone     ");
	setcurLCD(0, 2);	
	putrsLCD("                ");
	setcurLCD(0, 3);
	if(gOperationMode == 0)	putrsLCD("Load-Share      ");
	else	putrsLCD("Load-Share   *  ");
	switch(hKey())
	{
		case keyKEY_UP:
			iMenuIndex = M3_BATTTYPE;
			break;
		case keyKEY_UPfast:
			break;	
		case keyKEY_DOWN:
			iMenuIndex = M3_SYSTYPE;
			break;
		case keyKEY_DOWNfast:
			inputCash--;
			break;
		case keyKEY_LEFT:
			iMenuIndex = M3_System;
			break;
		case keyKEY_ENTER:
			iMenuIndex = M4_OPERATIONMODESET;
			iMenuCnt = gOperationMode;
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
	}
}

void DispOperationModeSet()
{
	//UINT8 i;
	
	setcurLCD(0, 0);
	putrsLCD("*Operation Type*");
	setcurLCD(0, 1);
	putrsLCD("Stand-Alone     ");
	setcurLCD(0, 2);	
	putrsLCD("                ");
	setcurLCD(0, 3);
	putrsLCD("Load-Share      ");
	
	switch(iMenuCnt)
	{
		case 0 :setcurLCD(13, 1);
				putrsLCD("*<-");
				break;
		case 1 :setcurLCD(13, 3);
				putrsLCD("*<-");
				break;
		default :
				break;
	}
	switch(hKey())
	{
		case keyKEY_UP:			
			iMenuCnt=0;
			break;
		case keyKEY_UPfast:
			break;	
		case keyKEY_DOWN:
			iMenuCnt=1;
			break;
		case keyKEY_DOWNfast:
			inputCash--;
			break;
		case keyKEY_LEFT:
			iMenuIndex = M3_OPERATIONMODE;
			break;
		case keyKEY_ENTER:
			gOperationMode=iMenuCnt;
            write_EE(OP_MODE, gOperationMode);
			Set_OK=1;
			iMenuIndex = M3_OPERATIONMODE;
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
	}
}

void DispSysType()
{
    gSysTypeMode = read_EE(SYS_TYPE);
	setcurLCD(0, 0);
	putrsLCD("  **SYS Type**  "); 
	setcurLCD(0, 1);
	if(gSysTypeMode == 0)	putrsLCD("Dual Module  *  ");
	else	putrsLCD("Dual Module     ");
	setcurLCD(0, 2);	
	putrsLCD("                ");
	setcurLCD(0, 3);
	if(gSysTypeMode == 0)	putrsLCD("Single Module   ");
	else	putrsLCD("Single Module*  ");
	switch(hKey())
	{
		case keyKEY_UP:
			iMenuIndex = M3_OPERATIONMODE;
			break;
		case keyKEY_UPfast:
			break;	
		case keyKEY_DOWN:
			iMenuIndex = M3_BATTTYPE;
			break;
		case keyKEY_DOWNfast:
			inputCash--;
			break;
		case keyKEY_LEFT:
			iMenuIndex = M3_System;
			break;
		case keyKEY_ENTER:
			iMenuIndex = M4_SYSTYPESET;
			iMenuCnt = gSysTypeMode;
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
	}
}

void DispSysTypeSet()
{
	//UINT8 i;
	setcurLCD(0, 0);
	putrsLCD("  **SYS Type**  ");
	setcurLCD(0, 1);
	putrsLCD("Dual Module     ");
	setcurLCD(0, 2);	
	putrsLCD("                ");
	setcurLCD(0, 3);
	putrsLCD("Single Module   ");
	
	switch(iMenuCnt)
	{
		case 0 :setcurLCD(13, 1);
				putrsLCD("*<-");
				break;
		case 1 :setcurLCD(13, 3);
				putrsLCD("*<-");
				break;
		default :
				break;
	}
	switch(hKey())
	{
		case keyKEY_UP:			
			iMenuCnt=0;
			break;
		case keyKEY_UPfast:
			break;	
		case keyKEY_DOWN:
			iMenuCnt=1;
			break;
		case keyKEY_DOWNfast:
			break;
		case keyKEY_LEFT:
			iMenuIndex = M3_SYSTYPE;
			break;
		case keyKEY_ENTER:
			gSysTypeMode=iMenuCnt;
            write_EE(SYS_TYPE, gSysTypeMode);
			Set_OK=1;
			iMenuIndex = M3_SYSTYPE;
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
	}
}

void AlarmHistoryDisplay(UINT8 index)
{
	UINT8 a, b ,c,tmp;
	//
	setcurLCD(0, 0);
	putrsLCD("**AlarmHistory**");
	
	setcurLCD(0, 1);
	putrsLCD("   <   /   >    ");
	
	setcurLCD(0, 3);	
	putrsLCD("   /  /     :   ");
	
	setcurLCD(0, 2);
	switch(gAlarmHistory[index][7])
	{
		case 11:
			putrsLCD("#1 In. High Volt");
			break;
		case 12:
			putrsLCD("#1 In. Low  Volt");
			break;
		case 13:
			putrsLCD("   #1 AC Fail   ");
			break;
		case 14:
			putrsLCD("#1 Out.High Volt");
			break;
		case 15:
			putrsLCD("#1 Out.Low  Volt");
			break;
		case 16:
			putrsLCD("#1 Out.Over Curr");
			break;
		case 17:
			putrsLCD("  #1 Over Temp  ");
			break;
		case 21:
			putrsLCD("#2 In. High Volt");
			break;
		case 22:
			putrsLCD("#2 In. Low  Volt");
			break;
		case 23:
			putrsLCD("   #2 AC Fail   ");
			break;
		case 24:
			putrsLCD("#2 Out.High Volt");
			break;
		case 25:
			putrsLCD("#2 Out.Low  Volt");
			break;                       
		case 26:
			putrsLCD("#2 Out.Over Curr");
			break;                       
		case 27:
			putrsLCD("  #2 Over Temp  ");
			break;                       
		case 51:
			putrsLCD("Battery HighVolt");
			break;
		case 52:
			putrsLCD("Battery Low Volt");
			break;
		case 61:
			putrsLCD("#1 Out.High Alm ");
			break;
		case 62:
			putrsLCD("#1 Out.Low Alm  ");
			break;
		case 63:
			putrsLCD("#2 Out.High Alm ");
			break;
		case 64:
			putrsLCD("#2 Out.Low Alm  ");
			break;
		case 71:
			putrsLCD("   #1 Startup   ");
			break;
		case 72:
			putrsLCD("   #2 Startup   ");
			break;
        case 81:
			putrsLCD("#1 FLOAT Charge ");
			break;
		case 82:
			putrsLCD("#1 EQUAL Charge ");
			break;
        case 83:
			putrsLCD("#2 FLOAT Charge ");
			break;
		case 84:
			putrsLCD("#2 EQUAL Charge ");
			break;
   		default :
			break;
	}	
	//
	tmp=index;	//gAlarmHistoryCnt;
	a = tmp / 100;
	b = (tmp - a*100)/10;
	c = (tmp - a*100)-(b*10);
	setcurLCD(4, 1);
	//put_Num3_LCD(index);
	putcLCD(a + 0x30);	//4
	putcLCD(b + 0x30);	//5
	putcLCD(c + 0x30);	//6
	putcLCD('/');		//7
	//
	a=(gAlarmHistoryCnt%1000)/100;
	b=(gAlarmHistoryCnt%100)/10;
	c=gAlarmHistoryCnt%10;
	setcurLCD(8, 1);
	putcLCD(a + 0x30);	//8
	putcLCD(b + 0x30);	//9
	putcLCD(c + 0x30);	//10
	//------------------------------
	a = gAlarmHistory[index][1] / 10;
	b = gAlarmHistory[index][1] - a*10;
	setcurLCD(1, 3);
	putcLCD(a + 0x30);
	setcurLCD(2, 3);
	putcLCD(b + 0x30);
	a = gAlarmHistory[index][2] / 10;
	b = gAlarmHistory[index][2] - a*10;
	setcurLCD(4, 3);
	putcLCD(a + 0x30);
	setcurLCD(5, 3);
	putcLCD(b + 0x30);
	a = gAlarmHistory[index][3] / 10;
	b = gAlarmHistory[index][3] - a*10;
	setcurLCD(7, 3);
	putcLCD(a + 0x30);
	setcurLCD(8, 3);
	putcLCD(b + 0x30);
	a = gAlarmHistory[index][4] / 10;
	b = gAlarmHistory[index][4] - a*10;
	setcurLCD(10, 3);
	putcLCD(a + 0x30);
	setcurLCD(11, 3);
	putcLCD(b + 0x30);
	a = gAlarmHistory[index][5] / 10;
	b = gAlarmHistory[index][5] - a*10;
	setcurLCD(13, 3);
	putcLCD(a + 0x30);
	setcurLCD(14, 3);
	putcLCD(b + 0x30);
}

void AlarmOccurrenceApply(unsigned int packetsize)
{
	//UINT8 i;
	//if((prev_alarm_name != ErrCode)	&& (gAlarmHistory[gAlarmHistoryCnt-1][7] != ErrCode))// && (gAlarmHistory[gAlarmHistoryCnt-2][7] != errCnt))		//(prev_alarm_status !=	RxBuf[41]))
	if(ErrCode != 0 && prev_alarm_name != ErrCode)
	{
	//----------------------------------------------------------
		rtc_get_time();
		rtc_get_date();
		//
		ErrCnt=ErrCode;
		if(gAlarmHistoryCnt == 150)	gAlarmHistoryCnt = 0;
		else	gAlarmHistoryCnt++;
		
		gAlarmHistory[gAlarmHistoryCnt][0] = 1;
		gAlarmHistory[gAlarmHistoryCnt][1] = year;
		gAlarmHistory[gAlarmHistoryCnt][2] = mth;
		gAlarmHistory[gAlarmHistoryCnt][3] = day;
		gAlarmHistory[gAlarmHistoryCnt][4] = hour;
		gAlarmHistory[gAlarmHistoryCnt][5] = min;
		gAlarmHistory[gAlarmHistoryCnt][6] = sec;
		gAlarmHistory[gAlarmHistoryCnt][7] = ErrCnt;
		gAlarmHistory[gAlarmHistoryCnt][8] = gAlarmHistoryCnt;
		gAlarmHistory[gAlarmHistoryCnt][9] = gC_AlarmHistoryCnt;
		//
		write_EE(ALARM_STORAGE + (gAlarmHistoryCnt*10),     gAlarmHistory[gAlarmHistoryCnt][0]);
		write_EE(ALARM_STORAGE + (gAlarmHistoryCnt*10) + 1, gAlarmHistory[gAlarmHistoryCnt][1]);
		write_EE(ALARM_STORAGE + (gAlarmHistoryCnt*10) + 2, gAlarmHistory[gAlarmHistoryCnt][2]);
		write_EE(ALARM_STORAGE + (gAlarmHistoryCnt*10) + 3, gAlarmHistory[gAlarmHistoryCnt][3]);
		write_EE(ALARM_STORAGE + (gAlarmHistoryCnt*10) + 4, gAlarmHistory[gAlarmHistoryCnt][4]);
		write_EE(ALARM_STORAGE + (gAlarmHistoryCnt*10) + 5, gAlarmHistory[gAlarmHistoryCnt][5]);
		write_EE(ALARM_STORAGE + (gAlarmHistoryCnt*10) + 6, gAlarmHistory[gAlarmHistoryCnt][6]);
		write_EE(ALARM_STORAGE + (gAlarmHistoryCnt*10) + 7, gAlarmHistory[gAlarmHistoryCnt][7]);
		write_EE(ALARM_STORAGE + (gAlarmHistoryCnt*10) + 8, gAlarmHistory[gAlarmHistoryCnt][8]);
		write_EE(ALARM_STORAGE + (gAlarmHistoryCnt*10) + 9, gAlarmHistory[gAlarmHistoryCnt][9]);
		write_EE(ALARM_HISTORY_CNT, gAlarmHistoryCnt);
		//
		prev_alarm_name	= ErrCode;
		/*
		if(fNewErr == 11)
		{
			prev_Module1_Sts[1] = Module1_Sts[1];
		}
		else if(fNewErr == 12)
		{
			prev_Module1_Sts[2] = Module1_Sts[2];
		}
		else if(fNewErr == 13)
		{
			prev_Module1_Sts[3] = Module1_Sts[3];
		}
		else if(fNewErr == 14)
		{
			prev_Module1_Sts[4] = Module1_Sts[4];
		}
		else if(fNewErr == 15)
		{
			prev_Module1_Sts[5] = Module1_Sts[5];
		}
		else if(fNewErr == 16)
		{
			prev_Module1_Sts[6] = Module1_Sts[6];
		}
		else if(fNewErr == 17)
		{
			prev_Module1_Sts[7] = Module1_Sts[7];
		}
		//for(i=1;i<8;i++)
		//{
		//	prev_Module1_Sts[i] = Module1_Sts[i];
		//}
		*/
		switch(ErrCode)
		{
			case 11: prev_Module1_Sts[1] = Module1_Sts[1];break;
			case 12: prev_Module1_Sts[2] = Module1_Sts[2];break;
			case 13: prev_Module1_Sts[3] = Module1_Sts[3];break;
			case 14: prev_Module1_Sts[4] = Module1_Sts[4];break;
			case 15: prev_Module1_Sts[5] = Module1_Sts[5];break;
			case 16: prev_Module1_Sts[6] = Module1_Sts[6];break;
			case 17: prev_Module1_Sts[7] = Module1_Sts[7];break;
			case 21: prev_Module2_Sts[1] = Module2_Sts[1];break;
			case 22: prev_Module2_Sts[2] = Module2_Sts[2];break;
			case 23: prev_Module2_Sts[3] = Module2_Sts[3];break;
			case 24: prev_Module2_Sts[4] = Module2_Sts[4];break;
			case 25: prev_Module2_Sts[5] = Module2_Sts[5];break;
			case 26: prev_Module2_Sts[6] = Module2_Sts[6];break;
			case 27: prev_Module2_Sts[7] = Module2_Sts[7];break;
			case 51: prev_Battery_Sts[0] = Battery_Sts[0];break;
			case 52: prev_Battery_Sts[1] = Battery_Sts[1];break;
			case 61: prev_Warning_Sts[0] = Warning_Sts[0];break;
			case 62: prev_Warning_Sts[1] = Warning_Sts[1];break;
			case 63: prev_Warning_Sts[2] = Warning_Sts[2];break;
			case 64: prev_Warning_Sts[3] = Warning_Sts[3];break;
			case 71: prev_ModuleStartup_Sts[0] = ModuleStartup_Sts[0];break;
			case 72: prev_ModuleStartup_Sts[1] = ModuleStartup_Sts[1];break;
            case 81: prev_Module_CHG_Sts[1] = Module_CHG_Sts[1];break;
			case 82: prev_Module_CHG_Sts[1] = Module_CHG_Sts[1];break;
			case 83: prev_Module_CHG_Sts[3] = Module_CHG_Sts[3];break;
			case 84: prev_Module_CHG_Sts[3] = Module_CHG_Sts[3];break;  
//          case 85: prev_Module_CHG_Sts[5] = Module_CHG_Sts[5];break;
//			case 86: prev_Module_CHG_Sts[5] = Module_CHG_Sts[5];break;
//			case 87: prev_Module_CHG_Sts[7] = Module_CHG_Sts[7];break;
//			case 88: prev_Module_CHG_Sts[7] = Module_CHG_Sts[7];break;
		}
		ErrCode = 0;
	}
}

void DispTime()
{
	UINT8 a, b;		//, c, d;
	if(fTimeSet == 0)
	{
		setcurLCD(0, 0);
		putrsLCD("**Time & Date** ");
	}
	else if(fTimeSet == 1)
	{
		setcurLCD(0, 0);
		putrsLCD("Time & Date Set ");
	}
	setcurLCD(0, 1);
	putrsLCD("                ");
	//
	setcurLCD(0, 2);
	putrsLCD("Date :20  /  /  ");
	setcurLCD(8, 2);
	put_Num_LCD(year);
	setcurLCD(11, 2);
	put_Num_LCD(mth);
	setcurLCD(14, 2);
	put_Num_LCD(day);
	//
	setcurLCD(0, 3);
	putrsLCD("Time :  :  :    ");
	setcurLCD(6, 3);
	put_Num_LCD(hour);
	setcurLCD(9, 3);
	put_Num_LCD(min);
	//setcurLCD(12, 3);
	//put_Num_LCD(sec);
	a = sec / 10;
	b = sec - a*10;
	setcurLCD(12, 3);
	putcLCD(a + 0x30);
	putcLCD(b + 0x30);
	//
	rtc_get_date();
	rtc_get_time();
	//
	switch(hKey())
	{
		case keyKEY_UP:
			if(fTimeSet == 0)	iMenuIndex = M0_HISTORY;
			else if(fTimeSet == 1)	iMenuIndex = M1_MODULERESTART;
			break;
		case keyKEY_DOWN:
			if(fTimeSet == 0)	iMenuIndex = M0_LOGO;
			else if(fTimeSet == 1)	iMenuIndex = M1_SYSTEMINIT;
			break;
		case keyKEY_LEFT:
			if(fTimeSet == 0)	break;
			else if(fTimeSet == 1)	iMenuIndex = M0_SETUP;
			break;
		case keyKEY_RIGHT:
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
		case keyKEY_ENTER:
			if(fTimeSet == 0)	break;
			else if(fTimeSet == 1)
			{
				gTmp=1;
				rtc_get_date();
				rtc_get_time();
				inputCash = year;
				iMenuIndex = M1_TIMESET;
			}
			break;
		default:
			break;
	}
}

void DispTimeSet(void)
{
	char a, b;
	//
	setcurLCD(0, 0);
	putrsLCD("Time & Date Set ");	
	setcurLCD(0, 1);
	putrsLCD("                ");
	//setcurLCD(0, 2);
	//putrsLCD("Date :20  :  :  ");
	setcurLCD(8, 2);
	put_Num_LCD(year);
	setcurLCD(11, 2);
	put_Num_LCD(mth);
	setcurLCD(14, 2);
	put_Num_LCD(day);
	setcurLCD(6, 3);
	put_Num_LCD(hour);
	setcurLCD(9, 3);
	put_Num_LCD(min);
	setcurLCD(12, 3);
	put_Num_LCD(sec);
	a = sec / 10;
	b = sec - a*10;
	setcurLCD(12, 3);
	putcLCD(a + 0x30);
	putcLCD(b + 0x30);
	//
	switch(gTmp)
	{
		case 1: setcurLCD(5, 2);
				putrsLCD(">");
				break;
		case 2: setcurLCD(5, 2);
				putrsLCD(":");
				setcurLCD(10, 2);
				putrsLCD(">");
				break;
		case 3: setcurLCD(10, 2);
				putrsLCD("/");
				setcurLCD(13, 2);
				putrsLCD(">");
				break;
		case 4: setcurLCD(13, 2);
				putrsLCD("/");
				setcurLCD(5, 3);
				putrsLCD(">");
				break;
		case 5: setcurLCD(5, 3);
				putrsLCD(":");
				setcurLCD(8, 3);
				putrsLCD(">");
				break;
		case 6: setcurLCD(8, 3);
				putrsLCD(":");
				setcurLCD(11, 3);
				putrsLCD(">");
				break;
		//default : break;
	}
	switch(hKey())
	{
		case keyKEY_UP:
			inputCash++;
			break;
		case keyKEY_UPfast:
			inputCash++;
			break;	
		case keyKEY_DOWN:
			inputCash--;
			break;
		case keyKEY_DOWNfast:
			inputCash--;
			break;	
		case keyKEY_ENTER:
			if(gTmp == 6)
			{
				inputCash=0;
				rtc_set_datetime(day, mth, year, 0, hour, min);
				iMenuIndex = M0_TIME;
			}
			else if(gTmp == 5)
			{
				inputCash=sec;
				gTmp=6;
			}
			else if(gTmp == 4)
			{
				inputCash=min;
				gTmp=5;
			}
			else if(gTmp == 3)
			{
				inputCash=hour;
				gTmp=4;
			}
			else if(gTmp == 2)
			{
				inputCash=day;
				gTmp=3;
			}
			else if(gTmp == 1)
			{
				inputCash=mth;
				gTmp=2;
			}
			//return;
			break;
		case keyKEY_BZ_OFF:
			break;
		case keyKEY_BZ_OFFfast:
			break;
	}
	//
	if(gTmp == 1)
	{
		if(inputCash > 99) inputCash = 0;
		if(inputCash < 0) inputCash = 0;
		year = inputCash;
	}
	else if(gTmp == 2)
	{
		if(inputCash > 12) inputCash = 1;
		if(inputCash < 1) inputCash = 1;
		mth = inputCash;
	}
	else if(gTmp == 3)
	{
		if(inputCash > 31) inputCash = 1;
		if(inputCash < 1) inputCash = 1;
		day = inputCash;
	}
	else if(gTmp == 4)
	{
		if(inputCash > 23) inputCash = 0;
		if(inputCash < 0) inputCash = 0;
		hour = inputCash;
	}
	else if(gTmp == 5)
	{
		if(inputCash > 59) inputCash = 0;
		if(inputCash < 0) inputCash = 0;
		min = inputCash;
	}
	else if(gTmp == 6)
	{
		if(inputCash > 59) inputCash = 0;
		if(inputCash < 0) inputCash = 0;
		sec = inputCash;
	}

}
/*
void SelectModule()
{
	if(SetKeyOk == 1)// && fKey == 1)
	{
		switch(hKey())
		{
			case keyKEY_NO_1:
				{
					NO_1=ON;
					NO_2=OFF;
					gSelectModule = 1;
					Set_OK=1;
				}
				break;
			case keyKEY_NO_2:
				{
					NO_2=ON;
					NO_1=OFF;
					gSelectModule = 2;
					Set_OK=1;
				}
				break;
			default:
				break;
		}
	}
}
*/

void AutoManual()
{
	if(uc_key == KEY_AUTOMANUAL)
	{
		key1s=0;
		uc_key = 0;
		if(fKeyAutoManual == 0)
		{
			fKeyAutoManual = 1;
			gAutoManual ^= 1;
		}
	}
	else
	{
		fKeyAutoManual=0;
	}
	if(gAutoManual == 0)
	{
		AUTO = ON;
		MANUAL = OFF;
	}
	else
	{
		AUTO = OFF;
		MANUAL = ON;
	}
}

void SelectModule()
{
	if(gAutoManual == 1)
	{
		if(uc_key == KEY_NO_SELECT)	// || (uc_key == KEY_NO_2))
		{
			key1s=0;
			uc_key = 0;
			if(fKeyNoSelect == 0)
			{
				fKeyNoSelect = 1;
				if(gSelectModule == 1)
				{
					gSelectModule = 2;
				}
				else
				{
					gSelectModule = 1;
				}
				Set_OK=1;
			}
		}
		else
		{
			fKeyNoSelect = 0;
		}
	}
	if(key1s>=5)
	{
		key1s=0;
		if(Module1_Sts[0] == 1)		gSelectModule = 1;
		else if(Module2_Sts[0] == 1)	gSelectModule = 2;
	}
}

void LedStatus()
{
	//UINT8 i=0;
	UINT8 NfbCheckErr=0;
	StatusBuf[0] = (LedState[0]&0x80)>>7;
	StatusBuf[1] = (LedState[0]&0x40)>>6;
	StatusBuf[2] = (LedState[0]&0x20)>>5;
	StatusBuf[3] = (LedState[0]&0x10)>>4;
	StatusBuf[4] = (LedState[0]&0x08)>>3;
	StatusBuf[5] = (LedState[0]&0x04)>>2;
	StatusBuf[6] = (LedState[0]&0x02)>>1;
	StatusBuf[7] = (LedState[0]&0x01);
	if(StatusBuf[0]==1)	M1_NFB=ON;
	else M1_NFB=OFF;
    if(StatusBuf[4]==1)	DC1_NFB=ON;
	else DC1_NFB=OFF;
    
	if(StatusBuf[1]==1)	M2_NFB=ON;
	else M2_NFB=OFF;
	if(StatusBuf[5]==1)	DC2_NFB=ON;
	else DC2_NFB=OFF;
    
    
	//
	StatusBuf[8] = (LedState[1]&0x80)>>7;
	StatusBuf[9] = (LedState[1]&0x40)>>6;
	StatusBuf[10] = (LedState[1]&0x20)>>5;
	StatusBuf[11] = (LedState[1]&0x10)>>4;
	StatusBuf[12] = (LedState[1]&0x08)>>3;
	StatusBuf[13] = (LedState[1]&0x04)>>2;
	StatusBuf[14] = (LedState[1]&0x02)>>1;
	StatusBuf[15] = (LedState[1]&0x01);
	if(StatusBuf[8]==1)	BATT_NFB=ON;
	else BATT_NFB=OFF;
	if(StatusBuf[11]==1)	LOAD_NFB=ON;
	else LOAD_NFB=OFF;
	//
    if(gSysTypeMode == 0)
    {
        NfbCheckErr = (StatusBuf[0] &= StatusBuf[1] &= StatusBuf[4] &= StatusBuf[5] &= StatusBuf[8] &= StatusBuf[11]);
    }
    else
    {
        NfbCheckErr = (StatusBuf[0] &= StatusBuf[4] &= StatusBuf[8] &= StatusBuf[11]);
    }
	if(NfbCheckErr != 1)
	{
		NfbCheckErr=1;
		fCheckErr=1;
		//fNfbCheckErr=1;
		FAIL=ON;
		NORMAL=OFF;
	}
	else 
	{
		if(fCheckErr == 1)		// || fNfbCheckErr == 1)
		{
			FAIL=ON;
			NORMAL=OFF;		
		}
		else
		{
			NfbCheckErr=1;
			FAIL=OFF;
			NORMAL=ON;
		}
	}
	/*
	StatusBuf[8] = (LedState[1]&0x80)>>7;
	StatusBuf[9] = (LedState[1]&0x40)>>6;
	StatusBuf[10] = (LedState[1]&0x20)>>5;
	StatusBuf[11] = (LedState[1]&0x10)>>4;
	StatusBuf[12] = (LedState[1]&0x08)>>3;
	StatusBuf[13] = (LedState[1]&0x04)>>2;
	StatusBuf[14] = (LedState[1]&0x02)>>1;
	StatusBuf[15] = (LedState[1]&0x01);
	if(StatusBuf[8]==1)	BATT_NFB=ON;
	else BATT_NFB=OFF;
	if(StatusBuf[11]==1)	LOAD_NFB=ON;
	else LOAD_NFB=OFF;
	*/
	/*	//SPARE
	if(LedBuf[12]==1)	DC1_NFB=ON;
	else DC1_NFB=OFF;
	if(LedBuf[13]==1)	DC2_NFB=ON;
	else DC2_NFB=OFF;
	if(LedBuf[14]==1)	DC3_NFB=ON;
	else DC3_NFB=OFF;
	if(LedBuf[15]==1)	DC4_NFB=ON;
	else DC4_NFB=OFF;
	*/
	//NFB Status Setting Data transmit
}

void ModuleStatus()
{
	UINT8 i=0;
	UINT8 ModuleCheckErr=0;
	//#1 Module Status
	Module1_Sts[0] = (ModuleState[0]&0x80)>>7;
	Module1_Sts[1] = (ModuleState[0]&0x40)>>6;
	Module1_Sts[2] = (ModuleState[0]&0x20)>>5;
	Module1_Sts[3] = (ModuleState[0]&0x10)>>4;
	Module1_Sts[4] = (ModuleState[0]&0x08)>>3;
	Module1_Sts[5] = (ModuleState[0]&0x04)>>2;
	Module1_Sts[6] = (ModuleState[0]&0x02)>>1;
	Module1_Sts[7] = (ModuleState[0]&0x01);
	//#2 Module Status
	Module2_Sts[0] = (ModuleState[1]&0x80)>>7;
	Module2_Sts[1] = (ModuleState[1]&0x40)>>6;
	Module2_Sts[2] = (ModuleState[1]&0x20)>>5;
	Module2_Sts[3] = (ModuleState[1]&0x10)>>4;
	Module2_Sts[4] = (ModuleState[1]&0x08)>>3;
	Module2_Sts[5] = (ModuleState[1]&0x04)>>2;
	Module2_Sts[6] = (ModuleState[1]&0x02)>>1;
	Module2_Sts[7] = (ModuleState[1]&0x01);

    if(gSysTypeMode == 0)
    {
        if(Module1_Sts[0]==1)	NO_1=ON,M1_LINE=ON,OUT_LINE=ON,NO_2=OFF,M2_LINE=OFF;	// 01 : #1 Module Run
        //else NO_1=OFF,M1_LINE=OFF;                                // 00 : #1 Module Stop
        if(Module2_Sts[0]==1)	NO_2=ON,M2_LINE=ON,OUT_LINE=ON,NO_1=OFF,M1_LINE=OFF;	// 03 : #2 Module Run
        //else NO_2=OFF,M2_LINE=OFF;								// 02 : #2 Module Stop
        for(i = 1; i < 8; i++)
        {
            ModuleCheckErr |= Module1_Sts[i];
            ModuleCheckErr |= Module2_Sts[i];
        }
    }
    else
    {
        if(Module1_Sts[0]==1)	NO_1=ON,M1_LINE=ON,OUT_LINE=ON,NO_2=OFF,M2_LINE=OFF;	// 01 : #1 Module Run
        //else NO_1=OFF,M1_LINE=OFF;							// 00 : #1 Module Stop
        for(i = 1; i < 8; i++)
        {
            ModuleCheckErr |= Module1_Sts[i];
        }
    }
        
	if(ModuleCheckErr != 0)
	{
		ModuleCheckErr=0;
		fCheckErr=1;
		FAIL=ON;
		NORMAL=OFF;
	}
	else 
	{
		if(fCheckErr == 1 || fNfbCheckErr == 1)
		{
			FAIL=ON;
			NORMAL=OFF;		
		}
		else
		{
			ModuleCheckErr=0;
			FAIL=OFF;
			NORMAL=ON;
		}
	}

	if(Module1_Sts[1] != prev_Module1_Sts[1])
	{
		if(Module1_Sts[1] == 1)
		{
			ErrCode=11;
			AlarmOccurrenceApply(gPacketsize);
		}
		else if(prev_Module1_Sts[1] == 1)
		{
			prev_Module1_Sts[1]=0;
		}
	}
	else if(Module1_Sts[2] != prev_Module1_Sts[2])
	{
		if(Module1_Sts[2] == 1)
		{
			ErrCode=12;
			AlarmOccurrenceApply(gPacketsize);
		}
		else if(prev_Module1_Sts[2] == 1)
		{
			prev_Module1_Sts[2]=0;
		}
	}
	else if(Module1_Sts[3] != prev_Module1_Sts[3])
	{
		if(Module1_Sts[3] == 1)
		{
			ErrCode=13;
			AlarmOccurrenceApply(gPacketsize);
		}
		else if(prev_Module1_Sts[3] == 1)
		{
			prev_Module1_Sts[3]=0;
		}
	}
	else if(Module1_Sts[4] != prev_Module1_Sts[4])
	{
		if(Module1_Sts[4] == 1)
		{
			ErrCode=14;
			AlarmOccurrenceApply(gPacketsize);
		}
		else if(prev_Module1_Sts[4] == 1)
		{
			prev_Module1_Sts[4]=0;
		}
	}
	else if(Module1_Sts[5] != prev_Module1_Sts[5])
	{
		if(Module1_Sts[5] == 1)
		{
			ErrCode=15;
			AlarmOccurrenceApply(gPacketsize);
		}
		else if(prev_Module1_Sts[5] == 1)
		{
			prev_Module1_Sts[5]=0;
		}
	}
	else if(Module1_Sts[6] != prev_Module1_Sts[6])
	{
		if(Module1_Sts[6] == 1)
		{
			ErrCode=16;
			AlarmOccurrenceApply(gPacketsize);
		}
		else if(prev_Module1_Sts[6] == 1)
		{
			prev_Module1_Sts[6]=0;
		}
	}
	else if(Module1_Sts[7] != prev_Module1_Sts[7])
	{
		if(Module1_Sts[7] == 1)
		{
			ErrCode=17;
			AlarmOccurrenceApply(gPacketsize);
		}
		else if(prev_Module1_Sts[7] == 1)
		{
			prev_Module1_Sts[7]=0;
		}
	}
    
    if(gSysTypeMode == 0)
    {
        if(Module2_Sts[1] != prev_Module2_Sts[1])
        {
            if(Module2_Sts[1] == 1)
            {
                ErrCode=21;
                AlarmOccurrenceApply(gPacketsize);
            }
            else if(prev_Module2_Sts[1] == 1)
            {
                prev_Module2_Sts[1]=0;
            }
        }
        else if(Module2_Sts[2] != prev_Module2_Sts[2])
        {
            if(Module2_Sts[2] == 1)
            {
                ErrCode=22;
                AlarmOccurrenceApply(gPacketsize);
            }
            else if(prev_Module2_Sts[2] == 1)
            {
                prev_Module2_Sts[2]=0;
            }
        }
        else if(Module2_Sts[3] != prev_Module2_Sts[3])
        {
            if(Module2_Sts[3] == 1)
            {
                ErrCode=23;
                AlarmOccurrenceApply(gPacketsize);
            }
            else if(prev_Module2_Sts[3] == 1)
            {
                prev_Module2_Sts[3]=0;
            }
        }
        else if(Module2_Sts[4] != prev_Module2_Sts[4])
        {
            if(Module2_Sts[4] == 1)
            {
                ErrCode=24;
                AlarmOccurrenceApply(gPacketsize);
            }
            else if(prev_Module2_Sts[4] == 1)
            {
                prev_Module2_Sts[4]=0;
            }
        }
        else if(Module2_Sts[5] != prev_Module2_Sts[5])
        {
            if(Module2_Sts[5] == 1)
            {
                ErrCode=25;
                AlarmOccurrenceApply(gPacketsize);
            }
            else if(prev_Module2_Sts[5] == 1)
            {
                prev_Module2_Sts[5]=0;
            }
        }
        else if(Module2_Sts[6] != prev_Module2_Sts[6])
        {
            if(Module2_Sts[6] == 1)
            {
                ErrCode=26;
                AlarmOccurrenceApply(gPacketsize);
            }
            else if(prev_Module2_Sts[6] == 1)
            {
                prev_Module2_Sts[6]=0;
            }
        }
        else if(Module2_Sts[7] != prev_Module2_Sts[7])
        {
            if(Module2_Sts[7] == 1)
            {
                ErrCode=27;
                AlarmOccurrenceApply(gPacketsize);
            }
            else if(prev_Module2_Sts[7] == 1)
            {
                prev_Module2_Sts[7]=0;
            }
        }
    }
}

void BatteryStatus()
{
	UINT8 i=0;
	UINT8 BattCheckErr=0;
	Battery_Sts[0] = (BatteryState[0]&0x80)>>7;
	Battery_Sts[1] = (BatteryState[0]&0x40)>>6;
	Battery_Sts[2] = (BatteryState[0]&0x20)>>5;
	//Battery_Sts[3] = (BatteryState[0]&0x10)>>4;
	for(i = 0; i < 2; i++)
	{
		BattCheckErr |= Battery_Sts[i];
	}
	if(BattCheckErr != 0)
	{
		BattCheckErr=0;
		fCheckErr=1;
		FAIL=ON;
		NORMAL=OFF;
	}
	else 
	{
		if(fCheckErr == 1)
		{
			FAIL=ON;
			NORMAL=OFF;
		}
		else
		{
			BattCheckErr=0;
			FAIL=OFF;
			NORMAL=ON;
		}
	}
	if(Battery_Sts[2]==0)	fCharge=' ';			//Charge
	else if(Battery_Sts[2]==1)	fCharge='-';	//Discharge
	//
	if(Battery_Sts[0] != prev_Battery_Sts[0])
	{
		if(Battery_Sts[0] == 1)
		{
			ErrCode=51;
			AlarmOccurrenceApply(gPacketsize);
		}
		else if(prev_Battery_Sts[0] == 1)
		{
			prev_Battery_Sts[0]=0;
		}
	}
	else if(Battery_Sts[1] != prev_Battery_Sts[1])
	{
		if(Battery_Sts[1] == 1)
		{
			ErrCode=52;
			AlarmOccurrenceApply(gPacketsize);
		}
		else if(prev_Battery_Sts[1] == 1)
		{
			prev_Battery_Sts[1]=0;
		}
	}
}

void WarningStatus()
{
	UINT8 i=0;
	UINT8 WarningCheckErr=0;
	Warning_Sts[0] = (WarningState[0]&0x80)>>7;
	Warning_Sts[1] = (WarningState[0]&0x40)>>6;
	Warning_Sts[2] = (WarningState[0]&0x20)>>5;
	Warning_Sts[3] = (WarningState[0]&0x10)>>4;
	Warning_Sts[4] = (WarningState[0]&0x08)>>3;
	Warning_Sts[5] = (WarningState[0]&0x04)>>2;
	Warning_Sts[6] = (WarningState[0]&0x02)>>1;
	Warning_Sts[7] = (WarningState[0]);
	for(i = 0; i < 4; i++)
	{
		WarningCheckErr |= Warning_Sts[i];
	}
	if(WarningCheckErr != 0)
	{
		WarningCheckErr=0;
		fCheckErr=1;
		FAIL=ON;
		NORMAL=OFF;
	}
	else 
	{
		if(fCheckErr == 1)
		{
			FAIL=ON;
			NORMAL=OFF;
		}
		else
		{
			WarningCheckErr=0;
			FAIL=OFF;
			NORMAL=ON;
		}
	}
	if(Warning_Sts[0] != prev_Warning_Sts[0])
	{
		if(Warning_Sts[0] == 1)
		{
			ErrCode=61;
			AlarmOccurrenceApply(gPacketsize);
		}
		else if(prev_Warning_Sts[0] == 1)
		{
			prev_Warning_Sts[0]=0;
		}
	}
	else if(Warning_Sts[1] != prev_Warning_Sts[1])
	{
		if(Warning_Sts[1] == 1)
		{
			ErrCode=62;
			AlarmOccurrenceApply(gPacketsize);
		}
		else if(prev_Warning_Sts[1] == 1)
		{
			prev_Warning_Sts[1]=0;
		}
	}
	else if(Warning_Sts[2] != prev_Warning_Sts[2])
	{
		if(Warning_Sts[2] == 1)
		{
			ErrCode=63;
			AlarmOccurrenceApply(gPacketsize);
		}
		else if(prev_Warning_Sts[2] == 1)
		{
			prev_Warning_Sts[2]=0;
		}
	}
	else if(Warning_Sts[3] != prev_Warning_Sts[3])
	{
		if(Warning_Sts[3] == 1)
		{
			ErrCode=64;
			AlarmOccurrenceApply(gPacketsize);
		}
		else if(prev_Warning_Sts[3] == 1)
		{
			prev_Warning_Sts[3]=0;
		}
	}
}

void ModuleStartupStatus()
{
	ModuleStartup_Sts[0] = Module1_Sts[0];
	ModuleStartup_Sts[1] = Module2_Sts[0];
	if(ModuleStartup_Sts[0] != prev_ModuleStartup_Sts[0])
	{
		if(Module1_Sts[0] == 1)
		{
			ErrCode=71;
			AlarmOccurrenceApply(gPacketsize);
		}
		else if(prev_ModuleStartup_Sts[0] == 1)
		{
			prev_ModuleStartup_Sts[0]=0;
		}
	}
	else if(ModuleStartup_Sts[1] != prev_ModuleStartup_Sts[1])
	{
		if(Module2_Sts[0] == 1)
		{
			ErrCode=72;
			AlarmOccurrenceApply(gPacketsize);
		}
		else if(prev_ModuleStartup_Sts[1] == 1)
		{
			prev_ModuleStartup_Sts[1]=0;
		}
	}
}

void ChargeStatus(void)
{
	Module_CHG_Sts[0] = (ChargeState[0]&0x80)>>7;
	Module_CHG_Sts[1] = (ChargeState[0]&0x40)>>6;
	Module_CHG_Sts[2] = (ChargeState[0]&0x20)>>5;
	Module_CHG_Sts[3] = (ChargeState[0]&0x10)>>4;
	Module_CHG_Sts[4] = (ChargeState[0]&0x08)>>3;
	Module_CHG_Sts[5] = (ChargeState[0]&0x04)>>2;
	Module_CHG_Sts[6] = (ChargeState[0]&0x02)>>1;
	Module_CHG_Sts[7] = (ChargeState[0]);

	if(Module_CHG_Sts[1] != prev_Module_CHG_Sts[1])
	{
		if(Module_CHG_Sts[1] == 1)
		{
			ErrCode = 81; // float
			AlarmOccurrenceApply(gPacketsize);
		}
        else if(Module_CHG_Sts[1] == 0)
		{
			ErrCode = 82; // Equal
			AlarmOccurrenceApply(gPacketsize);
		}
	}
	if(Module_CHG_Sts[3] != prev_Module_CHG_Sts[3])
	{
		if(Module_CHG_Sts[3] == 1)
		{
			ErrCode = 83; // float
			AlarmOccurrenceApply(gPacketsize);
		}
        else if(Module_CHG_Sts[3] == 0)
		{
			ErrCode = 84; // Equal
			AlarmOccurrenceApply(gPacketsize);
		}
	}
}



