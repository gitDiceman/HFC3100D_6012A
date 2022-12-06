/* 
 * File:   Alram_Sts.h
 * Author: KHJ
 *
 * Created on 2018? 2? 13? (?), ?? 2:00
 */
#include 	<p30f6012A.h>
#include    "ADC.h"
#include 	"eeprom.h"
#include    "Definition.h"
#include    "Initialize.h"
#include    "Alram_Sts.h"
#include    "RS232.h"

void Alram_Sts(void);
void CHG_CMD(void);
void CHG_Check(void);
void CHG_Time(void);
void hex16_3digt(unsigned char hex_data_8);
void hex16_4digt(unsigned int hex_data);

float Input_UV_Ref = 0;	
float Input_OV_Ref = 0;  	
float Input_V_Ref = 0;
float Input_Shutdown_Ref = 0; 

UINT8   CHG_Mode = 0x00; //
UINT8   CHG_SEL = 0x00;
UINT8   CHG_EN = 0x00;
UINT8   Input_OV = 0x00;
UINT8   Input_UV = 0x00;
UINT8   Input_Shutdown = 0x00;

UINT8   Res_Year[10] = {0,}; 
UINT8   Res_Month[10] = {0,};
UINT8   Res_Day[10] = {0,};
UINT8   Res_Hour[10] = {0,}; 
UINT8   Res_Min[10] = {0,};
UINT8   Res_Sec[10] = {0,};

void Alram_Sts(void)
{
    if(ACV_R >= Set_Input_OV || ACV_S >= Set_Input_OV || ACV_T >= Set_Input_OV)
    {
        Input_OV = 0x80;
    }
    else if((ACV_R >= Input_Shutdown_Ref || ACV_S >= Input_Shutdown_Ref || ACV_T >= Input_Shutdown_Ref) && (ACV_R <= Set_Input_UV || ACV_S <= Set_Input_UV || ACV_T <= Set_Input_UV))
    {
        Input_UV = 0x40;
    }
    else if(ACV_R <= Input_Shutdown_Ref || ACV_S <= Input_Shutdown_Ref || ACV_T <= Input_Shutdown_Ref)
    {
        Input_Shutdown = 0x20;
    }
    else
    {
        Input_OV = 0x00;
        Input_UV = 0x00;
        Input_Shutdown = 0x00;
    }    
    
}
    
void CHG_CMD(void)
{
    if(CHG_SEL == 0 && CHG_Mode == 0)
    {
        Set_VRef = ((battVoltSet[4] & 0x0f)*100)+((battVoltSet[5] & 0x0f)*10)+((battVoltSet[6] & 0x0f)*1)+((battVoltSet[7] & 0x0f)*0.1);
        CHG_Sts.b7 = 0;
        CHG_Sts.b6 = 0;
        MANUAL = OFF;
        Floating = OFF;
        EQUALIZING = ON;
    }
    else if(CHG_SEL == 0 && CHG_Mode == 1)
    {
        Set_VRef = ((battVoltSet[0] & 0x0f)*100)+((battVoltSet[1] & 0x0f)*10)+((battVoltSet[2] & 0x0f)*1)+((battVoltSet[3] & 0x0f)*0.1);
        CHG_Sts.b7 = 0;
        CHG_Sts.b6 = 1;
        MANUAL = OFF;
        Floating = ON;
        EQUALIZING = OFF;
    }
    else if(CHG_SEL == 1 && CHG_Mode == 0)
    {
        if(CHG_EN == 0)
        {
            Set_VRef = ((battVoltSet[4] & 0x0f)*100)+((battVoltSet[5] & 0x0f)*10)+((battVoltSet[6] & 0x0f)*1)+((battVoltSet[7] & 0x0f)*0.1);
        }
        else
        {
            Set_VRef = ((outSetNew[0]-0x30)*100)+((outSetNew[1]-0x30)*10)+((outSetNew[2]-0x30)*1)+((outSetNew[3]-0x30)*0.1);
        }
        CHG_Sts.b7 = 1;
        CHG_Sts.b6 = 0;
        MANUAL = ON;
        Floating = OFF;
        EQUALIZING = ON;
    }
    else if(CHG_SEL == 1 && CHG_Mode == 1)
    {
        if(CHG_EN == 0)
        {
            Set_VRef = ((battVoltSet[0] & 0x0f)*100)+((battVoltSet[1] & 0x0f)*10)+((battVoltSet[2] & 0x0f)*1)+((battVoltSet[3] & 0x0f)*0.1);
        }
        else
        {
            Set_VRef = ((outSetNew[0]-0x30)*100)+((outSetNew[1]-0x30)*10)+((outSetNew[2]-0x30)*1)+((outSetNew[3]-0x30)*0.1);
        }
        CHG_Sts.b7 = 1;
        CHG_Sts.b6 = 1;
        MANUAL = ON;
        Floating = ON;
        EQUALIZING = OFF;
    }
}

void CHG_Check(void)
{
    float temp = 0.0;
    CHG_Time();
    // AUTO Equal Start
    if(CHG_SEL == 0 && Auto_Flag == 0) //Auto_Flag : in to the Charge state 
    {
        Auto_Flag = 1;
        CHG_Mode = 0;
        CHG_CMD();
        Set_OK = 1;  
        Set_cnt = 0;
    }
    //AUTO Equal
    else if(CHG_SEL == 0 && Auto_Flag == 1)
    {
        // Auto Equal Finish
        temp = ((battVoltSet[4] & 0x0f)*100)+((battVoltSet[5] & 0x0f)*10)+((battVoltSet[6] & 0x0f)*1)+((battVoltSet[7] & 0x0f)*0.1); 
        if(Set_cnt == 10 && (((BATT_A <= CHG_Cut_CURR)&&(Voltage_Out > temp*0.98)) || (Cut_TIMER >= (unsigned int)CHG_Cut_TIME)))
        {
            Auto_Flag = 2;
            CHG_Mode = 1;
            CHG_CMD();
            Set_OK = 1;
        }
        // Time delay Double check
        else
        {
            Set_cnt++;
            if(Set_cnt >= 10) Set_cnt = 10;
        }       
    }
    // Normal Condition, Floating charge
    else
    {
        Set_cnt = 0;
    }
}

void CHG_Time(void)
{ 
    int i = 0;
    for(i=0;i<10;i++)
    {
        if(CHG_SEL == 0) // view Standard paper
        {
            Res_Year[i] = read_EE(SET_YEAR_0 + (i*6));        
            if(Res_Year[i] == rm_bcd(read_ds1302(0x8d)) || Res_Year[i] == 0)
            {
                Res_Month[i] = read_EE(SET_MONTH_0 + (i*6));
                if(Res_Month[i] == rm_bcd(read_ds1302(0x89)) || Res_Month[i] == 0)
                {
                    Res_Day[i] = read_EE(SET_DAY_0 + (i*6));
                    if(Res_Day[i] == rm_bcd(read_ds1302(0x87)) || Res_Day[i] == 0)
                    {
                        Res_Hour[i] = read_EE(SET_HOUR_0 + (i*6));
                        if(Res_Hour[i] == rm_bcd(read_ds1302(0x85)))
                        {
                            Res_Min[i] = read_EE(SET_MIN_0 + (i*6));
                            if(Res_Min[i] == rm_bcd(read_ds1302(0x83)))
                            {
                                Res_Sec[i] = read_EE(SET_SEC_0 + (i*6));
                                if(Res_Sec[i] == rm_bcd(read_ds1302(0x81)))
                                {
                                    Auto_Flag = 0;
                                    tmr1Count4 = 0;
                                    tmr1min = 0;
                                    Cut_TIMER = 0;
                                }
                            }
                        } 
                    }
                }
            }
        }
    }
    
}

void hex16_3digt(unsigned char hex_data_8)
{
	unsigned char buffer;
	buffer = hex_data_8;
	WDT = HI;_NOP();_NOP();_NOP();WDT = LO;
	hund = ((buffer/100) | 0x30);buffer %= 100;
	ten = ((buffer/10) | 0x30);buffer %= 10;
	one = (buffer | 0x30);
	WDT = HI;
}//hex16_3digt end

void hex16_4digt(unsigned int hex_data)
{
	unsigned int buffer;
	buffer = hex_data;
	WDT = HI;_NOP();_NOP();_NOP();WDT = LO;
	thnd = ((buffer/1000) | 0x30);buffer %= 1000;
	hund = ((buffer/100) | 0x30);buffer %= 100;
	ten = ((buffer/10) | 0x30);buffer %= 10;
	one = (buffer | 0x30);
	WDT = HI;
}//hex16_4digt end


