/*
******************************************************************************************
* PROJECT : HFC3000(100A) MAIN CONTROL
* FILE NAME: TYPEDEF.H
* DEVICE : dsPIC30F6012A
* COMPILER : MPLAB-C V3.02 
* REVISION HISTORY
* 1) 2011.12.30 (V0.9)
* 
****************************************************************************************** */
/********************************************************************************
 * Definitions
 ********************************************************************************/
#define sdiv(a,b)			__builtin_divsd(a,b)
#define abs(a)				((a)>0?(a):-(a))
#define max(a,b)			((a)>(b)?(a):(b))
#define min(a,b)			((a)<(b)?(a):(b))
#define limit(a,min,max)	((a)=((a)=(a)<(min)?(min):(a))>(max)?(max):(a))
 //-------------------------------------------------------
#define HEXA_MODE       		(0x50)
#define CODEB_MODE      	(0x90)
#define NO_CODE_MODE    	(0xB0)
#define DOT_MASK        		(0x80)
#define DOT					(0x7f)
#define BLANK						(0x8f)
// NO DECODEDING MODE
#define __a     (0x7D)
#define __A     (0x7E)
#define __b     (0x1F)
#define __B     (0x7F)
#define __C     (0x4B)
#define __c     (0x0D)
#define __d     (0x3D)
#define __E     (0x4F)
#define __e     (0x6F)
#define __F     (0x4E)
#define __G     (0x5F) 
#define __h     (0x1E)
#define __H     (0x3E)
#define __i     (0x10)  
#define __I     (0x0A)
#define __J     (0x39)
#define __L     (0x0B)
#define __n     (0x1C)
#define __O     (0x7B)
#define __o     (0x1D)
#define __P     (0x6E)
#define __r     (0x0C)
#define __S     (0x57)
#define __t     (0x0F)
#define __u     (0x19)
#define __U     (0x3B)
#define __0     (0x7B)
#define __1     (0x30)
#define __2     (0x6D)
#define __3     (0x75)
#define __4     (0x36)
#define __5     (0x57)
#define __6     (0x5F)
#define __7     (0x70)
#define __8     (0x7F)
#define __9     (0x76)

#define __sa    (0x40) //
#define __sb    (0x20)
#define __sc    (0x10)
#define __sd    (0x01)
#define __se    (0x08)
#define __sf    (0x02)
#define __sg    (0x04)

#define __sz    (0x00)
#define __ss    (0x7F)

#define SNMPSN			(0x33)
#define SNMPMETER 		(0x44)
#define SNMPALARM		(0x55)
//=====================================//TCP add 2017.07.07 JSS
#define TCPREQ			(0x52)	// 'R'
#define TCPHISTORY 		(0x48)	// 'H'
#define TCPMANAGE		(0x56)	// 'V'


#define HIGH    					(1)
#define LOW     					(!HIGH)
		//
#define LEFT						(0)
#define RIGHT 					(1)
		//
#define ON      					(1)
#define OFF     					(!ON)
#define	INPUT					(1)
#define	OUTPUT				(0)
		//
#define WRITE_CONFIG	0xC000
#define READ_CONFIG		0x4000
#define WRITE_DATA			0x8000
#define READ_DATA			0x0000
#define TE_HIGH				0x0400
#define RTS_HIGH				0x0200
#define TE_LOW				0x0000
#define RTS_LOW				0x0000
		//
#define SEG_MODE		(LATDbits.LATD5)  //'H' control, 'L' data
#define SEL_RFND			(LATDbits.LATD6)	//LOW - RIGHT fnd select(LOAD)
#define SEL_LFND			(LATDbits.LATD7)	//LOW - LEFT fnd select(BATTERY)
#define SEL_SPI				(LATDbits.LATD11)//active low
		//
#define SPI_CLK				(LATGbits.LATG6)
#define SPI_DOUT			(LATGbits.LATG8)
#define SPI_DIN				(LATGbits.LATG7)
/*			50A용
#define	Mod1_Mount	CheckBit(bNfbState,6)				//0 mount, 1 demount
#define	Mod2_Mount	CheckBit(bModState,0)	
#define	Mod3_Mount	CheckBit(bModState,1)
#define	Mod4_Mount	CheckBit(bModState,2)
*/
/*
#define	Mod1_Mount	CheckBit(bNfbState,6)				//0 mount, 1 demount
#define	Mod2_Mount	CheckBit(bModState,1)	
#define	Mod3_Mount	CheckBit(bModState,3)
#define	Mod4_Mount	CheckBit(bModState,5)
*/

#define BZ					(LATGbits.LATG9)  //hi - buzzer ON

#define	M1_FAIL				(LATDbits.LATD8) 		
#define	M2_FAIL				(LATDbits.LATD9) 		
#define	BATT_LOW			(LATDbits.LATD10) 	

#define 	M1_FAIL_ON()		(LATDbits.LATD8 = 1)
#define 	M2_FAIL_ON()		(LATDbits.LATD9 = 1)
#define 	BATT_LOW_ON()		(LATDbits.LATD10 = 1)

#define 	M1_FAIL_OFF()		(LATDbits.LATD8 = 0)
#define 	M2_FAIL_OFF()		(LATDbits.LATD9 = 0)
#define 	BATT_LOW_OFF()		(LATDbits.LATD10 = 0)

#define 	M1_FAIL_TOGGLE()		(LATDbits.LATD8 ^= 1)
#define 	M2_FAIL_TOGGLE()		(LATDbits.LATD9 ^= 1)
#define 	BATT_LOW_TOGGLE()		(LATDbits.LATD10 ^= 1)

#define 	BZ_TOGGLE()	(LATGbits.LATG9 ^=1)
#define 	BZ_OFF()			(LATGbits.LATG9 = 0) 
#define 	BZ_ON()			(LATGbits.LATG9 = 1) 
//--------------------------------------------------------------
//#define 	CAN_RX			(PORTFbits.RF0)
//#define		Can_Tx_H			(LATFbits.LATF1=1)	//for debug.
//#define		Can_Tx_L			(LATFbits.LATF1=0)	//for debug.
#define		debughi()			(LATFbits.LATF1	=1)	//for debug.
#define		debuglow()		(LATFbits.LATF1	=0)	//for debug.
#define 	debugtoggle()		(LATFbits.LATF1 ^= 1)

#define		testhi()			(LATFbits.LATF0	=1)	//for debug.
#define		testlow()			(LATFbits.LATF0	=0)	//for debug.
#define 	testtoggle()		(LATFbits.LATF0 ^= 1)
//-------------------------------------------------------------

#define		NFB2_READ		_LATB13			//Active low
#define 	NFB1_READ		_LATB14			//Active low
		
#define FND0	(LATGbits.LATG1)
#define FND1	(LATGbits.LATG0) 	
#define FND2	(LATGbits.LATG14)	
#define FND3	(LATGbits.LATG12)	
#define FND4	(LATGbits.LATG13)	
#define FND5	(LATGbits.LATG15)	
#define FND6	(LATCbits.LATC1)
#define FND7	(LATCbits.LATC2)

#define LCD0	(LATGbits.LATG1)
#define LCD1	(LATGbits.LATG0) 	
#define LCD2	(LATGbits.LATG14)	
#define LCD3	(LATGbits.LATG12)	
#define LCD4	(LATGbits.LATG13)	
#define LCD5	(LATGbits.LATG15)	
#define LCD6	(LATCbits.LATC1)
#define LCD7	(LATCbits.LATC2)

#define line1	0
#define line2	1
#define line3	2
#define line4	3
#define sline1 (0x80)
#define sline2 (0xc0)
#define sline3 (0x90)
#define sline4 (0xd0)
//----------------------------------------//Main -> Module
#define Request		0x00				//Request Module's Voltage & Current
#define SetAll		0x01				//Set	 All Module's Voltage
#define SetID 		0x02				//Set ID's Module Set.
#define SetInit		0x00				//개별모듈설정.

#define CheckBit(x,y) 	((x)>>(y)) & (0x1)
#define SetBit(x,y) 		(x)|=((0x1)<<(y))
#define ClrBit(x,y) 		(x)&=~((0x1)<<(y))
				
#define     _NOP()		__asm__ volatile ("NOP")
/***************************************************************************
 *  generic type definitiON
 ***************************************************************************/

typedef int             		INT ;							//16BIT
typedef unsigned int            UINT ;


/* Compiler-independent, fixed size types */
typedef signed char         	INT8 ;      /* Signed 8-bit integer    */
typedef unsigned char       	UINT8 ;     /* Unsigned 8-bit integer  */
typedef signed short int    	INT16 ;     /* Signed 16-bit integer   */
typedef unsigned short int  	UINT16 ;    /* Unsigned 16-bit integer */
typedef long int            	INT32 ;     /* Signed 32-bit integer   */
typedef unsigned long       	UINT32 ;
typedef float               	FLOAT32 ;   /* 32-bit IEEE single precisiON */
typedef double              	FLOAT64 ;   /* 64-bit IEEE double precisiON */

typedef unsigned char  			BYTE ;
typedef unsigned int   			WORD;
typedef enum {false,true} 		BOOL;

typedef union
{
    UINT8 _byte;
    struct
    {
        unsigned lsb :4,
                 msb :4;
    };
}BYTE_T;

/***************************************************************************
 *  specific definitiONs
 ***************************************************************************/
typedef struct 
{
	unsigned b0 :1,b1 :1,b2 :1,b3 :1,b4 :1,b5 :1,b6 :1,b7 :1;	
} bool8;

typedef struct 
{
	unsigned b0 :1,b1 :1,b2 :1,b3 :1,b4 :1,b5 :1,b6 :1,b7 :1;
	unsigned b8 :1,b9 :1,b10:1,b11:1,b12:1,b13:1,b14:1,b15:1;
} bool16;




#define SData_HIGH()        (PORTE_Bit7 =1)
#define SData_LOW()         (PORTE_Bit7 = 0)
#define SData_Toggle()      (PORTE_Bit7 ^= 1)

#define SLoad_HIGH()           (PORTE_Bit6 = 1)
#define SLoad_LOW()          (PORTE_Bit6 = 0)
#define SLoad_Toggle()       (PORTE_Bit6 ^= 1)

#define SClock_HIGH()       (PORTE_Bit5 = 1)
#define SClock_LOW()        (PORTE_Bit5 = 0)
#define SClock_Toggle()       (PORTE_Bit5 ^= 1)


//------------------------------------------------------------------------------------------------------
//1500-1ms, 1000-670us, 500-340us, 200-136us, 100-69us, 10-9.2us, 1-2.9us//
//------------------------------------------------------------------------------------------------------
//=======================================================


