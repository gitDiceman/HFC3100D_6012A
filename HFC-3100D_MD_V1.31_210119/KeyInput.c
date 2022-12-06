#include 	<p30f6012A.h>
#include 	"Definition.h"
#include	"Function.h"
//====================================================================================//
//                          Function Declaration                                      //
//====================================================================================//
extern void DelayNop(UINT16 N);



//====================================================================================//
//                          Variable Declaration                                      //
//====================================================================================//
static UINT16	key_cnt[9] = {0,0,0,0,0,0,0,0,0};
static UINT8	KeyBuffer;
static UINT8 	KeyDataTemp;
static UINT16	KeyOnfTime; 
static UINT16 	keySequence;

void KeyDetecting(void)
{
	UINT8 i;
	unsigned char KeyData;
	if (KeyOnfTime != 0) KeyOnfTime--;
    #ifdef KEY_BUZZER
    if (BuzzTimer != 0){
        BuzzTimer--;
	    Buzzer_On();
	}
    else Buzzer_Off();
	#endif
    switch(getKeyData())
	{
		case KEY_ENTER:			KeyData = keyKEY_ENTER;			key_cnt[F_KEY_ENTER]++;			break;
		case KEY_LEFT:	        KeyData = keyKEY_LEFT;	 		key_cnt[F_KEY_LEFT]++;   		break;
		case KEY_UP:			KeyData = keyKEY_UP;			key_cnt[F_KEY_UP]++;     		break;
		case KEY_RIGHT:         KeyData = keyKEY_RIGHT;			key_cnt[F_KEY_RIGHT]++;			break;
		case KEY_DOWN:			KeyData = keyKEY_DOWN;			key_cnt[F_KEY_DOWN]++;			break;
		case KEY_AUTOMANUAL:  	KeyData = keyKEY_AUTOMANUAL;	key_cnt[F_KEY_AUTOMANUAL]++;  	break;
		case KEY_NO_SELECT:  	KeyData = keyKEY_NO_SELECT;		key_cnt[F_KEY_NO_SELECT]++;  	break;
		case KEY_LAMP:			KeyData = keyKEY_LAMP;			key_cnt[F_KEY_LAMP]++;			break;
		case KEY_BZ_OFF:		KeyData = keyKEY_BZ_OFF;		key_cnt[F_KEY_BZ_OFF]++;		break;
        default:
			for (i=0; i<9; i++)
			{
				key_cnt[i] = 0;   
			}
			break;
	}
    switch (keySequence)
	{
        case SEQ_NOP:
            if (KeyData != keyNULL){
                KeyDataTemp = KeyData;
                keySequence = SEQ_SOME;
            }
            break;

        case SEQ_SOME:
            if (KeyData == keyNULL)
			{
                keySequence = SEQ_NOP;
                break;
            }
            if (KeyDataTemp == KeyData)
			{
                KeyOnfTime  = KEY_ON_TIME;
                keySequence = SEQ_ON_TIME;
            }
            else
			{
                keySequence = SEQ_NOP;
            }
            break;

        case SEQ_ON_TIME:
            if (KeyDataTemp != KeyData){
                keySequence = SEQ_NOP;
            }
            else if (KeyOnfTime==0){
                keySequence = SEQ_CATCH;
            }
            break;

        case SEQ_CATCH: // 3
            KeyBuffer = KeyDataTemp;
			
#ifdef KEY_BUZZER
            BuzShort();
#endif            
			BL = 0;
			tmr2Count4 = 0;
			
            KeyOnfTime  = KEY_REJECT_TIME;
            keySequence = SEQ_REJECT;
            break;

        case SEQ_REJECT:	// 4
            if (KeyOnfTime == 0){
#ifdef KEY_BUZZER
                if (KeyBuffer != keyNULL) BuzReject();
#endif
                keySequence = SEQ_OFF;
            }
            break;

        case SEQ_OFF:	// 5
            if (KeyData == keyNULL){
                KeyOnfTime  = KEY_OFF_TIME;
                keySequence = SEQ_OFF_TIME;
            }
            break;

        case SEQ_OFF_TIME:	// 6
            if (KeyData != keyNULL){
                keySequence = SEQ_OFF;
            }
            else if (KeyOnfTime == 0){
                keySequence = SEQ_END;
            }
            break;

        case SEQ_END:	// 7
            keySequence = SEQ_NOP;
            break;
    }
}

UINT8 getKeyData(void)
{
	UINT8 uc_key = 0;	
	SW_SCAN3 = 1;
	SW_SCAN1 = 0;
	SW_SCAN2 = 0;
	DelayNop(10);
	if ( SW_KEY_NUM1 == 1 )			uc_key = KEY_ENTER;
	else if ( SW_KEY_NUM2 == 1 )	uc_key = KEY_BZ_OFF;
	else if ( SW_KEY_NUM3 == 1 )	uc_key = KEY_LAMP;

	SW_SCAN3 = 0;
	SW_SCAN1 = 1;
	SW_SCAN2 = 0;
	DelayNop(10);
	if ( SW_KEY_NUM1 == 1 )			uc_key = KEY_UP;
	else if ( SW_KEY_NUM2 == 1 )	uc_key = KEY_RIGHT;	
	else if ( SW_KEY_NUM3 == 1 )	uc_key = KEY_AUTOMANUAL;

	SW_SCAN3 = 0;
	SW_SCAN1 = 0;
	SW_SCAN2 = 1;
	DelayNop(10);
	if ( SW_KEY_NUM1 == 1 )			uc_key = KEY_DOWN;
	else if ( SW_KEY_NUM2 == 1 )	uc_key = KEY_LEFT;			
	else if ( SW_KEY_NUM3 == 1 )	uc_key = KEY_NO_SELECT;

	return uc_key;
}


UINT8 hKey(void)
{
	UINT8 KeyValue;

	if (key_cnt[F_KEY_ENTER] >= DELAY_LONG_PRESSED)
	{
		key_cnt[F_KEY_ENTER] = DELAY_LONG_PRESSED;
		KeyValue        = keyKEY_ENTERfast;
	}
	else if (key_cnt[F_KEY_LEFT] >= DELAY_LONG_PRESSED)
	{
		key_cnt[F_KEY_LEFT] = DELAY_LONG_PRESSED;
		KeyValue         = keyKEY_LEFTfast;
	}
	else if (key_cnt[F_KEY_UP] >= DELAY_LONG_PRESSED)
	{
		key_cnt[F_KEY_UP] = DELAY_LONG_PRESSED;
		KeyValue         = keyKEY_UPfast;
	}
	else if (key_cnt[F_KEY_RIGHT] >= DELAY_LONG_PRESSED)
	{
		key_cnt[F_KEY_RIGHT] = DELAY_LONG_PRESSED;
		KeyValue         = keyKEY_RIGHTfast;
	}
	else if (key_cnt[F_KEY_DOWN] >= DELAY_LONG_PRESSED)
	{
		key_cnt[F_KEY_DOWN] = DELAY_LONG_PRESSED;
		KeyValue         = keyKEY_DOWNfast;
	}
	else if (key_cnt[F_KEY_AUTOMANUAL] >= DELAY_LONG_PRESSED)
	{
		key_cnt[F_KEY_AUTOMANUAL] = DELAY_LONG_PRESSED;
		KeyValue         = keyKEY_AUTOMANUALfast;
	}
	else if (key_cnt[F_KEY_NO_SELECT] >= DELAY_LONG_PRESSED)
	{
		key_cnt[F_KEY_NO_SELECT] = DELAY_LONG_PRESSED;
		KeyValue         = keyKEY_NO_SELECTfast;
	}
	else if (key_cnt[F_KEY_LAMP] >= DELAY_LONG_PRESSED)
	{
		key_cnt[F_KEY_LAMP] = DELAY_LONG_PRESSED;
		KeyValue         = keyKEY_LAMPfast;
	}
	else if (key_cnt[F_KEY_BZ_OFF] >= DELAY_LONG_PRESSED)
	{
		key_cnt[F_KEY_BZ_OFF]   = DELAY_LONG_PRESSED;
		KeyValue        = keyKEY_BZ_OFFfast;
	}		
	else
	{
		if (KeyBuffer != keyNULL  )
		{
			KeyValue = KeyBuffer;
		}
		else
		{
			KeyValue = keyNULL;
		}
	}
	KeyBuffer = keyNULL;

	return (unsigned char)KeyValue;
}

