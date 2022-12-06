
//===============
*2022년 11월28일(구본영)
//===============
HFC3100D 표준품용 F/W 정리배포.
Tools : 	MplabX IDE V6.0
	MplabC30 V3.31
             pickit3, 4, ICD2 

1. 메인컨트롤 - "HFC-3100D_MC_V1.31_210106" //dspic30f6012a
	: 수정내용없음.

2. 메인디스플레이 - "HFC-3100D_MD_V1.31_210119"//dspic30f6012a
	: 수정내용없음.

3. 정류컨트롤 - "HFC-3100D_RC_V1.4_221124" //dspic33fj64gs606
	: 충전전류제어부 PI Gain Calibration.

4. 정류디스플레이 - "HFC-3100D_RD_V1.32_221124"//dspic30f6012a - PF,PT package
             : PT package lcd화면 어긋남. 충전전류 초기에 설정값이 아닌 초기값으로 동작
             : 절체전압 최소값 90V.
             

//================
*2022년 11월 24일(함형원)
//===============
정류컨트롤 수정내용.
1.충전전류 제한 동작 시 전류제한 및 전류제한 해제의 동작 주기가 빨라 순간적으로
  축전지 충방전 상태가 반복되는 현상 개선(리튬인산철 축전지 사용 대비)
2.RC의 Define_PSFB.h에서 축전지 전류제한 PI Gain 조정  
  // Battery Current Dropping Voltage Define 
  #define	KpBattCurrLimit		Q15(0.03)        // Q15(0.1)  HHW      
  #define	KiBattCurrLimit		Q15(0.001)    // Q15(0.005) HHW
