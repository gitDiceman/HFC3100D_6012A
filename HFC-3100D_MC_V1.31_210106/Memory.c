/*
******************************************************************************************
* PROJECT : HFC3000(100A) MAIN CONTROL
* FILE NAME: Memory.c
* DEVICE : dsPIC30F6012A, 
* COMPILER : MPLAB-C V3.02 
* DESCRIPTON : EEPROM ROUTINE, SETTING VALUE
* REVISION HISTORY
* 1) 2011.12.30 (V0.9)
* 
****************************************************************************************** */
#include 	<p30f6012A.h>		
#include 	"typedef.h"				//????? ??
#include 	"Memory.h"				//????? ? ??

//void Set_EEPROM();
//void EEPROM_Init(void);
//void System_Check(void);

/*
SYSTEM_INFO_MAIN:	
			DCB 'TITLE:HFC-3000 Main Control Program for Version 1.2'
			DCB 'Designed and programmed by , in 2011-12-01.'
			DCB 'Manufactured by Seoul Electric Power System Co., Ltd.'
SYSTEM_INFO_RECT:
			DCB 'TITLE:HFC-3000 Rectifier Display Program for Version 4.0'
			DCB 'Designed and programmed by , in 2009-08-11.'
SYSTEM_INFO_CELL:
		DCB 'TITLE:HFC-3000 Battery Cell Monitoring Program for Version 1.0'
		DCB 'Designed and programmed by , in 2009-04-30.'
*/			
//[xMxMC:Ver5.0,Lastdate:00-00-00
//       /RD:Ver0.0,Lastdate:00-00-00
//		  /BC:Ver0.0,Lastdate:00-00-00/x]
UINT8   FW_INFO[90]={
		'[', 'x', 'M', 'x', 'M', 'C', ':' ,'V' ,'e' ,'r' ,'1', '.', '2', ',', 						//MC V1.2
		'L', 'a', 's', 't', 'd', 'a','t', 'e', ':', '1', '2', '-', '0', '1', '-', '0',	'1', '/', 	//11-12-31
		'R', 'D', ':', 'V', 'e', 'r', '4', '.','2', ',', 												//RD V4.2
		'L', 'a', 's', 't', 'd', 'a', 't', 'e',':', '0', '9', '-', '0', '8', '-', '0', '1', '/',	//09-08-01
		'B', 'C', ':', 'V', 'e', 'r', '4', '.', '1', ',',												//BC V1.0
		'L', 'a', 's', 't', 'd', 'a', 't', 'e', ':', '0','9', '-', '0', '4', '-', '3', '0', '/',	//09-04-30
		 0x00, ']'};
				//
UINT8   BATT2V_CELL_DEF1[107]={
			'[',0x6b,'M',0x0b,'L',						// 5 BYTES
			'1','8','0','/','1','0','0','/','2','0','0','/','1','0','0','/',			//16 BYTES
			'3','0','0','/','1','0','0','/','2','0','0','/','3','0','0','/',			//16 BYTES
			'4','0','0','/','5','0','0','/','1','0','0','/','3','0','0','/',			//16 BYTES
			'2','0','0','/','2','0','0','/','2','0','0','/','2','0','0','/',			//16 BYTES
			'2','0','0','/','2','0','0','/','2','0','0','/','2','0','0','/',			//16 BYTES
			'2','0','0','/','2','0','0','/','2','0','0','/','2','0','0','/',			//16 BYTES
			'2','0','0','/',0x00,']'};													// 6 BYTES
			//
UINT8   BATT2V_CELL_DEF2[107]={	//battery Cell 2set add JSS 2017.06.27
			'[',0x6b,'M',0x37,'L',						// 5 BYTES
			'1','8','0','/','1','0','0','/','2','0','0','/','1','0','0','/',			//16 BYTES
			'3','0','0','/','1','0','0','/','2','0','0','/','3','0','0','/',			//16 BYTES
			'4','0','0','/','5','0','0','/','1','0','0','/','3','0','0','/',			//16 BYTES
			'2','0','0','/','2','0','0','/','2','0','0','/','2','0','0','/',			//16 BYTES
			'2','0','0','/','2','0','0','/','2','0','0','/','2','0','0','/',			//16 BYTES
			'2','0','0','/','2','0','0','/','2','0','0','/','2','0','0','/',			//16 BYTES
			'2','0','0','/',0x00,']'};
			//
UINT8	BATT12V_CELL_DEF[27]={
		   '[',0x1b,'M',0x0b,'H','2','5','0','/','1',
		   '1','0','/','1','2','0','/','1','3','0',
		   '/','1','4','0','/',0x00,']'};
UINT8	BATCELLA[84]={};						//84bytes
UINT8	BATCELLB[84]={};						//84bytes
			//
//====================================================
UINT8   ACRV[3]	=		{0x33,0x37,0x37};	//ac voltage
UINT8   ACSV[3]	=		{0x33,0x37,0x38};
UINT8   ACTV[3]	=		{0x33,0x37,0x39};
UINT8   ACRI[2]	=		{0x30,0x31};		//ac current
UINT8   ACSI[2]	=		{0x30,0x31};
UINT8   ACTI[2]	=		{0x30,0x31};

//=====================================================//	TCP add 2017.07.13 JSS
UINT16  TCPACRV[3]	=	{0x33,0x37,0x37};	//TCP ac voltage
UINT16  TCPACSV[3]	=	{0x33,0x37,0x38};
UINT16  TCPACTV[3]	=	{0x33,0x37,0x39};
UINT8   TCPACRI[2]	=	{0x30,0x31};		//TCP ac current
UINT8   TCPACSI[2]	=	{0x30,0x31};
UINT8   TCPACTI[2]	=	{0x30,0x31};
UINT8   TCPLDV[3]		=	{0x34,0x38,0x35};	//load voltage
UINT8   TCPOUTV[3]	=	{0x35,0x33,0x35};	//out voltage
UINT16  TCPLDI[3]		=	{0x30,0x30,0x30};	//load current
UINT16  TCPBATI[3]	=	{0x30,0x30,0x30};	//TCP battery (dis)charge current
UINT8   TCPBATTMP[3]  =	{0x00,};		// TCP battery temperature.
UINT8   TCPBATTCELLV1[100]  =	{0x00,};		// TCP battery Cell1 Volt
UINT8   TCPBATTCELLV2[100]  =	{0x00,};		// TCP battery Cell2 Volt
//====================================================
UINT8	CURLMT[3]={0x36,0x30,0x30};	//current limit
UINT8   MODCLMT[3]={0x36,0x30,0x30};//module current limit
UINT8   BATCLMT[3]={0x30,0x31,0x30};	//battery charge limit
UINT8   CURTIME[12]={0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30};
//yy,mm,dd,hh,mm,ss
#if PHY
UINT8 PASS[6]={0x31,0x41,0x30,0x30,0x30,0x30};		//PASSWORD 6DIGITS
#else
UINT8   PASS[9]={0x31,0x41,0x30,0x30,0x30,0x30,0x30,0x30,0x30};		//PASSWORD 9DIGITS
#endif
UINT8   SYSTEMINIT='N';				//or 'Y'  
UINT8   PRODUCTSN[13]={'S','E','S','-','2','0','1','2','0','1','0','0','0'};	
			//
UINT8	FLTV[3]={0x35,0x33,0x35};			//FLOAT VOLTAGE
UINT8	EQV[3]={0x35,0x33,0x35};			//EQ. VOLTAGE
//-------------------------------------------------------------------------------------------
//[0,1,2] - module volt, [3,4,5] - module Current,[6] -State,[7]-errrocode
UINT8   MD1[8],MD2[8],MD3[8],MD4[8],MD5[8],MD6[8],MD7[8],MD8[8];
UINT8   MdState[8]={1,1,1,1,1,1,1,1};	//module state , high 8bit (state) low 8bit(error code)
UINT8   MdSetState[8]={1,1,1,1,1,1,1,1};	//module state , high 8bit (state) low 8bit(error code)
UINT8   MdError[8]={0,0,0,0,0,0,0,0};	//module state , high 8bit (state) low 8bit(error code)
UINT8   fModSetSend=0x00;//(0x0F all), (0x01,02,03,04 id), (0x00 REQ)
//------------------------------------------------------------------------------------------
UINT8	bModule_Rx[100];

UINT8   bTcpModule_Rx[18]={0,};		//TCP add 2017.07.13 JSS
UINT8   Module_Rx_Cnt=0;
UINT8	fModuleRx=0x00;		//Module Receive data done flag, 15bytes, 0x55;
UINT8	Md_Run_Cnt=0;			//Running Module Cnt//동작중인 module 갯수
//------------------------------------------------------------------------------------------------------------
UINT16  Md_Timeout[5]={0,};
UINT16  ModuleTimeout[5]={0,};	// Module Fault add JSS 2017.07.27
//UINT16 ModuleVolt[5]={0,};	// Module Fault add JSS 2017.07.27
UINT16  Lcd_Timeout=0;
UINT16  Snmp_Timeout=0;
//-----------------------------------------------------------------------
// 0b0000-0000(OV-UV-OC-S.D.--AC-DC-MD//ErrorCode
//-----------------------------------------------------------------------
UINT8	ErrorCode=0,ErrorCodeOld=0;
UINT8	ActiveError=0;
UINT8 	fActiveError=0;
UINT16 	ErrorValue=0,ErrModuleNo=0;

//------------------------------------------------------------
UINT8	fStart=0,fCnt=0;		//최초 파워언시 초기 미터링 데이타 센싱후에 셋함.	

UINT8 	day,mth,year,dow,hour,min,sec,sec_old;
UINT8 	day_buf, mth_buf, year_buf, hour_buf,min_buf,sec_buf,err_buf;//history list view때 사용

UINT8	errcntac1=0,errcntac2=0,errcntac3=0,errcntac4=0,errcntac5=0,errcntac6=0,errcntac7=0,errcntac8=0,errcntac9=0;
UINT8	errcntld1=0,errcntld2=0,errcntld3=0;
UINT8	errcntbt1=0,errcntbt2=0,errcntbt3=0;

//==========
//Parameter
//==========
// Operation Mode Command
UINT8   SYS_Type = 0;                    // Module Status from Module
UINT8	fBzOn1=OFF;
UINT8	fBzOn2=OFF;
UINT8	fBzOn1_ing=OFF;
UINT8	fBzOn2_ing=OFF;
UINT8	BzCnt=0;
UINT8	bMuteCnt=0;
UINT8	fAlarmState=0;


//------------------------------------------------------------------------------
UINT16	Batt_Cell_Chk, Batt_Cell_Type; 
UINT16	Remote_Chk, Energy_Chk,Energy_Rate;
UINT8	Energy_Cnt=0,Energy_Toggle=0;

//------------------------------------------------------------------------------
UINT16 	Flt_V,Eq_V, Buzzer_Mode; 
UINT16 	Charge_Mode, Charge_Duration, Charge_Last_Cur,Eq_Last_Cur_Cnt=0;;
UINT16  Eq_Timer=0;

UINT16 	alm_acv_hi, alm_acv_low; 
UINT16	AlmAcData, AlmAcDataOld;
UINT8	AlmLdData, AlmLdDataOld;
UINT8	AlmBatData, AlmBatDataOld;
UINT8	AlmModule;	// Module OFF Alarm add JSS 2017.07.27

UINT8	History_Cnt=0, Mnu_History_Cnt,Total_History_Cnt;

volatile UINT8	f_WdtReset = 1;	// 1 is Watchdog Reset Occur default//
int _EEDATA(2) EE_mem[2048] = {};					//100% EEPROM memory

void Set_EEPROM()
{
	TBLPAG = __builtin_tblpage(EE_mem); 
	NVMADRU = __builtin_tblpage(EE_mem);
}

void EEPROM_Init(void)
{
    UINT16	tmpCnt = 0;
    //=======================================================================
	if(RCONbits.POR)		//check power on reset//
	{
		tmpCnt=read_EE(POR_CNT_EE);
		if(tmpCnt<60000)
		{
			tmpCnt++;
			write_EE(POR_CNT_EE,tmpCnt);            //Write PowerOnReset counter//
		}
		RCONbits.POR = 0;							//Clear PowerOnReset flag
	}
	//-----------------------------------------------------------------------			
	if(RCONbits.WDTO)		//Check WatchDog reset//
	{
		tmpCnt=read_EE(WTR_CNT_EE);
		if(tmpCnt<60000)
		{
	      	tmpCnt++;
			write_EE(WTR_CNT_EE,tmpCnt);    //2013-07-14 ?? 10:54:09
		}	
		RCONbits.WDTO = 0;                  //SFR flag clear//
		f_WdtReset = 1;                     //set local flag
	}
	else
	{ 
		f_WdtReset=0;			
	}
}

void System_Check(void)
{
     UINT16 i = 0;
//========================================================		
	if(read_EE(INIT_EE) != 12345)			//Check First Running.
	{
		write_EE(INIT_EE,12345);		//0x3039
		write_EE(POR_CNT_EE,0);			//2013-07-14 ?? 11:18:06
		write_EE(WTR_CNT_EE,0);
	}
//-----------------------------------------------------------------------------
	if(f_WdtReset==0)				//1 is occur watch dog reset// 	
	{			
		for(i=0;i<350;i++)			//wait system stable -//delay 560ms//
		{
			__asm__ volatile ("CLRWDT");
			Delay_Cnt(2000);
		}
		//--------------------------------
		BZ_OFF();	
	}
	else		//occur watchdog reset //
	{
		f_WdtReset=0;				//clear flag//
	}	
    SYS_Type = read_EE(SYS_TYPE);
}

void Mode_Check(void)
{
    write_EE(SYS_TYPE,SYS_Type);
    SYS_Type = read_EE(SYS_TYPE);
}




