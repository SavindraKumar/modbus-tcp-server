//! @addtogroup ModbusTCPUserFunction
//! @brief User functions
//! @{
//!
//****************************************************************************/
//! @file mbap_user.c
//! @brief User functions for reading/writing modbus data
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
#include "mbap_user.h"
#include "mbap_debug.h"
#include "mbap_conf.h"

//****************************************************************************/
//                           Defines and typedefs
//****************************************************************************/

//****************************************************************************/
//                           external variables
//****************************************************************************/
int16_t g_sInputRegsBuf[MAX_INPUT_REGISTERS]                = {1, 2, 3};
int16_t g_sHoldingRegsBuf[MAX_HOLDING_REGISTERS]            = {5, 6, 7};
uint8_t g_ucDiscreteInputsBuf[DISCRETE_INPUT_BUF_SIZE]      = {0xef};
uint8_t g_ucCoilsBuf[COILS_BUF_SIZE]                        = {5};
int16_t g_sHoldingRegsLowerLimitBuf[MAX_HOLDING_REGISTERS]  = {0, 0, 0};
int16_t g_sHoldingRegsHigherLimitBuf[MAX_HOLDING_REGISTERS] = {200, 200, 200};

//****************************************************************************/
//                           Local variables
//****************************************************************************/

//****************************************************************************/
//                           Local Functions
//****************************************************************************/
//
//! @brief Read discrete inputs from user data
//! @param[in]   usStartAddress Discrete inputs start address
//! @param[in]   sNumOfData     Number of discrete inputs to read
//! @param[out]  pucRecBuf      Receive buffer holds read discrete inputs
//! @return      None
//
static void ReadDiscreteInputs(uint16_t usStartAddress,
                               int16_t sNumOfData,
                               uint8_t *pucRecBuf);

//
//! @brief Read coils from user data
//! @param[in]   usStartAddress Coils start address
//! @param[in]   sNumOfData     Number of coils to read
//! @param[out]  pucRecBuf      Receive buffer holds read coils
//! @return      None
//
static void ReadCoils(uint16_t usStartAddress,
                      int16_t sNumOfData,
                      uint8_t *pucRecBuf);

//
//! @brief Read input registers from user data
//! @param[in]   usStartAddress Input registers start address
//! @param[in]   usNumOfData    Number of input registers to read
//! @param[out]  pucRecBuf      Receive buffer holds input registers
//! @return      None
//
static void ReadInputRegisters(uint16_t usStartAddress,
                               uint16_t usNumOfData,
                               uint8_t *pucRecBuf);

//
//! @brief Read holding registers from user data
//! @param[in]   usStartAddress Holding registers start address
//! @param[in]   usNumOfData    Number of holding registers to read
//! @param[out]  pucRecBuf      Receive buffer holds holding registers
//! @return      None
static void ReadHoldingRegisters(uint16_t usStartAddress,
                                 uint16_t usNumOfData,
                                 uint8_t *pucRecBuf);

//
//! @brief Write holding registers into user data
//! @param[in]   usStartAddress Holding registers start address
//! @param[in]   usNumOfData    Number of holding registers to write
//! @param[out]  pucWriteBuf    Write buffer holds holding registers
//! @return      None
static void WriteHoldingRegisters(uint16_t usStartAddress,
                                  uint16_t usNumOfData,
                                  const uint8_t *pucWriteBuf);

//
//! @brief Write Coils into user data
//! @param[in]   usStartAddress Coils start address
//! @param[in]   sNumOfData     Number of Coils to write
//! @param[out]  pucWriteBuf    Write buffer holds Coils
//! @return      None
static void WriteCoils(uint16_t usStartAddress,
                       int16_t sNumOfData,
                       const uint8_t *pucWriteBuf);

//****************************************************************************/
//                    G L O B A L  F U N C T I O N S
//****************************************************************************/
void mu_Init(void)
{
    ModbusData_t tModbusData;

    //Init modbus data
    tModbusData.usInputRegisterStartAddress   = INPUT_REGISTER_START_ADDRESS;
    tModbusData.usMaxInputRegisters           = MAX_INPUT_REGISTERS;
    tModbusData.usHoldingRegisterStartAddress = HOLDING_REGISTER_START_ADDRESS;
    tModbusData.usMaxHoldingRegisters         = MAX_HOLDING_REGISTERS;
    tModbusData.psHoldingRegisterLowerLimit   = g_sHoldingRegsLowerLimitBuf;
    tModbusData.psHoldingRegisterHigherLimit  = g_sHoldingRegsHigherLimitBuf;
    tModbusData.usDiscreteInputStartAddress   = DISCRETE_INPUTS_START_ADDRESS;
    tModbusData.usMaxDiscreteInputs           = MAX_DISCRETE_INPUTS;
    tModbusData.usCoilsStartAddress           = COILS_START_ADDRESS;
    tModbusData.usMaxCoils                    = MAX_COILS;
    tModbusData.ptfnReadInputRegisters        = ReadInputRegisters;
    tModbusData.ptfnReadHoldingRegisters      = ReadHoldingRegisters;
    tModbusData.ptfnReadDiscreteInputs        = ReadDiscreteInputs;
    tModbusData.ptfnReadCoils                 = ReadCoils;
    tModbusData.ptfnWriteHoldingRegisters     = WriteHoldingRegisters;
    tModbusData.ptfnWriteCoils                = WriteCoils;

    //pass modbus data data pointer to modbus tcp application
    mbap_DataInit(tModbusData);
}

//****************************************************************************/
//                           L O C A L  F U N C T I O N S
//****************************************************************************/
static void ReadInputRegisters(uint16_t usStartAddress,
                               uint16_t usNumOfData,
                               uint8_t *pucRecBuf)
{
    MBT_DEBUGF(MBT_CONF_DEBUG_LEVEL_MSG, "Read Input Registers User function\r\n");

    while (usNumOfData > 0)
    {
        *pucRecBuf++ = (uint8_t)(g_sInputRegsBuf[usStartAddress] >> 8);
        *pucRecBuf++ = (uint8_t)(g_sInputRegsBuf[usStartAddress] & 0xFF);
        usStartAddress++;
        usNumOfData--;
    }
}//end ReadInputRegisters

static void ReadDiscreteInputs(uint16_t usStartAddress,
                               int16_t sNumOfData,
                               uint8_t *pucRecBuf)
{
    MBT_DEBUGF(MBT_CONF_DEBUG_LEVEL_MSG, "Read Discrete Inputs User function\r\n");

    while (sNumOfData > 0)
    {
        uint16_t  usTmpBuf;
        uint16_t  usMask;
        uint16_t  usByteOffset;
        uint16_t  usNPreBits;

        usByteOffset = usStartAddress  / 8 ;
        usNPreBits   = usStartAddress - usByteOffset * 8;
        usMask       = (1 << sNumOfData)  - 1 ;
        usTmpBuf     = g_ucDiscreteInputsBuf[usByteOffset];
        usTmpBuf    |= g_ucDiscreteInputsBuf[usByteOffset + 1] << 8;
        // throw away unneeded bits
        usTmpBuf     = usTmpBuf >> usNPreBits;
        // mask away bits above the requested bitfield
        usTmpBuf     = usTmpBuf & usMask;
        *pucRecBuf++ = (uint8_t)usTmpBuf;

        sNumOfData     = sNumOfData - 8;
        usStartAddress = usStartAddress - 8;
    }
}//end ReadDiscreteInputs

static void ReadCoils(uint16_t usStartAddress,
                      int16_t sNumOfData,
                      uint8_t *pucRecBuf)
{
    MBT_DEBUGF(MBT_CONF_DEBUG_LEVEL_MSG, "Read Coils User function\r\n");

    while (sNumOfData > 0)
    {
        uint16_t  usTmpBuf;
        uint16_t  usMask;
        uint16_t  usByteOffset;
        uint16_t  usNPreBits;

        usByteOffset = usStartAddress / 8;
        usNPreBits   = usStartAddress - usByteOffset * 8;
        usMask       = (1 << sNumOfData) - 1;
        usTmpBuf     = g_ucCoilsBuf[usByteOffset];
        usTmpBuf    |= g_ucCoilsBuf[usByteOffset + 1] << 8;

        // throw away unneeded bits
        usTmpBuf = usTmpBuf >> usNPreBits;
        // mask away bits above the requested bitfield
        usTmpBuf     = usTmpBuf & usMask;
        *pucRecBuf++ = (uint8_t)usTmpBuf;
        sNumOfData   = sNumOfData - 8;
    }
}//end ReadCoils

static void ReadHoldingRegisters(uint16_t usStartAddress,
                                 uint16_t usNumOfData,
                                 uint8_t *pucRecBuf)
{
    MBT_DEBUGF(MBT_CONF_DEBUG_LEVEL_MSG, "Read Holding Registers User function\r\n");

    while (usNumOfData > 0)
    {
        *pucRecBuf++ = (uint8_t)(g_sHoldingRegsBuf[usStartAddress] >> 8);
        *pucRecBuf++ = (uint8_t)(g_sHoldingRegsBuf[usStartAddress] & 0xFF);
        usStartAddress++;
        usNumOfData--;
    }
}//end ReadHoldingRegisters

static void WriteHoldingRegisters(uint16_t usStartAddress,
                                  uint16_t usNumOfData,
                                  const uint8_t *pucWriteBuf)
{
    MBT_DEBUGF(MBT_CONF_DEBUG_LEVEL_MSG, "Write Holding Registers User function\r\n");

    uint8_t  ucCount = 0;

    while (usNumOfData > 0)
    {
        uint16_t usValue = 0;

        usValue  = (uint16_t)(pucWriteBuf[ucCount] << 8);
        ucCount++;
        usValue |= (uint16_t)(pucWriteBuf[ucCount]);
        g_sHoldingRegsBuf[usStartAddress] = (int16_t)usValue;

        ucCount++;
        usStartAddress++;
        usNumOfData--;
    }
}//end WriteHoldingRegisters

static void WriteCoils(uint16_t usStartAddress,
                       int16_t sNumOfData,
                       const uint8_t *pucWriteBuf)
{
    uint8_t ucCount = 0;

    MBT_DEBUGF(MBT_CONF_DEBUG_LEVEL_MSG, "Write Coils User function\r\n");

    while (sNumOfData > 0)
    {
        uint16_t  usTmp;
        uint16_t  usMask;
        uint16_t  usByteOffset;
        uint16_t  usNPreBits;
        uint8_t   ucNumOfBits;
        uint16_t  usCoilValue;

        ucNumOfBits = (sNumOfData > 8 ? 8 : sNumOfData);

        // Calculate byte offset for first byte containing the bit values starting
        // at usBitOffset
        usByteOffset = usStartAddress / 8;

        // How many bits precede our bits to set
        usNPreBits = usStartAddress - usByteOffset * 8;

        if (1 == sNumOfData)
        {
            usCoilValue         = (uint16_t)(pucWriteBuf[0] << 8);
            usCoilValue        |= (uint16_t)(pucWriteBuf[1]);

            if (0xFF00 == usCoilValue)
            {
                //Turn on coil
                usCoilValue = 1;
            }
            else if (0x0000 == usCoilValue)
            {
                //Turn off coil
                usCoilValue = 0;
            }
        }
        else
        {
            usCoilValue = (uint16_t)pucWriteBuf[ucCount];
        }

        // Move bit field into position over bits to set
        usCoilValue <<= usNPreBits;

        // Prepare a mask for setting the new bits
        usMask   = (uint16_t)((1 << ucNumOfBits) - 1);
        usMask <<= usStartAddress - usByteOffset * 8;

        // copy bits into temporary storage
        usTmp  = g_ucCoilsBuf[usByteOffset];
        usTmp |= g_ucCoilsBuf[usByteOffset + 1] << 8;

        // Zero out bit field bits and then or value bits into them
        usTmp = (usTmp & (~usMask)) | usCoilValue;

        // move bits back into storage
        g_ucCoilsBuf[usByteOffset]     = (uint8_t)(usTmp & 0xFF);
        g_ucCoilsBuf[usByteOffset + 1] = (uint8_t)(usTmp >> 8);

        sNumOfData = sNumOfData - 8;

        ucCount++;
    }
}//end WriteCoils
//****************************************************************************/
//                             End of file
//****************************************************************************/
/** @}*/
