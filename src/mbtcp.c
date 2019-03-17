//! @addtogroup ModbusTCP
//! @brief Modbus TCP Application
//! @{
//!
//****************************************************************************/
//! @file mbtcp.c
//! @brief Modbus TCP Application source file
//! @author Savindra Kumar(savindran1989@gmail.com)
//! @bug No known bugs.
//!
//****************************************************************************/
//****************************************************************************/
//                           Includes
//****************************************************************************/
//standard header files
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
//user defined header files
#include "mbtcpconf.h"
#include "mbtcp.h"

//****************************************************************************/
//                           Defines and typedefs
//****************************************************************************/
#define MBT_PROTOCOL_ID                     0
#define DEVICE_ID                           1
#define MBAP_HEADER_LENGTH                  7
//Modbus Application Protocol Header
#define MBAP_TRANSACTION_ID_OFFSET          0
#define MBAP_PROTOCOL_ID_OFFSET             2
#define MBAP_LENGTH_OFFSET                  4
#define MBAP_UNIT_ID_OFFSET                 6
//PDU Offset in query
#define MBT_FUNCTION_CODE_OFFSET            7
#define MBT_DATA_START_ADDRESS_OFFSET       8
#define MBT_NO_OF_DATA_OFFSET               10
//PDU Offset in response
#define MBT_BYTE_COUNT_OFFSET               8
#define MBT_DATA_VALUES_OFFSET              9

#define MBT_MAX_PDU_LENGTH                  256
#define MBT_EXCEPTION_PACKET_LENGTH         9

//****************************************************************************/
//                           Private Functions
//****************************************************************************/
static uint16_t HandleRequest (const uint8_t *pucQuery, uint8_t *pucResponse);
static uint8_t ValidateFunctionCodeAndDataAddress (const uint8_t *pucQuery);
static bool BasicValidation (const uint8_t *pucQuery);
static uint16_t ReadCoils (const uint8_t *pucQuery, uint8_t *pucResponse);
static uint16_t ReadDiscreteInputs (const uint8_t *pucQuery, uint8_t *pucResponse);
static uint16_t ReadHoldingRegisters (const uint8_t *pucQuery, uint8_t *pucResponse);
static uint16_t ReadInputRegisters (const uint8_t *pucQuery, uint8_t *pucResponse);
static uint16_t WriteSingleCoil (const uint8_t *pucQuery, uint8_t *pucResponse);
static uint16_t WriteSingleHoldingRegister (const uint8_t *pucQuery, uint8_t *pucResponse);
static uint16_t WriteMultipleCoils (const uint8_t *pucQuery, uint8_t *pucResponse);
static uint16_t WriteMultipleHoldingRegisters (const uint8_t *pucQuery, uint8_t *pucResponse);
static uint16_t BuildExceptionPacket (const uint8_t *pucQuery, uint8_t ucException, uint8_t *pucResponse);

//****************************************************************************/
//                           external variables
//****************************************************************************/

//****************************************************************************/
//                           Private variables
//****************************************************************************/
static const ModbusData_t *m_ModbusData = NULL;

//****************************************************************************/
//                    G L O B A L  F U N C T I O N S
//****************************************************************************/
//
//! @brief Intialise Modbus Data
//! @param[in]  ModbusData  Modbus data structure
//! @return     None
//
void MBT_DataInit(const ModbusData_t *ModbusData)
{
    m_ModbusData = ModbusData;

} //end MBT_DataInit

//
//! @brief Process Modbus TCP Application request
//! @param[in]   pucQuery      Pointer to Modbus TCP Query buffer
//! @param[in]   ucQueryLength Modbus TCP Query Length
//! @param[out]  pucResponse   Pointer to Modbus TCP Response buffer
//! @return      uint16_t      Modbus TCP Response Length
//
uint16_t MBT_ProcessRequest(const uint8_t *pucQuery, uint8_t ucQueryLength, uint8_t *pucResponse)
{
    uint16_t pusResponseLength = 0;
    uint8_t  ucException       = 0;
    bool     bStatus           = false;

    bStatus = BasicValidation(pucQuery);

    //If Protocol Id, Pdu length or Unit Id validated sucessfully
    //Proceed for next validation steps
    if (bStatus)
    {
        ucException = ValidateFunctionCodeAndDataAddress(pucQuery);

        if (ucException)
        {
            pusResponseLength = BuildExceptionPacket(pucQuery, ucException, pucResponse);
        }
        else
        {
            pusResponseLength = HandleRequest(pucQuery, pucResponse);
        }
    }//end if

    return (pusResponseLength);
}//end MB_ProcessRequest

/******************************************************************************
 *                           L O C A L  F U N C T I O N S
 *****************************************************************************/
//
//! @brief Validate protocol id, uint id and pdu length
//! @param[in] pucQuery Pointer to modbus query buffer
//! @parm[out] None
//! @return    bool true - Validation ok, false - Validate not ok
//
static bool BasicValidation(const uint8_t *pucQuery)
{
    uint16_t usProtocolId = 0;
    uint16_t usPdulength  = 0;
    uint8_t  ucUnitId     = 0;
    bool     bStatus       = true;

    //Modbus Application Protocol(MBAP) Header Information
    usProtocolId  = (uint16_t)(pucQuery[MBAP_PROTOCOL_ID_OFFSET] << 8);
    usProtocolId |= (uint16_t)(pucQuery[MBAP_PROTOCOL_ID_OFFSET + 1]);
    usPdulength   = (uint16_t)(pucQuery[MBAP_LENGTH_OFFSET] << 8);
    usPdulength  |= (uint16_t)(pucQuery[MBAP_LENGTH_OFFSET + 1]);
    ucUnitId      = (uint8_t)(pucQuery[MBAP_UNIT_ID_OFFSET]);

    //check for Modbus TCP/IP protocol
    if (MBT_PROTOCOL_ID != usProtocolId)
    {
        bStatus = false;
    }

    //check if pdu length exceed
    if (usPdulength > MBT_MAX_PDU_LENGTH)
    {
        bStatus = false;
    }

    //check for Unit Id
    if (DEVICE_ID != ucUnitId)
    {
        bStatus = false;
    }

    return (bStatus);
}//end BasicValidation

//
//! @brief Validate function code and data address in modbus query
//! @param[in]  pucQuery Pointer to modbus query buffer
//! @param[out] None
//! @return     uint8_t 0 - NoException, nonzero - Exception
//
static uint8_t ValidateFunctionCodeAndDataAddress(const uint8_t *pucQuery)
{
    uint8_t  ucFunctionCode     = 0;
    uint16_t usDataStartAddress = 0;
    uint16_t usNumberOfData     = 0;
    uint8_t  ucException        = 0;

    //Modbus PDU Information
    ucFunctionCode      = (uint8_t)pucQuery[MBT_FUNCTION_CODE_OFFSET];
    usDataStartAddress  = (uint16_t)(pucQuery[MBT_DATA_START_ADDRESS_OFFSET] << 8);
    usDataStartAddress |= (uint16_t)(pucQuery[MBT_DATA_START_ADDRESS_OFFSET + 1]);
    usNumberOfData      = (uint16_t)(pucQuery[MBT_NO_OF_DATA_OFFSET] << 8);
    usNumberOfData     |= (uint16_t)(pucQuery[MBT_NO_OF_DATA_OFFSET + 1]);

    switch (ucFunctionCode)
    {
#if FC_READ_COILS_ENABLE
    case FC_READ_COILS:
    if (!((usDataStartAddress >= m_ModbusData->usCoilsStartAddress) &&
         ((usDataStartAddress + usNumberOfData) <= (m_ModbusData->usCoilsStartAddress + m_ModbusData->usNumOfCoils))))
    {
        ucException = ILLEGAL_DATA_ADDRESS;
    }
    break;
#endif

#if FC_READ_DISCRETE_INPUTS_ENABLE
    case FC_READ_DISCRETE_INPUTS:
    if (!((usDataStartAddress >= m_ModbusData->usDiscreteInputStartAddress) &&
         ((usDataStartAddress + usNumberOfData) <= (m_ModbusData->usDiscreteInputStartAddress + m_ModbusData->usNumDiscreteInputs))))
    {
        ucException = ILLEGAL_DATA_ADDRESS;
    }
    break;
#endif

#if FC_READ_HOLDING_REGISTERS_ENABLE
    case FC_READ_HOLDING_REGISTERS:
    if (!((usDataStartAddress >= m_ModbusData->usHoldingRegisterStartAddress) &&
         ((usDataStartAddress + usNumberOfData) <= (m_ModbusData->usHoldingRegisterStartAddress + m_ModbusData->usNumOfHoldingRegisters))))
    {
        ucException = ILLEGAL_DATA_ADDRESS;
    }
    break;
#endif

#if FC_READ_INPUT_REGISTERS_ENABLE
    case FC_READ_INPUT_REGISTERS:
        if (!((usDataStartAddress >= m_ModbusData->usInputRegisterStartAddress) &&
             ((usDataStartAddress + usNumberOfData) <= (m_ModbusData->usInputRegisterStartAddress + m_ModbusData->usNumOfInputRegisters))))

        {
            ucException = ILLEGAL_DATA_ADDRESS;
        }
        break;
#endif

#if FC_WRITE_COIL_ENABLE
        case FC_WRITE_COIL:
        if (!((usDataStartAddress >= m_ModbusData->usCoilsStartAddress) &&
             ((usDataStartAddress + usNumberOfData) <= (m_ModbusData->usCoilsStartAddress + m_ModbusData->usNumOfCoils))))
        {
            ucException = ILLEGAL_DATA_ADDRESS;
        }
        break;
#endif

#if FC_WRITE_HOLDING_REGISTER_ENABLE
        case FC_WRITE_HOLDING_REGISTER:
        if (!((usDataStartAddress >= m_ModbusData->usHoldingRegisterStartAddress) &&
             (usDataStartAddress <= (m_ModbusData->usHoldingRegisterStartAddress + m_ModbusData->usNumOfHoldingRegisters))))
        {
            ucException = ILLEGAL_DATA_ADDRESS;
        }
        break;
#endif

#if FC_WRITE_COILS_ENABLE
        case FC_WRITE_COILS:
        if (!((usDataStartAddress >= m_ModbusData->usCoilsStartAddress) &&
             ((usDataStartAddress + usNumberOfData) <= (m_ModbusData->usCoilsStartAddress + m_ModbusData->usNumOfCoils))))
        {
            ucException = ILLEGAL_DATA_ADDRESS;
        }
        break;
#endif

#if FC_WRITE_HOLDING_REGISTERS_ENABLE
        case FC_WRITE_HOLDING_REGISTERS:
        if (!((usDataStartAddress >= m_ModbusData->usHoldingRegisterStartAddress) &&
             ((usDataStartAddress + usNumberOfData) <= (m_ModbusData->usHoldingRegisterStartAddress + m_ModbusData->usNumOfHoldingRegisters))))
        {
            ucException = ILLEGAL_DATA_ADDRESS;
        }
        break;
#endif

    default:
        ucException = ILLEGAL_FUNCTION_CODE;
        break;
    }	//end switch

    return (ucException);
}//end ValidateFunctionCodeAndDataAddress

//
//! @brief Handle Modbus Request after function code data adddress validated successfully
//! @param[in]    pucQuery         Pointer to modbus query buffer
//! @param[out]   pucResponse      Pointer to modbus response buffer
//! @return       uint16_t         ResponeLength
//
static uint16_t HandleRequest(const uint8_t *pucQuery, uint8_t *pucResponse)
{
    uint8_t  ucFunctionCode   = 0;
    uint16_t usResponseLength = 0;

    // filter PDU information
    ucFunctionCode = (uint8_t) pucQuery[MBT_FUNCTION_CODE_OFFSET];

    switch (ucFunctionCode)
    {
    case FC_READ_COILS:
        usResponseLength = ReadCoils(pucQuery, pucResponse);
        break;
    case FC_READ_DISCRETE_INPUTS:
        usResponseLength = ReadDiscreteInputs(pucQuery, pucResponse);
        break;
    case FC_READ_HOLDING_REGISTERS:
        usResponseLength = ReadHoldingRegisters(pucQuery, pucResponse);
        break;
    case FC_READ_INPUT_REGISTERS:
        usResponseLength = ReadInputRegisters(pucQuery, pucResponse);
        break;
    case FC_WRITE_COIL:
        usResponseLength = WriteSingleCoil(pucQuery, pucResponse);
        break;
    case FC_WRITE_HOLDING_REGISTER:
        usResponseLength = WriteSingleHoldingRegister(pucQuery, pucResponse);
        break;
    case FC_WRITE_COILS:
        usResponseLength = WriteMultipleCoils(pucQuery, pucResponse);
        break;
    case FC_WRITE_HOLDING_REGISTERS:
        usResponseLength = WriteMultipleHoldingRegisters(pucQuery, pucResponse);
        break;
    default:
        usResponseLength = 0;
        break;
    }//end switch

    return (usResponseLength);
}//end HandleRequest

//
//! @brief Build Exception Packet
//! @param[in]    pucQuery     Pointer to modbus query buffer
//! @param[in]    ucException  Exception type
//! @param[out]   pucResponse  Pointer to modbus response buffer
//! @return       uint16_t     Response Length
//
static uint16_t BuildExceptionPacket(const uint8_t *pucQuery, uint8_t ucException, uint8_t *pucResponse)
{
    memcpy(pucResponse, pucQuery, MBAP_HEADER_LENGTH);
    //Modify information for response
    pucResponse[MBAP_LENGTH_OFFSET]       = 0;
    pucResponse[MBAP_LENGTH_OFFSET + 1]   = 3;
    pucResponse[MBT_FUNCTION_CODE_OFFSET] = 0x80 + pucQuery[MBT_FUNCTION_CODE_OFFSET];
    pucResponse[8]                        = ucException;

    return (MBT_EXCEPTION_PACKET_LENGTH);
}//end BuildExceptionPacket

//
//! @brief Read Coils from Modbus data
//! @param[in]  pucQuery    Pointer to modbus query buffer
//! @param[out] pucResponse Pointer to modbus response buffer
//! @return     uint16_t    Response Length
//
static uint16_t ReadCoils(const uint8_t *pucQuery, uint8_t *pucResponse)
{
    uint16_t usDataStartAddress = 0;
    uint16_t usNumberOfData     = 0;

    usDataStartAddress  = (uint16_t)(pucQuery[MBT_DATA_START_ADDRESS_OFFSET] << 8);
    usDataStartAddress |= (uint16_t)(pucQuery[MBT_DATA_START_ADDRESS_OFFSET + 1]);
    usNumberOfData      = (uint16_t)(pucQuery[MBT_NO_OF_DATA_OFFSET] << 8);
    usNumberOfData     |= (uint16_t)(pucQuery[MBT_NO_OF_DATA_OFFSET + 1]);

    return 0;
}//end ReadCoils

//
//! @brief Read Discrete Inputs from Modbus data
//! @param[in]   pucQuery   Pointer  to modbus query buffer
//! @param[out]  pucResponse Pointer to modbus response buffer
//! @return      uint16_t    Response Length
//
static uint16_t ReadDiscreteInputs(const uint8_t *pucQuery, uint8_t *pucResponse)
{
    uint16_t usDataStartAddress = 0;
    uint16_t usNumberOfData     = 0;

    usDataStartAddress  = (uint16_t)(pucQuery[MBT_DATA_START_ADDRESS_OFFSET] << 8);
    usDataStartAddress |= (uint16_t)(pucQuery[MBT_DATA_START_ADDRESS_OFFSET + 1]);
    usNumberOfData      = (uint16_t)(pucQuery[MBT_NO_OF_DATA_OFFSET] << 8);
    usNumberOfData     |= (uint16_t)(pucQuery[MBT_NO_OF_DATA_OFFSET + 1]);

    return 0;
}//end ReadDiscreteInputs

//
//! @brief Read Holding Registers from Modbus data
//! @param[in]   pucQuery   Pointer  to modbus query buffer
//! @param[out]  pucResponse Pointer to modbus response buffer
//! @return      uint16_t    Response Length
//
static uint16_t ReadHoldingRegisters(const uint8_t *pucQuery, uint8_t *pucResponse)
{
    uint16_t usDataStartAddress = 0;
    uint16_t usNumberOfData     = 0;
    uint16_t usPduLength        = 0;
    uint16_t usStartAddress     = 0;
    uint16_t usResponseLength   = 0;
    uint8_t  *pucRegBuffer      = NULL;

    usDataStartAddress  = (uint16_t)(pucQuery[MBT_DATA_START_ADDRESS_OFFSET] << 8);
    usDataStartAddress |= (uint16_t)(pucQuery[MBT_DATA_START_ADDRESS_OFFSET + 1]);
    usNumberOfData      = (uint16_t)(pucQuery[MBT_NO_OF_DATA_OFFSET] << 8);
    usNumberOfData     |= (uint16_t)(pucQuery[MBT_NO_OF_DATA_OFFSET + 1]);

    usStartAddress = (usDataStartAddress - m_ModbusData->usHoldingRegisterStartAddress);
    //UnitId(1 byte) + function code(1 byte) + Byte Count(1 byte) + (2 * Number of Data)
    usPduLength    = 3 + (usNumberOfData * 2);

    //Copy MBAP Header and function code into respone
    memcpy(pucResponse, pucQuery, (MBAP_HEADER_LENGTH + 1));

    //Modify Information in MBAP Header for response
    pucResponse[MBAP_LENGTH_OFFSET]     = (uint16_t)(usPduLength << 8);
    pucResponse[MBAP_LENGTH_OFFSET + 1] = (uint16_t)(usPduLength & 0xFF);
    pucResponse[MBT_BYTE_COUNT_OFFSET]  = (uint8_t)(usNumberOfData * 2);
    pucRegBuffer                        = &pucResponse[MBT_DATA_VALUES_OFFSET];

    while (usNumberOfData > 0)
    {
        *pucRegBuffer++ = (uint8_t)(m_ModbusData->psHoldingRegisters[usStartAddress] >> 8);
        *pucRegBuffer++ = (uint8_t)(m_ModbusData->psHoldingRegisters[usStartAddress] & 0xFF);
        usStartAddress++;
        usNumberOfData--;
    }

    //MBAP Header + function code(1 byte) + Byte Count(1 byte) + data length
    usResponseLength = MBAP_HEADER_LENGTH + 2 + pucResponse[MBT_BYTE_COUNT_OFFSET];

    return (usResponseLength);
}//end ReadHoldingRegisters

//
//! @brief Read Input Registers from Modbus data
//! @param[in]   pucQuery   Pointer  to modbus query buffer
//! @param[out]  pucResponse Pointer to modbus response buffer
//! @return      uint16_t    Response Length
//
static uint16_t ReadInputRegisters(const uint8_t *pucQuery, uint8_t *pucResponse)
{
    uint16_t usDataStartAddress = 0;
    uint16_t usNumberOfData     = 0;
    uint16_t usPduLength        = 0;
    uint16_t usStartAddress     = 0;
    uint16_t usResponseLength   = 0;
    uint8_t  *pucRegBuffer      = NULL;

    usDataStartAddress  = (uint16_t)(pucQuery[MBT_DATA_START_ADDRESS_OFFSET] << 8);
    usDataStartAddress |= (uint16_t)(pucQuery[MBT_DATA_START_ADDRESS_OFFSET + 1]);
    usNumberOfData      = (uint16_t)(pucQuery[MBT_NO_OF_DATA_OFFSET] << 8);
    usNumberOfData     |= (uint16_t)(pucQuery[MBT_NO_OF_DATA_OFFSET + 1]);

    usStartAddress = (usDataStartAddress - m_ModbusData->usInputRegisterStartAddress);
    //UnitId(1 byte) + function code(1 byte) + Byte Count(1 byte) + (2 * Number of Data)
    usPduLength    = 3 + (usNumberOfData * 2);

    //Copy MBAP Header and function code into respone
    memcpy(pucResponse, pucQuery, (MBAP_HEADER_LENGTH + 1));

    //Modify Information in MBAP Header for response
    pucResponse[MBAP_LENGTH_OFFSET]     = (uint16_t)(usPduLength << 8);
    pucResponse[MBAP_LENGTH_OFFSET + 1] = (uint16_t)(usPduLength & 0xFF);
    pucResponse[MBT_BYTE_COUNT_OFFSET]  = (uint8_t)(usNumberOfData * 2);
    pucRegBuffer                        = &pucResponse[MBT_DATA_VALUES_OFFSET];

    while (usNumberOfData > 0)
    {
        *pucRegBuffer++ = (uint8_t)(m_ModbusData->psInputRegisters[usStartAddress] >> 8);
        *pucRegBuffer++ = (uint8_t)(m_ModbusData->psInputRegisters[usStartAddress] & 0xFF);
        usStartAddress++;
        usNumberOfData--;
    }

    //MBAP Header + function code(1 byte_ + Byte Count(1 byte) + data length
    usResponseLength = MBAP_HEADER_LENGTH + 2 + pucResponse[MBT_BYTE_COUNT_OFFSET];

    return (usResponseLength);
}//end ReadInputRegisters

//
//! @brief Read Write Single Coil into Modbus data
//! @param[in]   pucQuery   Pointer  to modbus query buffer
//! @param[out]  pucResponse Pointer to modbus response buffer
//! @return      uint16_t    Response Length
//
static uint16_t WriteSingleCoil(const uint8_t *pucQuery, uint8_t *pucResponse)
{
    uint16_t usDataStartAddress = 0;
    uint16_t usNumberOfData     = 0;

    usDataStartAddress  = (uint16_t)(pucQuery[MBT_DATA_START_ADDRESS_OFFSET] << 8);
    usDataStartAddress |= (uint16_t)(pucQuery[MBT_DATA_START_ADDRESS_OFFSET + 1]);
    usNumberOfData      = (uint16_t)(pucQuery[MBT_NO_OF_DATA_OFFSET] << 8);
    usNumberOfData     |= (uint16_t)(pucQuery[MBT_NO_OF_DATA_OFFSET + 1]);

    return 0;
}//end WriteSingleCoil

//
//! @brief Read Write Single Holding Register into Modbus data
//! @param[in]   pucQuery   Pointer  to modbus query buffer
//! @param[out]  pucResponse Pointer to modbus response buffer
//! @return      uint16_t    Response Length
//
static uint16_t WriteSingleHoldingRegister(const uint8_t *pucQuery, uint8_t *pucResponse)
{
    uint16_t usDataStartAddress = 0;
    uint16_t usRegisterValue    = 0;
    uint16_t usPduLength        = 0;
    uint16_t usStartAddress     = 0;
    uint16_t usResponseLength   = 0;

    usDataStartAddress  = (uint16_t)(pucQuery[MBT_DATA_START_ADDRESS_OFFSET] << 8);
    usDataStartAddress |= (uint16_t)(pucQuery[MBT_DATA_START_ADDRESS_OFFSET + 1]);
    usRegisterValue     = (uint16_t)(pucQuery[MBT_NO_OF_DATA_OFFSET] << 8);
    usRegisterValue    |= (uint16_t)(pucQuery[MBT_NO_OF_DATA_OFFSET + 1]);

    usStartAddress = usDataStartAddress - m_ModbusData->usHoldingRegisterStartAddress;

    if ((m_ModbusData->psHoldingRegisterHigherLimit[usStartAddress] >= (int16_t) usRegisterValue) &&
        (m_ModbusData->psHoldingRegisterLowerLimit[usStartAddress]<= (int16_t) usRegisterValue))
    {
        m_ModbusData->psHoldingRegisters[usStartAddress] = usRegisterValue;
        //Copy same data in response as received in query
        //MBAP Header + function code(1 byte) + start address( 2 byte) +
        //Register value(2 byte)
        memcpy(pucResponse, pucQuery, (MBAP_HEADER_LENGTH + 5));
        usResponseLength = MBAP_HEADER_LENGTH + 5;
    }
    else
    {
        //UnitId(1 byte) + Error Code(1 byte) + Exception Code(1 byte)
        usPduLength = 3;

        pucResponse[MBAP_LENGTH_OFFSET]       = (uint16_t)(usPduLength << 8);
        pucResponse[MBAP_LENGTH_OFFSET + 1]   = (uint16_t)(usPduLength & 0xFF);
        pucResponse[MBT_FUNCTION_CODE_OFFSET] = 0x80 + pucQuery[MBT_FUNCTION_CODE_OFFSET];
        pucResponse[8]                        = ILLEGAL_DATA_VALUE;
        usResponseLength                      = MBT_EXCEPTION_PACKET_LENGTH;
    }

    return usResponseLength;
}//end WriteSingleHoldingRegister

//
//! @brief Read Write Multiple Coils into Modbus data
//! @param[in]   pucQuery   Pointer  to modbus query buffer
//! @param[out]  pucResponse Pointer to modbus response buffer
//! @return      uint16_t    Response Length
//
static uint16_t WriteMultipleCoils(const uint8_t *pucQuery, uint8_t *pucResponse)
{
    uint16_t usDataStartAddress = 0;
    uint16_t usNumberOfData     = 0;

    usDataStartAddress  = (uint16_t)(pucQuery[MBT_DATA_START_ADDRESS_OFFSET] << 8);
    usDataStartAddress |= (uint16_t)(pucQuery[MBT_DATA_START_ADDRESS_OFFSET + 1]);
    usNumberOfData      = (uint16_t)(pucQuery[MBT_NO_OF_DATA_OFFSET] << 8);
    usNumberOfData     |= (uint16_t)(pucQuery[MBT_NO_OF_DATA_OFFSET + 1]);

    return 0;
}//end WriteMultipleCoils

//
//! @brief Read Write Multiple Holding Registers into Modbus data
//! @param[in]   pucQuery   Pointer  to modbus query buffer
//! @param[out]  pucResponse Pointer to modbus response buffer
//! @return      uint16_t    Response Length
//
static uint16_t WriteMultipleHoldingRegisters(const uint8_t *pucQuery, uint8_t *pucResponse)
{
    uint16_t usDataStartAddress = 0;
    uint16_t usNumberOfData     = 0;

    usDataStartAddress  = (uint16_t)(pucQuery[MBT_DATA_START_ADDRESS_OFFSET] << 8);
    usDataStartAddress |= (uint16_t)(pucQuery[MBT_DATA_START_ADDRESS_OFFSET + 1]);
    usNumberOfData      = (uint16_t)(pucQuery[MBT_NO_OF_DATA_OFFSET] << 8);
    usNumberOfData     |= (uint16_t)(pucQuery[MBT_NO_OF_DATA_OFFSET + 1]);

    return 0;
}//end WriteMultipleHoldingRegisters

/******************************************************************************
 *                             End of file
 ******************************************************************************/
/** @}*/
