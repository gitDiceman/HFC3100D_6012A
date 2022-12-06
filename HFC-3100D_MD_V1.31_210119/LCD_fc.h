/*
**********************************************************
* PROJECT : HFC3100D MAIN DISPLAY
* FILE NAME: LCD_fc.h
* DEVICE : dsPIC30F6012A
* COMPILER : MPLAB-C
* REVISION HISTORY
* 1) 2018.02.28 (V1.0)
* 
********************************************************** */

//====================================================================================//
//                          Function Declaration                                      //
//====================================================================================//
// LCD Display
extern void DispInit(void);
extern void InitLCD(void);
extern void WriteCmdLCD(unsigned char LCD_CMD);
extern void setcurLCD(unsigned char CurX, unsigned char CurY);
extern void putrsLCD(const char *Str);
extern void putsLCD(char *Str);
extern void putcLCD(unsigned char LCD_Char);
extern void puthexLCD(unsigned char HEX_Val);
extern void put_Num_LCD(unsigned char The_Number );
extern void initLCD(void);
extern void put_Num3_LCD(unsigned int The_Number );
extern void LCD_Build_Char(unsigned char loc, unsigned char *p);
extern void WriteDataLCD( unsigned char LCD_CMD);
extern void LCD_CMD_W_Timing(void);
extern void LCD_DAT_W_Timing(void);
extern void LCD_L_Delay(void);
extern void LCD_S_Delay(void);

//Display
extern void MainProcess(void);
extern void DispLogo(void);
extern void DispOutBatt(void);
extern void DispLoad(void);
extern void DispSetup(void);
extern void SysSetup(void);
extern void SysSetupSet(void);
extern void DispSetupSet(void);
extern void DispBattAlarm(void);
extern void DispBattAlarmSet(void);
//extern void DispAutoManual(void);
//extern void DispAutoManualSet(void);
extern void DispModuleRestart(void);
extern void DispModuleRestartSet(void);
extern void DispSystemInit(void);
extern void DispSystemInitSet(void);
extern void DispPassword(void);
extern void DispPasswordSet(void);
extern void DispHistory(void);
extern void DispTime(void);
extern void DispTimeSet(void);
extern void DispAlarmStop(void);
extern void DispAlarmStopSet(void);
extern void DispBattType(void);
extern void DispBattTypeSet(void);
extern void DispInputVoltType(void);
extern void DispInputVoltTypeSet(void);
extern void DispOperationMode(void);
extern void DispOperationModeSet(void);
extern void DispSysType(void);
extern void DispSysTypeSet(void);

extern void SelectModule(void);
extern void AutoManual(void);

//ALARMHISTORY
extern void AlarmHistoryDisplay(UINT8 index);
extern void AlarmOccurrenceApply(unsigned int packetsize);
//RTC
extern void rtc_get_date();
extern void rtc_get_time();
//LED Status, Module Status
extern void LedStatus(void);
extern void ModuleStatus(void);
extern void BatteryStatus(void);
extern void WarningStatus(void);
extern void ModuleStartupStatus(void);
extern void ChargeStatus(void);
//Buzzer
extern void errBz_On(void);
//====================================================================================//
//                          Variable Declaration                                      //
//====================================================================================//
extern INT16 Temp_LCD_DATA ;
extern UINT8 Temp_CMD;
extern UINT8 Str_Temp;

//Display
extern UINT8 iMenuIndex;
extern UINT8 iMenuCnt;


//Switch
extern INT8 inputCash;
extern UINT8 passWord[10];
extern UINT8 fPassSet;

//AlarmHistory
extern UINT8 gAlarmHistory[200][9];
extern UINT8 gAlarmHistoryCnt;
extern UINT8 gC_AlarmHistoryCnt;
extern UINT8 prev_alarm_name;
extern UINT8 ErrCode;
extern UINT8 fCheckErr;
extern UINT8 fNfbCheckErr;
extern UINT8 errCnt;
extern UINT8 gPacketsize;
extern UINT8 Module1_Sts[8];
extern UINT8 Module2_Sts[8];
extern UINT8 Battery_Sts[8];
extern UINT8 Warning_Sts[8];
extern UINT8 Module_CHG_Sts[8];
extern UINT8 ModuleStartup_Sts[2];
extern UINT8 prev_Module1_Sts[8];
extern UINT8 prev_Module2_Sts[8];
extern UINT8 prev_Battery_Sts[8];
extern UINT8 prev_Warning_Sts[8];
extern UINT8 prev_ModuleStartup_Sts[2];
extern UINT8 prev_Module_CHG_Sts[8];
extern UINT8 fNewErr;


//LED STATUS
extern UINT8 StatusBuf[20];

//Time & Date
extern UINT8	fTimeSet;

//NFB STATUS
extern UINT8 fM1_NFB,fM2_NFB;

//Battery Charge,Discharge Flag
extern UINT8 fCharge;




