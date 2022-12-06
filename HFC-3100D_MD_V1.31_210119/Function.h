
////////////////////////////////////////
// PROJECT : HFC3100D(100A) Main LCD
// FILE NAME: Function.h
// DEVICE : dsPIC30F6012A
// COMPILER : MPLAB-C V3.02 
// REVISION HISTORY
// 1) 2018.2.12 (V1.0)
////////////////////////////////////////

//====================================================================================//
//                          Function Declaration                                      //
//====================================================================================//
//Display
extern void Delay10ms(void);		//10ms delay
extern void Delay2ms(void);
extern void Delay5ms(void);
extern void Delay500ms(void);
extern void Delay1s(void);
extern void LedAllOn(void);
extern void LedAllOff(void);
extern void InitledAllOn1(void);
extern void InitledAllOff1(void);
extern void InitledAllOn2(void);
extern void InitledAllOff2(void);
extern void InitledAllOn3(void);
extern void InitledAllOff3(void);
extern void InitSetValue(void);
extern void SystemInitialize(void);

//Key
extern void KeyDetecting(void);

//RTC function
extern void write_ds1302_byte(UINT8 cmd);
extern void write_ds1302(UINT8 cmd, UINT8 data);
extern void rtc_init(void);
extern void rtc_set_datetime(UINT8 day, UINT8 mth, UINT8 year, UINT8 dow, UINT8 hour, UINT8 min);
extern void rtc_get_date();
extern void rtc_get_time();
extern void rtc_write_nvr(UINT8 address, UINT8 data);

//====================================================================================//
//                          Variable Declaration                                      //
//====================================================================================//

//Global
extern UINT8 data_8,gCnt_8,buff_8;
extern UINT16 data_16,gCnt_16,buff_16;
extern UINT8 gCnt,gTmp;
extern UINT8 gErrCode;

//Main
extern UINT16 tmr1Count0,tmr1Count1,tmr1Count2,tmr1Count3,tmr1Count4;
extern UINT16 tmr2Count0,tmr2Count1,tmr2Count2,tmr2Count3,tmr2Count4;
extern UINT16 tmr50ms,tmr100ms,tmr200ms,tmr500ms,tmr1s;
extern UINT16 cursor100ms,cursor1s;
extern UINT8 errBz100ms;
extern UINT16 key1s;
//Display
extern UINT8 fAlarmStop;

//Key
extern UINT8 getKeyData(void);
extern UINT8 hKey(void);
extern UINT8 uc_key;
extern UINT8 KeyValue1;
extern UINT8 fKey;
extern UINT8 fKeyAutoManual;
extern UINT8 fKeyNoSelect;
extern UINT8 SetKeyOk;

//EEPROM
extern UINT8 gPass[10];
extern UINT8 SgPass[10];
extern UINT8 gBattVoltSet[10];
extern UINT8 gRestartTime[2];
extern int _EEDATA(2) EE_mem[2048];					//100% EEPROM memory
extern UINT8 gAutoManual;
extern UINT8 gSelectModule;
extern UINT8 gOperationMode;
extern UINT8 gBattTypeMode;
extern UINT8 gInVoltMode;
extern UINT8 gSysTypeMode;

//RTC
extern UINT8 read_ds1302(UINT8 cmd);
extern UINT8 get_bcd(UINT8 data);
extern UINT8 rm_bcd(UINT8 data);
extern UINT8 rtc_read_nvr(UINT8 address);
extern UINT8 day,mth,year,dow,hour,min,sec,sec_old;

//Flag

//Count
extern UINT8 Key_Cnt;

//Buzzer
extern UINT8 ferrBz[5];
//TEST
extern UINT16 Vvalue;
extern UINT16 Avalue;




