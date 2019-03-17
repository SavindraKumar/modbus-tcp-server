//! @addtogroup Main
//! @brief Initialize Modbus application and TCP Interface from main application
//! @{
//!
//****************************************************************************/
//! @file main.c
//! @brief Start of application
//! @author Savindra Kumar(savindran1989@gmail.com)
//! @bug No known bugs.
//!
//****************************************************************************/
//                           Includes
//****************************************************************************/
//standard header files
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//user defined files
#include "mbtcpconf.h"
#include "tcp.h"

//****************************************************************************/
//                           Defines and typedefs
//****************************************************************************/
#define INPUT_REGISTER_START_ADDRESS         0
#define NUMBER_OF_INPUT_REGISTERS            10

//****************************************************************************/
//                           external variables
//****************************************************************************/

//****************************************************************************/
//                           Local variables
//****************************************************************************/
int16_t g_sInputRegisters[NUMBER_OF_INPUT_REGISTERS] =
{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

//****************************************************************************/
//                           Local Functions
//****************************************************************************/

//****************************************************************************/
//                    G L O B A L  F U N C T I O N S
//****************************************************************************/
//
//! @brief main function
//! @param[in]  None
//! @return     int
//
int main(void)
{
    ModbusData_t ModbusData;

    ModbusData.psInputRegisters            = g_sInputRegisters;
    ModbusData.usInputRegisterStartAddress = INPUT_REGISTER_START_ADDRESS;
    ModbusData.usNumOfInputRegisters       = NUMBER_OF_INPUT_REGISTERS;

    MBT_DataInit(&ModbusData);
    TcpInit();

    return 0;
}//end main

//****************************************************************************/
//                           L O C A L  F U N C T I O N S
//****************************************************************************/

//****************************************************************************/
//                             End of file
//****************************************************************************/
/** @}*/
