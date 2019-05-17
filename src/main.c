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
#include "mbap_conf.h"
#include "tcp.h"

//****************************************************************************/
//                           Defines and typedefs
//****************************************************************************/
#define INPUT_REGISTER_START_ADDRESS      (0u)
#define MAX_INPUT_REGISTERS               (15u)
#define HOLDING_REGISTER_START_ADDRESS    (0u)
#define MAX_HOLDING_REGISTERS             (15u)
#define DISCRETE_INPUTS_START_ADDRESS     (0u)
#define MAX_DISCRETE_INPUTS               (3u)
#define COILS_START_ADDRESS               (0u)
#define MAX_COILS                         (3u)
#define DISCRETE_INPUT_BUF_SIZE           (MAX_DISCRETE_INPUTS / 8u + 1u)
#define COILS_BUF_SIZE                    (MAX_COILS / 8u + 1u)

//****************************************************************************/
//                           external variables
//****************************************************************************/

//****************************************************************************/
//                           Local variables
//****************************************************************************/
int16_t g_sInputRegsBuf[MAX_INPUT_REGISTERS]                = {1, 2, 3};
int16_t g_sHoldingRegsBuf[MAX_HOLDING_REGISTERS]            = {5, 6, 7};
uint8_t g_ucDiscreteInputsBuf[DISCRETE_INPUT_BUF_SIZE]      = {0xef};
uint8_t g_ucCoilsBuf[COILS_BUF_SIZE]                        = {5};
int16_t g_sHoldingRegsLowerLimitBuf[MAX_HOLDING_REGISTERS]  = {0, 0, 0};
int16_t g_sHoldingRegsHigherLimitBuf[MAX_HOLDING_REGISTERS] = {200, 200, 200};
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

    ModbusData.psInputRegisters              = g_sInputRegsBuf;
    ModbusData.usInputRegisterStartAddress   = INPUT_REGISTER_START_ADDRESS;
    ModbusData.usMaxInputRegisters           = MAX_INPUT_REGISTERS;
    ModbusData.psHoldingRegisters            = g_sHoldingRegsBuf;
    ModbusData.usHoldingRegisterStartAddress = HOLDING_REGISTER_START_ADDRESS;
    ModbusData.usMaxHoldingRegisters         = MAX_HOLDING_REGISTERS;
    ModbusData.psHoldingRegisterLowerLimit   = g_sHoldingRegsLowerLimitBuf;
    ModbusData.psHoldingRegisterHigherLimit  = g_sHoldingRegsHigherLimitBuf;
    ModbusData.pucDiscreteInputs             = g_ucDiscreteInputsBuf;
    ModbusData.usDiscreteInputStartAddress   = DISCRETE_INPUTS_START_ADDRESS;
    ModbusData.usMaxDiscreteInputs           = MAX_DISCRETE_INPUTS;
    ModbusData.pucCoils                      = g_ucCoilsBuf;
    ModbusData.usCoilsStartAddress           = COILS_START_ADDRESS;
    ModbusData.usMaxCoils                    = MAX_COILS;

    mbap_DataInit(&ModbusData);
    tcp_Init();

    return 0;
}//end main

//****************************************************************************/
//                           L O C A L  F U N C T I O N S
//****************************************************************************/

//****************************************************************************/
//                             End of file
//****************************************************************************/
/** @}*/
