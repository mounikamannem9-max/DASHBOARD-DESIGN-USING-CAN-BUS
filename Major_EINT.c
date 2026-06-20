/*****************************************************************************
 * Project : Dashboard Indicator Receiver Node
 *
 * Description:
 * This node receives indicator status messages through CAN communication
 * and controls an LED sequence to simulate vehicle turn indicators.
 *
 * Features:
 *  - Receives CAN messages from Dashboard Node
 *  - Left Indicator LED Animation
 *  - Right Indicator LED Animation
 *  - Indicator OFF Control
 *
 * Controller : LPC2129
 * Communication : CAN Protocol
 * Output : 8 LEDs connected to Port1 (P1.23 - P1.30)
 *****************************************************************************/

#include <LPC21xx.h>
#include "delay.h"
#include "Major_Can.h"
#include "types.h"

/* Indicator States */
#define OFF   0
#define LEFT  1
#define RIGHT 2

/* LED Configuration */
#define LED_START 23
#define LED_MASK  (0xFF << LED_START)

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
CANF rxF;                      // CAN Receive Frame

volatile u8 indicator = OFF;   // Current indicator status
volatile u8 pos = 0;           // Current LED position

/* Function Prototypes */
void blink_left_step(void);
void blink_right_step(void);

int main()
{
    /* Initialize CAN Controller */
    Init_CAN1();

    /* Configure LEDs as Output */
    IODIR1 |= LED_MASK;

    /* Turn OFF all LEDs initially
       (Active-Low LEDs) */
    IOSET1 = LED_MASK;

    while (1)
    {
        /*********************************************************************
         * CAN Message Reception
         *********************************************************************/
        if (C1GSR & (1 << 0))      // Check Receive Buffer Status
        {
            CAN1_Rx(&rxF);         // Receive CAN Frame

            /* Release Receive Buffer */
            C1CMR = (1 << 2);

            /* Process Indicator Messages */
            if (rxF.ID == 0x11)
            {
                /* Left Indicator Command */
                if (rxF.Data1 == 'L')
                {
                    indicator = LEFT;
                    /* Start animation from rightmost LED */
                    pos = 7;
                }

                /* Right Indicator Command */
                else if (rxF.Data1 == 'R')
                {
                    indicator = RIGHT;

                    /* Start animation from leftmost LED */
                    pos = 0;
                }

                /* Indicator OFF Command */
                else if (rxF.Data1 == 'O')
                {
                    indicator = OFF;
                    pos = 0;
                    /* Turn OFF all LEDs */
                    IOSET1 = LED_MASK;
                }
            }
        }
        if (indicator == LEFT)
        {
            blink_left_step();
        }
        else if (indicator == RIGHT)
        {
            blink_right_step();
        }
        else
        {
            /* Keep all LEDs OFF */
            IOSET1 = LED_MASK;
        }

        delay_ms(50);
    }
}

/*****************************************************************************
 * Function : blink_right_step()
 * Description:
 * Generates Right Indicator Animation
 * LEDs shift from Left ? Right
 *****************************************************************************/
void blink_right_step(void)
{
    /* Turn OFF all LEDs */
    IOSET1 = LED_MASK;

    /* Turn ON current LED */
    IOCLR1 = (1 << (LED_START + pos));

    /* Move to next LED */
    pos++;

    /* Restart sequence after last LED */
    if (pos >= 8)
        pos = 0;
}

/*****************************************************************************
 * Function : blink_left_step()
 * Description:
 * Generates Left Indicator Animation
 * LEDs shift from Right ? Left
 *****************************************************************************/
void blink_left_step(void)
{
    /* Turn OFF all LEDs */
    IOSET1 = LED_MASK;

    /* Move towards left side */
    if (pos == 0)
        pos = 7;
    else
        pos--;

    /* Turn ON current LED */
    IOCLR1 = (1 << (LED_START + pos));
}