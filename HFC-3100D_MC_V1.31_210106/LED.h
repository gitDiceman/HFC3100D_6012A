/*
**********************************************************
* PROJECT : HFC3100D MAIN CONTROL
* FILE NAME: LED.h
* DEVICE : dsPIC30F6012A
* COMPILER : MPLAB-C
* REVISION HISTORY
* 1) 2018.03.09 (V1.0)
* 
********************************************************** */
extern void GetNfbState1(void);
extern void GetNfbState2(void);
extern void GetContact(void);

extern UINT8	MD1[8],MD2[8],MD3[8],MD4[8],MD5[8],MD6[8],MD7[8],MD8[8];
extern UINT8	Md_Run_Cnt;

extern UINT16 	tmr2_7;
extern UINT16 	tmr2_8;
extern UINT16 	tmr2_9;
