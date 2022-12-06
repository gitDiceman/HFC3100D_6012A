/*
******************************************************************************************
* PROJECT : HFC3000(100A) MAIN CONTROL
* FILE NAME: Memory.h
* DEVICE : dsPIC30F6012A, 
* COMPILER : MPLAB-C V3.02 
* DESCRIPTON : EEPROM ROUTINE, SETTING VALUE
* REVISION HISTORY
* 1) 2011.12.30 (V0.9)
* 
****************************************************************************************** */
//#define		MODULE50A			//50A 적용			
/*=================================================
 :		built-in EEPROM routine
 :	set_EEPROM();						//set EE_mem[] table page
 :	dest = read_EE(addr); 				//dest <= EE_mem[addr]
 :	write_EE(addr,data);  				//EE_mem[addr] <= data
 :	write_EE_array(addr, &data, size)	//EE_mem[addr] <= data[]
=================================================*/
#define TCP 1	//TCP add 2017.07.07 JSS
#define JSS	1	//battery Cell 2set add JSS 2017.06.28
#define PHY	0
#define BCD_M 0

#define read_EE(addr)				read_EE1(__builtin_tbloffset(EE_mem) + (addr)*2)
#define write_EE(addr,data)         write_EE1(__builtin_tbloffset(EE_mem) + (addr)*2, data)

//EEPROM Address
#define     SYS_TYPE         10

#if BCD_M // phy
        unsigned char BCD_L, BCD_H;
        unsigned char BUFF[32], cnt;
#endif

//=====================================================
			

//----------------------------------------------------------------------------
// 경보 & 설정값 범위
//------------------------------------------
#define ACV_HI_DFT		418		//LCD-418 ACV
#define ACV_HI_MAX		430
#define ACV_HI_MIN		380

#define ACV_LOW_DFT		342		//LCD-325 ACV
#define ACV_LOW_MAX		380
#define ACV_LOW_MIN		325

#define ACI_HI_DFT		35		//15A	// 400A add 2017.07.25 JSS
#define ACI_HI_MAX		50				// 400A add 2017.07.25 JSS
#define ACI_HI_MIN		20				// 400A add 2017.07.25 JSS
			//
#define LDV_HI_DFT		550		//LCD-50.0 DCV
#define LDV_HI_MAX		600
#define LDV_HI_MIN		490

#define LDV_LOW_DFT		460		//LCD-46.0 DCV
#define LDV_LOW_MAX		490
#define LDV_LOW_MIN		430
			//
#define BTV_HI_DFT		600		//LCD-60.0 DCV
#define BTV_HI_MAX		600
#define BTV_HI_MIN		530

#define BTV_LOW_DFT		460
#define BTV_LOW_MAX		530
#define BTV_LOW_MIN		460

#define FLT_V_DFT		535
#define EQ_V_DFT		535

#ifdef MODULE50A
	#define MOD_CUR_DFT	550		//55A
	#define CUR_DFT		220		//100A//경보	// 200A add 2017.07.25 JSS
	#define BAT_CUR_DFT	300		//30.0A 
#else
	#define MOD_CUR_DFT	900		//50.0A		//MainLcd확인.
	#define CUR_DFT		440		//100A//경보
	#define BAT_CUR_DFT	600		//55.0A 
#endif	

#define BUZZER_MODE_DFT 	'Y'		//buzzer operation 'Y' or 'N'
#define CHARGE_MODE_DFT		'F'		//CHARGE MODE 'F' FLOAT

#define	CHARGE_DURATION_DFT	1		//1Hr
#define	CHARGE_LAST_CUR_DFT	5		//5A

#define	ENERGY_RATE_DFT		40

//#define PASS_DFT		0		//초기비밀번호 0
//==============================================
// EEPROM ADDRESSS SETTING
//==============================================
#define	INIT_EE			1//0x3039,12345(Decimal):OK, or initialize	

//====================================================
#define	EQ_V_EE					21	//Equalizing voltage  
#define	FLT_V_EE				22	//Float charge voltage  
#define	BUZZER_MODE_EE	 		23	//'Y'/'N'  Y default
#define CHARGE_MODE_EE			24	//'F'/'E'  float default
#define	CHARGE_DURATION_EE		25	//1hr
#define	CHARGE_LAST_CUR_EE		26	//5A

//====================================================
#define BATT_CELL_CHK_EE		31	//'N'/'Y'
#define BATT_CELL_TYPE_EE		32	//2V x 24 Cell or 12V x 4 Cell	
#define	REMOTE_CHK_EE			33	//'N'/'Y'
#define ENERGY_CHK_EE			34	//'N'
#define ENERGY_RATE_EE			35	//40%

//====================================================
#define	PASS_EE_1			41
#define	PASS_EE_2			42
#define	PASS_EE_3			43
#define	PASS_EE_4			44
#define	PASS_EE_5			45
#define	PASS_EE_6			46
#define	PASS_EE_7			47
#define	PASS_EE_8			48
#define	PASS_EE_9			49
//---------------------------------------//현재 경보설정치 저장//	
#define ALM_ACV_HI_EE			52		//alm_acv_hi
#define ALM_ACV_LOW_EE			53		//alm_acv_low
#define ALM_LDV_HI_EE			54		//alm_ldv_hi
#define ALM_LDV_LOW_EE			55		//alm_ldv_low
#define ALM_BTV_HI_EE			56		//alm_btv_hi
#define ALM_BTV_LOW_EE			57		//alm_btv_low
#define ALM_ACI_HI_EE			58		//Aci_Lmt
#define CUR_EE				59		//load current
#define MOD_CUR_EE			60		//module current limit
#define BAT_CUR_EE			61		//battery charge current limit
//------------------------------------------------------------------------------
#define	CHKERRCNT			50

#define  	PRODUCT_EE_1		71
#define  	PRODUCT_EE_2		72
#define  	PRODUCT_EE_3		73
#define  	PRODUCT_EE_4		74
#define  	PRODUCT_EE_5		75
#define  	PRODUCT_EE_6		76
#define  	PRODUCT_EE_7		77
#define  	PRODUCT_EE_8		78
#define  	PRODUCT_EE_9		79
//====================================================
#define		HISTORY_CNT_EE		500	//History Counter 
#define 	HISTORY_CLEAR_EE	502	//History Clear시 시간.
#define 	HISTORY_ADDR_OFFSET_EE	510	//History Save Start Point//

#define		POR_CNT_EE		100
#define		WTR_CNT_EE		104
//-------------------------------------------------------------------------------

//########################################################
extern int read_EE1(int tbl_offset);
extern void write_EE1(int tbl_offset, int data);		//4ms


extern void Set_EEPROM();
extern void EEPROM_Init(void);
extern void System_Check(void);
extern void Mode_Check(void);
extern void Delay_Cnt(UINT16 cnt);
        
        
extern volatile     UINT8 f_WdtReset;	// 1 is Watchdog Reset Occur default//
extern UINT8		fBzOn1;
extern UINT8		fBzOn2;
extern UINT8		fBzOn1_ing;
extern UINT8		fBzOn2_ing;
extern UINT8		BzCnt;
extern UINT8		bMuteCnt;
extern UINT8		fAlarmState;

extern UINT16       Flt_V,Eq_V, Buzzer_Mode; 

// Operation Mode Command
extern UINT8    SYS_Type;
