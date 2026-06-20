/***************************************************************************
 * Project : Dashboard Design using LPC2129
 * Features:
 *  - Left/Right Turn Indicator Control using External Interrupts
 *  - Engine Temperature Monitoring using DS18B20 Sensor
 *  - Fuel Level Display through CAN Communication
 *  - LCD Display Interface
 *
 * Controller : LPC2129
 * Communication : CAN Protocol
 * Sensor : DS18B20 Temperature Sensor
 * Display : 16x2 LCD
 ***************************************************************************/

#include <lpc21xx.h>
#include "Major_adc.h"
#include "Major_adc_defines.h"
#include "Major_lcd.h"
#include "Major_dsb18b20.h"
#include "types.h"
#include "delay.h"
#include "Major_Can.h"

/* Indicator States */
#define OFF   0
#define LEFT  1
#define RIGHT 2

/* Global Variables */
u32 adcvalue;
f32 voltage;
int temp;
int Ful;
u8 percent = 0;
u8 battery_percent, i;
u32 level = 0;

/*----------------------------------------------------------
  CAN Communication Variables
----------------------------------------------------------*/
CANF txF;     // CAN Transmit Frame
CANF rxF;     // CAN Receive Frame

/*----------------------------------------------------------
  Indicator Status Variable
----------------------------------------------------------*/
volatile u8 indicator_status = OFF;

/*----------------------------------------------------------
  Custom LCD Characters
  Used for Left and Right Indicator Symbols
----------------------------------------------------------*/

u8 left_arrow[8] ={0x03,0x07,0x0F,0x1F,0x0F,0x07,0x03,0x01};
u8 right_arrow[8] ={0x18,0x1C,0x1E,0x1F,0x1E,0x1C,0x18,0x10};

/* Function Prototypes */
void LoadCustomChars(void);
void sendsignal(void);
void eint0_isr(void) __irq;
void eint1_isr(void) __irq;

u8 tp, tpd;

int main(void)
{
    /* Initialize LCD */
    InitLCD();
    /* Initialize CAN Controller */
    Init_CAN1();
    /* Display Project Title */
    CmdLCD(0x80);
    StrLCD(" DASHBOARD DESIGN");
    /* Display Indicator Label */
    CmdLCD(0xC0);
    StrLCD("INDICATOR:");
    /* Load Custom Arrow Symbols into LCD CGRAM */
    LoadCustomChars();
    /* Display Both Arrows Initially */
    CmdLCD(0xC0 + 13);
    CharLCD(5);      // Left Arrow
    CmdLCD(0xC0 + 15);
    CharLCD(6);      // Right Arrow

    /**********************************************************************
     * External Interrupt Configuration
     * EINT0 -> Left Indicator
     * EINT1 -> Right Indicator
     **********************************************************************/
    CfgPinFunc(0,1,3);   // P0.1 as EINT0
    CfgPinFunc(0,3,3);   // P0.3 as EINT1

    /* Configure Interrupts as Edge Triggered */
    EXTMODE |= (1<<0) | (1<<1);

    /* Falling Edge Trigger */
    EXTPOLAR &= ~((1<<0) | (1<<1));

    /* Clear Pending Interrupt Flags */
    EXTINT = (1<<0) | (1<<1);

    /* Configure VIC for EINT0 */
    VICVectCntl0 = (1<<5) | 14;
    VICVectAddr0 = (u32)eint0_isr;

    /* Configure VIC for EINT1 */
    VICVectCntl1 = (1<<5) | 15;
    VICVectAddr1 = (u32)eint1_isr;

    /* Enable Interrupts */
    VICIntEnable = (1<<14) | (1<<15);

    while(1)
    {
        /*------------------------------------------------------
          LEFT Indicator Blinking
        ------------------------------------------------------*/
        if(indicator_status == LEFT)
        {
            CmdLCD(0xC0 + 13);
            CharLCD(5);

            delay_ms(300);

            CmdLCD(0xC0 + 13);
            CharLCD(' ');

            CmdLCD(0xC0 + 15);
            CharLCD(6);
        }

        /*------------------------------------------------------
          RIGHT Indicator Blinking
        ------------------------------------------------------*/
        else if(indicator_status == RIGHT)
        {
            CmdLCD(0xC0 + 13);
            CharLCD(5);

            CmdLCD(0xC0 + 15);
            CharLCD(6);

            delay_ms(300);

            CmdLCD(0xC0 + 15);
            CharLCD(' ');
        }

        /*------------------------------------------------------
          Indicator OFF State
        ------------------------------------------------------*/
        else
        {
            CmdLCD(0xC0 + 13);
            CharLCD(5);

            CmdLCD(0xC0 + 15);
            CharLCD(6);
        }

        delay_ms(100);

        /*------------------------------------------------------
          Read Engine Temperature from DS18B20 Sensor
        ------------------------------------------------------*/
        temp = ReadTemp();

        tp = temp >> 4;

        CmdLCD(0xD4);
        StrLCD("ENG TEMP:");
        IntLCD(tp);
        CharLCD(0xDF);      // Degree Symbol
        StrLCD(" C   ");

        /*------------------------------------------------------
          Receive Fuel Percentage via CAN
        ------------------------------------------------------*/
        CAN1_Rx(&rxF);

        CmdLCD(0x94);
        StrLCD("FUEL: ");

        IntLCD(rxF.Data1);
        CharLCD('%');

        percent = rxF.Data1;

        /*------------------------------------------------------
          Fuel Gauge Display
          1 Bar  -> 0-25%
          2 Bars -> 26-50%
          3 Bars -> 51-75%
          4 Bars -> 76-100%
        ------------------------------------------------------*/
        if(percent <= 25)
        {
            CharLCD(2);
            CharLCD(' ');
            CharLCD(' ');
            CharLCD(' ');
        }
        else if(percent <= 50)
        {
            CharLCD(2);
            CharLCD(3);
            CharLCD(' ');
            CharLCD(' ');
        }
        else if(percent <= 75)
        {
            CharLCD(2);
            CharLCD(3);
            CharLCD(4);
            CharLCD(' ');
        }
        else
        {
            CharLCD(2);
            CharLCD(3);
            CharLCD(4);
            CharLCD(5);
        }

        delay_ms(200);
    }
}

/***************************************************************************
 * EINT0 Interrupt Service Routine
 * Function : Toggle Left Indicator
 ***************************************************************************/
void eint0_isr(void) __irq
{
    IOPIN1 ^= (1<<16);

    indicator_status =
        (indicator_status == LEFT) ? OFF : LEFT;

    sendsignal();

    EXTINT = (1<<0);
    VICVectAddr = 0;
}

/***************************************************************************
 * EINT1 Interrupt Service Routine
 * Function : Toggle Right Indicator
 ***************************************************************************/
void eint1_isr(void) __irq
{
    indicator_status =
        (indicator_status == RIGHT) ? OFF : RIGHT;

    sendsignal();

    EXTINT = (1<<1);
    VICVectAddr = 0;
}

/***************************************************************************
 * CAN Transmission Function
 * Sends Indicator Status to Other Nodes
 *
 * 'L' -> Left Indicator ON
 * 'R' -> Right Indicator ON
 * 'O' -> Indicator OFF
 ***************************************************************************/
void sendsignal(void)
{
    txF.ID = 0x11;

    txF.BFV.RTR = 0;
    txF.BFV.DLC = 1;

    txF.Data1 =
        (indicator_status == LEFT)  ? 'L' :
        (indicator_status == RIGHT) ? 'R' : 'O';

    CAN1_Tx(txF);
}

/***************************************************************************
 * Load Custom Arrow Characters into LCD CGRAM
 ***************************************************************************/
void LoadCustomChars(void)
{
    u8 i;

    /* Store Left Arrow Pattern */
    CmdLCD(0x68);
    for(i = 0; i < 8; i++)
        CharLCD(left_arrow[i]);

    /* Store Right Arrow Pattern */
    CmdLCD(0x70);
    for(i = 0; i < 8; i++)
        CharLCD(right_arrow[i]);
}