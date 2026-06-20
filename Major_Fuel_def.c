/*****************************************************************************
 * Project : Fuel Level Monitoring Node
 * Description:
 * This node reads the fuel level using an analog sensor connected to the ADC
 * channel of LPC2129. The ADC value is converted into a percentage and
 * transmitted to other nodes through CAN communication.
 * Features:
 *  - ADC-based Fuel Level Measurement
 *  - Fuel Percentage Calculation
 *  - CAN Bus Transmission
 * Controller : LPC2129
 * Communication : CAN Protocol
 * Sensor : Analog Fuel Level Sensor / Potentiometer
 *****************************************************************************/

#include <LPC21XX.h>
#include "types.h"
#include "Major_adc_defines.h"
#include "Major_adc.h"
#include "delay.h"
#include "Major_Can.h"
#include "Major_candefines.h"
#include "Major_lcd.h"

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
f32 aR1;            // Reserved for analog calculations
u32 adcdval;        // Raw ADC value
u8 fuel;            // Fuel percentage
f32 voltage;        // ADC voltage value
CANF txF;           // CAN Transmit Frame

int main(void)
{
    /* Initialize CAN Controller */
    Init_CAN1();

    /* Initialize ADC Channel 0 */
    initadc(0);

    /* Configure CAN Frame */
    txF.ID = 1;          // CAN Message Identifier
    txF.BFV.RTR = 0;     // Data Frame (Not Remote Frame)
    txF.BFV.DLC = 4;     // Data Length Code

    while(1)
    {
         /* Read Analog Fuel Sensor Value*/
        Read_ADC(0, &voltage, &adcdval);
        /*********************************************************************
         * Convert ADC Reading to Fuel Percentage
         * ADC Range : 0 - 1023 (10-bit ADC)
         * Fuel Range: 0 - 100 %
         *********************************************************************/
        fuel = ((adcdval * 100) / 1023);
        /* Limit Maximum Value to 100% */
        if(fuel > 100)
            fuel = 100;
         /* Transmit Fuel Percentage via CAN**/
        txF.Data1 = fuel;
        CAN1_Tx(txF);
        /* Update every 100 ms */
        delay_ms(100);
    }
}
