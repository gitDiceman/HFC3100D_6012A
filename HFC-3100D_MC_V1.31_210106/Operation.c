/*
**********************************************************
* PROJECT : HFC3100D MAIN CONTROL
* FILE NAME: Operation.c
* DEVICE : dsPIC30F6012A
* COMPILER : MPLAB-C
* REVISION HISTORY
* 1) 2018.03.07 (V1.0)
* 
********************************************************** */
#include    <p30F6012A.h>
#include    "typedef.h"
#include    "SPI_RS485.h"
#include    "RS232.h"
#include    "Operation.h"

UINT8   RST_cnt = 0;            // Restart Count During Recovery condition
UINT8   RST_Timer = 0;          // Recovery Flag Timer During Recovery condition
UINT8   Recovery_Flag = 0;      // Recovery Flag During Recovery condition
UINT8   Set_OK = 0;             // OP_CMD => '1' : Module1 / '2' : Module2
UINT16  tmr2_2 = 0;             // Logic Timer

//////////////////////////////////////////////////////////////////////////////// 
//   Operation Logic Process(but, Manual Mode code is Added to SPI_RS485.C    //
//                         Health Monitoring Logic                            //
//             Please Refer to the Technical Documentation for Detail         //
////////////////////////////////////////////////////////////////////////////////
void OP_Logic(void)     
{   
    //-------------------------- Normal Condition ----------------------------//
    if((OP_CMD == 1 && Alram_Sts[1].b7 == 1 && Alram_Sts[2].b7 == 0 && Alram_Sts[1].b4 == 0 && Alram_Sts[2].b4 == 0 && LED_Module1_NFB == 1 && LED_Module2_NFB == 1 && LED_DC1_NFB == 1 && LED_DC2_NFB == 1) || (OP_CMD == 2 && Alram_Sts[1].b7 == 0 && Alram_Sts[2].b7 == 1 && Alram_Sts[1].b4 == 0 && Alram_Sts[2].b4 == 0 && LED_Module1_NFB == 1 && LED_Module2_NFB == 1 && LED_DC1_NFB == 1 && LED_DC2_NFB == 1))
    {   
        RST_cnt = 0;                // Restart Count Reset
        tmr2_2 = 0;                 // Recovery Tic Timer 2_2 Reset
        RST_Timer = 0;              // Recovery Flag Timer Reset
        Recovery_Flag = 0;          // Recovery_Flag Reset(Recovery Mode Clear)
        Set_OK = 0;                 // Forcing Change Normal Condition RS485 Bus 
    }
    //-------------------------- Fault Condition -----------------------------//
    else if(((OP_CMD == 1 && LED_DC1_NFB != 1) || (OP_CMD == 1 && LED_Module1_NFB != 1) || (OP_CMD == 1 && Alram_Sts[1].b7 != 1) || (OP_CMD == 1 && Alram_Sts[1].b4 != 0)) || ((OP_CMD == 2 && LED_DC2_NFB != 1) || (OP_CMD == 2 && LED_Module2_NFB != 1) || (OP_CMD == 2 && Alram_Sts[2].b7 != 1) || (OP_CMD == 2 && Alram_Sts[2].b4 != 0)))
    {   
        // Module 1 Fault
        if((OP_CMD == 1 && Alram_Sts[1].b4 != 0) || (OP_CMD == 1 && Alram_Sts[1].b7 != 1) || (OP_CMD == 1 && LED_Module1_NFB != 1) || (OP_CMD == 1 && LED_DC1_NFB != 1))
        {   
            // Bypass Mode Entering
            if(RST_cnt == 0 || RST_cnt > 3)
            {
                Set_OK = 1;                 // Forcing Set, Change Bus Stop Condition of RS-485 before to Send Operation Command
                OP_CMD = 2;                 // Forcing Command Change Operation Command Module2
                Request_Send();             // Forcing Operation Command Send to Modules
                Alram_Sts[1].b7 = 0;        // Forcing OFF Status Change to Module1(Not real, Because, Transmission Delay)
                Alram_Sts[2].b7 = 1;        // Forcing ON Status Change to Module2(Not real, Because, Transmission Delay)
                tmr2_2 = 0;                 // Recovery Tic Timer 2_2 Reset
                RST_Timer = 0;              // Recovery Flag Timer Reset
                
                if(RST_cnt == 0)            // First Fault Condition
                {
                    Recovery_Flag = 1;          // Recovery_Flag Set(Recovery Mode)
                    OP_CMD = 1;                 // Forcing Operation Command Module1 for Recovery Mode Memory 
                    RST_cnt++;                  // Restart Count Increase
                }
                
                else
                {
                    Recovery_Flag = 0;          // Recovery Flag Clear
                    RST_cnt = 0;                // Restart Count Clear
                }
            }
            
            // Recovery Mode Entering
            else if((RST_cnt > 0 && RST_cnt < 4) && RST_Time <= RST_Timer)
            {
                Set_OK = 1;                 // Forcing Set, Change Bus Stop Condition of RS-485 before to Send Operation Command
                Request_Send();             // Forcing Operation Command Send to Modules
                Alram_Sts[1].b7 = 1;        // Forcing ON Status Change to Module1(Not real, Because, Transmission Delay)
                Alram_Sts[2].b7 = 0;        // Forcing OFF Status Change to Module2(Not real, Because, Transmission Delay)
                tmr2_2 = 0;                 // Recovery Tic Timer 2_2 Reset
                RST_Timer = 0;              // Recovery Flag Timer Reset
                Recovery_Flag = 1;          // Recovery Flag Set(Recovery Mode)
                  
                // Bypass Mode, After Recovery Mode Failuare
                if((OP_CMD == 1 && Alram_Sts[1].b4 != 0) || (OP_CMD == 1 && Alram_Sts[1].b7 != 1) || (OP_CMD == 1 && LED_Module1_NFB != 1) || (OP_CMD == 1 && LED_DC1_NFB != 1))
                {
                    Set_OK = 1;                 // Forcing Set, Change Bus Stop Condition of RS-485 before to Send Operation Command
                    OP_CMD = 2;                 // Forcing Command Change Operation Command Module2
                    Request_Send();             // Forcing Operation Command Send to Modules
                    Alram_Sts[1].b7 = 0;        // Forcing OFF Status Change to Module1(Not real, Because, Transmission Delay)
                    Alram_Sts[2].b7 = 1;        // Forcing ON Status Change to Module2(Not real, Because, Transmission Delay)
                    Recovery_Flag = 1;          // Recovery Flag Set(Recovery Mode)
                    OP_CMD = 1;                 // Forcing Operation Command Module1 for Recovery Mode Memory 
                }
                RST_cnt++;                      // Restart Count Increase
            }            
            ////////////////////////////////////////////////////////////////////
        }

        // Module 2 Fault
        else if((OP_CMD == 2 && Alram_Sts[2].b4 != 0) || (OP_CMD == 2 && Alram_Sts[2].b7 != 1) || (OP_CMD == 2 && LED_Module2_NFB != 1) || (OP_CMD == 2 && LED_DC2_NFB != 1))
        {
            // Bypass Mode Entering
            if(RST_cnt == 0 || RST_cnt > 3)
            {
                Set_OK = 1;                     // Forcing Set, Change Bus Stop Condition of RS-485 before to Send Operation Command
                OP_CMD = 1;                     // Forcing Command Change Operation Command Module1
                Request_Send();                 // Forcing Operation Command Send to Modules
                Alram_Sts[1].b7 = 1;            // Forcing ON Status Change to Module1(Not real, Because, Transmission Delay)
                Alram_Sts[2].b7 = 0;            // Forcing OFF Status Change to Module2(Not real, Because, Transmission Delay)
                tmr2_2 = 0;                     // Recovery Tic Timer 2_2 Reset
                RST_Timer = 0;                  // Recovery Flag Timer Reset
                
                if(RST_cnt == 0)                // First Fault Condition
                {
                    Recovery_Flag = 1;                  // Recovery_Flag Set(Recovery Mode)
                    OP_CMD = 2;                         // Forcing Operation Command Module2 for Recovery Mode Memory
                    RST_cnt++;                          // Restart Count Increase
                }
                else
                {
                    Recovery_Flag = 0;                  // Recovery Flag Clear
                    RST_cnt = 0;                        // Restart Count Clear
                }
            }
            
            // Recovery Mode Entering
            else if((RST_cnt > 0 && RST_cnt < 4) && RST_Time <= RST_Timer)
            {
                Set_OK = 1;                 // Forcing Set, Change Bus Stop Condition of RS-485 before to Send Operation Command
                Request_Send();             // Forcing Operation Command Send to Modules
                Alram_Sts[1].b7 = 0;        // Forcing OFF Status Change to Module1(Not real, Because, Transmission Delay)
                Alram_Sts[2].b7 = 1;        // Forcing ON Status Change to Module2(Not real, Because, Transmission Delay)
                tmr2_2 = 0;                 // Recovery Tic Timer 2_2 Reset
                RST_Timer = 0;              // Recovery Flag Timer Reset
                Recovery_Flag = 1;          // Recovery Flag Set(Recovery Mode)
                // Bypass Mode, After Recovery Mode Failuare
                if((OP_CMD == 2 && Alram_Sts[2].b4 != 0) || (OP_CMD == 2 && Alram_Sts[2].b7 != 1) || (OP_CMD == 2 && LED_Module2_NFB != 1) || (OP_CMD == 2 && LED_DC2_NFB != 1))
                {
                    Set_OK = 1;                     // Forcing Set, Change Bus Stop Condition of RS-485 before to Send Operation Command
                    OP_CMD = 1;                     // Forcing Command Change Operation Command Module2
                    Request_Send();                 // Forcing Operation Command Send to Modules
                    Alram_Sts[1].b7 = 1;            // Forcing ON Status Change to Module1(Not real, Because, Transmission Delay)
                    Alram_Sts[2].b7 = 0;            // Forcing OFF Status Change to Module2(Not real, Because, Transmission Delay)
                    Recovery_Flag = 1;              // Recovery Flag Set(Recovery Mode)
                    OP_CMD = 2;                 // Forcing Operation Command Module2 for Recovery Mode Memory
                }
                RST_cnt++;                      //Restart Count Increase
            }            
        }
    }
    
    //-------------------------- Change Condition ----------------------------// 
    else if((RST_Time <= RST_Timer || Recovery_Flag == 0) && Alram_Sts[1].b7 == 1 && Alram_Sts[2].b7 == 1)
    {
        Set_OK = 1;                     // Forcing Set, Change Bus Stop Condition of RS-485 before to Send Operation Command
        Request_Send();                 // Forcing Operation Command Send to Modules
        if(OP_CMD == 1)                 // Module 1
        {
            Alram_Sts[1].b7 = 1;        // Forcing ON Status Change to Module1(Not real, Because, Transmission Delay)
            Alram_Sts[2].b7 = 0;        // Forcing OFF Status Change to Module2(Not real, Because, Transmission Delay)
        }
        else if(OP_CMD == 2)            // Module 2
        {
            Alram_Sts[1].b7 = 0;        // Forcing OFF Status Change to Module1(Not real, Because, Transmission Delay)
            Alram_Sts[2].b7 = 1;        // Forcing ON Status Change to Module2(Not real, Because, Transmission Delay)
        }
    }
    
    else
    {
        Set_OK = 0;         // Forcing Change Normal Condition RS485 Bus
    }
    
}    


