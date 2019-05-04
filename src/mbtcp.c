//! @addtogroup ModbusTCP
//! @brief Modbus TCP application
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
#define MBT_PROTOCOL_ID                             (0u)
#define DEVICE_ID                                   (1u)

//Modbus application protocol header
#define MBAP_HEADER_LEN                             (7u)
#define MBAP_TRANSACTION_ID_OFFSET                  (0u)
#define MBAP_PROTOCOL_ID_OFFSET                     (2u)
#define MBAP_LEN_OFFSET                             (4u)
#define MBAP_UNIT_ID_OFFSET                         (6u)
//PDU offset in query for multiple read/write
#define FUNCTION_CODE_OFFSET                        (7u)
#define DATA_START_ADDRESS_OFFSET                   (8u)
#define NO_OF_DATA_OFFSET                           (10u)
//PDU offset in response
#define BYTE_COUNT_OFFSET                           (8u)
#define DATA_VALUES_OFFSET                          (9u)
//write single holding register
//function code(1 byte) + start address( 2 bytes) + Register value(2 bytes) = 5 bytes
#define WRITE_SINGLE_REGISTER_RESPONSE_LEN          (MBAP_HEADER_LEN + 5u)
#define REGISTER_VALUE_OFFSET                       (10u)
//write single coil
//function code(1 byte) + start address( 2 bytes) + coil value(2 bytes) = 5 bytes
#define WRITE_SINGLE_COIL_RESPONSE_LEN              (MBAP_HEADER_LEN + 5u)
#define COIL_VALUE_OFFSSET                          (10u)
//Exception packet offset in response
#define EXCEPTION_FUNCTION_CODE_OFFSET              (7u)
#define EXCEPTION_TYPE_OFFSET                       (8u)
//Unit Id(1 byte) + Error Code(1 byte) + Exception Code(1 byte) = 2 bytes
#define MBAP_LEN_IN_EXCEPTION_PACKET                (3u)
//Error Code(1 byte) + Exception Code(1 byte) = 2 bytes
#define EXCEPTION_PACKET_LEN                        (MBAP_HEADER_LEN + 2u)
#define MAX_PDU_LEN                                 (256u)

#define MULTIPLE_OF_8                               (0x0007)

//UnitId(1 byte) + function code(1 byte) + Byte Count(1 byte) + (2 * Number of Data)
#define MBAP_LEN_READ_INPUT_REGISTERS(usNumOfData)       (3u + usNumOfData * 2u)
#define MBAP_LEN_READ_HOLDING_REGISTERS(usNumOfData)     (3u + usNumOfData * 2u)
#define MBAP_LEN_READ_DISCRETE_INPUTS(usNumOfData)       (3u + usNumOfData / 8 )
#define MBAP_LEN_READ_COILS(usNumOfData)                 (3u + usNumOfData / 8 )
//MBAP Header + function code(1 byte) + Byte Count(1 byte) + 2 * Number of Data
#define READ_INPUT_REGISTERS_RESPONSE_LEN(usNumOfData)   (MBAP_HEADER_LEN + 2u + usNumOfData * 2u)
#define READ_HOLDING_REGISTERS_RESPONSE_LEN(usNumOfData) (MBAP_HEADER_LEN + 2u + usNumOfData * 2u)
#define READ_DISCRETE_INPUTS_RESPONSE_LEN(usNumOfData)   (MBAP_HEADER_LEN + 2u + usNumOfData / 8)
#define READ_COILS_RESPONSE_LEN(usNumOfData)             (MBAP_HEADER_LEN + 2u + usNumOfData / 8)
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
void mbtcp_DataInit(const ModbusData_t *ModbusData)
{
    m_ModbusData = ModbusData;

}//end mbtcp_DataInit

//
//! @brief Process Modbus TCP Application request
//! @param[in]   pucQuery      Pointer to Modbus TCP Query buffer
//! @param[in]   ucQueryLen    Modbus TCP Query Length
//! @param[out]  pucResponse   Pointer to Modbus TCP Response buffer
//! @return      uint16_t      Modbus TCP Response Length
//
uint16_t mbtcp_ProcessRequest(const uint8_t *pucQuery, uint8_t ucQueryLen, uint8_t *pucResponse)
{
    uint16_t usResponseLen = 0;
    uint8_t  ucException   = 0;
    bool     bIsQueryOk    = false;

    bIsQueryOk = BasicValidation(pucQuery);

    //If Protocol Id, Pdu length or Unit Id validated sucessfully
    //Proceed for next validation steps
    if (bIsQueryOk)
    {
        ucException = ValidateFunctionCodeAndDataAddress(pucQuery);

        if (ucException)
        {
            usResponseLen = BuildExceptionPacket(pucQuery, ucException, pucResponse);
        }
        else
        {
            usResponseLen = HandleRequest(pucQuery, pucResponse);
        }
    }//end if

    return (usResponseLen);
}//end mbtcp_ProcessRequest

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
    uint16_t usMbapLen    = 0;
    uint8_t  ucUnitId     = 0;
    bool     bIsQueryOk   = true;

    //Modbus Application Protocol(MBAP) Header Information
    usProtocolId  = (uint16_t)(pucQuery[MBAP_PROTOCOL_ID_OFFSET] << 8);
    usProtocolId |= (uint16_t)(pucQuery[MBAP_PROTOCOL_ID_OFFSET + 1]);
    usMbapLen     = (uint16_t)(pucQuery[MBAP_LEN_OFFSET] << 8);
    usMbapLen    |= (uint16_t)(pucQuery[MBAP_LEN_OFFSET + 1]);
    ucUnitId      = (uint8_t)(pucQuery[MBAP_UNIT_ID_OFFSET]);

    //check for Modbus TCP/IP protocol
    if (MBT_PROTOCOL_ID != usProtocolId)
    {
        bIsQueryOk = false;
    }

    //check if pdu length exceed
    if (usMbapLen > MAX_PDU_LEN)
    {
        bIsQueryOk = false;
    }

    //check for Unit Id
    if (DEVICE_ID != ucUnitId)
    {
        bIsQueryOk = false;
    }

    return (bIsQueryOk);
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
    uint16_t usNumOfData        = 0;
    uint8_t  ucException        = 0;

    //Modbus PDU Information
    ucFunctionCode      = (uint8_t)pucQuery[FUNCTION_CODE_OFFSET];
    usDataStartAddress  = (uint16_t)(pucQuery[DATA_START_ADDRESS_OFFSET] << 8);
    usDataStartAddress |= (uint16_t)(pucQuery[DATA_START_ADDRESS_OFFSET + 1]);
    usNumOfData         = (uint16_t)(pucQuery[NO_OF_DATA_OFFSET] << 8);
    usNumOfData        |= (uint16_t)(pucQuery[NO_OF_DATA_OFFSET + 1]);

    switch (ucFunctionCode)
    {
#if FC_READ_COILS_ENABLE
    case FC_READ_COILS:
        if (!((usDataStartAddress >= m_ModbusData->usCoilsStartAddress) &&
             ((usDataStartAddress + usNumOfData) <= (m_ModbusData->usCoilsStartAddress + m_ModbusData->usMaxCoils))))
        {
            ucException = ILLEGAL_DATA_ADDRESS;
        }
        break;
#endif

#if FC_READ_DISCRETE_INPUTS_ENABLE
    case FC_READ_DISCRETE_INPUTS:
        if (!((usDataStartAddress >= m_ModbusData->usDiscreteInputStartAddress) &&
             ((usDataStartAddress + usNumOfData) <= (m_ModbusData->usDiscreteInputStartAddress + m_ModbusData->usMaxDiscreteInputs))))
        {
            ucException = ILLEGAL_DATA_ADDRESS;
        }
        break;
#endif

#if FC_READ_HOLDING_REGISTERS_ENABLE
    case FC_READ_HOLDING_REGISTERS:
        if (!((usDataStartAddress >= m_ModbusData->usHoldingRegisterStartAddress) &&
             ((usDataStartAddress + usNumOfData) <= (m_ModbusData->usHoldingRegisterStartAddress + m_ModbusData->usMaxHoldingRegisters))))
        {
            ucException = ILLEGAL_DATA_ADDRESS;
        }
        break;
#endif

#if FC_READ_INPUT_REGISTERS_ENABLE
    case FC_READ_INPUT_REGISTERS:
        if (!((usDataStartAddress >= m_ModbusData->usInputRegisterStartAddress) &&
             ((usDataStartAddress + usNumOfData) <= (m_ModbusData->usInputRegisterStartAddress + m_ModbusData->usMaxInputRegisters))))

        {
            ucException = ILLEGAL_DATA_ADDRESS;
        }
        break;
#endif

#if FC_WRITE_COIL_ENABLE
        case FC_WRITE_COIL:
            if (!((usDataStartAddress >= m_ModbusData->usCoilsStartAddress) &&
                 ((usDataStartAddress + usNumOfData) <= (m_ModbusData->usCoilsStartAddress + m_ModbusData->usNumOfCoils))))
            {
                ucException = ILLEGAL_DATA_ADDRESS;
            }
            break;
#endif

#if FC_WRITE_HOLDING_REGISTER_ENABLE
        case FC_WRITE_HOLDING_REGISTER:
            if (!((usDataStartAddress >= m_ModbusData->usHoldingRegisterStartAddress) &&
                 (usDataStartAddress <= (m_ModbusData->usHoldingRegisterStartAddress + m_ModbusData->usMaxHoldingRegisters))))
            {
                ucException = ILLEGAL_DATA_ADDRESS;
            }
            break;
#endif

#if FC_WRITE_COILS_ENABLE
        case FC_WRITE_COILS:
            if (!((usDataStartAddress >= m_ModbusData->usCoilsStartAddress) &&
                 ((usDataStartAddress + usNumOfData) <= (m_ModbusData->usCoilsStartAddress + m_ModbusData->usMaxCoils))))
            {
                ucException = ILLEGAL_DATA_ADDRESS;
            }
            break;
#endif

#if FC_WRITE_HOLDING_REGISTERS_ENABLE
        case FC_WRITE_HOLDING_REGISTERS:
            if (!((usDataStartAddress >= m_ModbusData->usHoldingRegisterStartAddress) &&
                 ((usDataStartAddress + usNumOfData) <= (m_ModbusData->usHoldingRegisterStartAddress + m_ModbusData->uMaxHoldingRegisters))))
            {
                ucException = ILLEGAL_DATA_ADDRESS;
            }
            break;
#endif

    default:
        ucException = ILLEGAL_FUNCTION_CODE;
        break;
    }//end switch

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
    uint8_t  ucFunctionCode = 0;
    uint16_t usResponseLen  = 0;

    // filter PDU information
    ucFunctionCode = (uint8_t) pucQuery[FUNCTION_CODE_OFFSET];

    switch (ucFunctionCode)
    {
    case FC_READ_COILS:
        usResponseLen = ReadCoils(pucQuery, pucResponse);
        break;
    case FC_READ_DISCRETE_INPUTS:
        usResponseLen = ReadDiscreteInputs(pucQuery, pucResponse);
        break;
    case FC_READ_HOLDING_REGISTERS:
        usResponseLen = ReadHoldingRegisters(pucQuery, pucResponse);
        break;
    case FC_READ_INPUT_REGISTERS:
        usResponseLen = ReadInputRegisters(pucQuery, pucResponse);
        break;
    case FC_WRITE_COIL:
        usResponseLen = WriteSingleCoil(pucQuery, pucResponse);
        break;
    case FC_WRITE_HOLDING_REGISTER:
        usResponseLen = WriteSingleHoldingRegister(pucQuery, pucResponse);
        break;
    case FC_WRITE_COILS:
        usResponseLen = WriteMultipleCoils(pucQuery, pucResponse);
        break;
    case FC_WRITE_HOLDING_REGISTERS:
        usResponseLen = WriteMultipleHoldingRegisters(pucQuery, pucResponse);
        break;
    default:
        usResponseLen = 0;
        break;
    }//end switch

    return (usResponseLen);
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
    memcpy(pucResponse, pucQuery, MBAP_HEADER_LEN);

    //Modify information for response
    pucResponse[MBAP_LEN_OFFSET]                = 0;
    pucResponse[MBAP_LEN_OFFSET + 1]            = MBAP_LEN_IN_EXCEPTION_PACKET;
    pucResponse[EXCEPTION_FUNCTION_CODE_OFFSET] = 0x80 + pucQuery[FUNCTION_CODE_OFFSET];
    pucResponse[EXCEPTION_TYPE_OFFSET]          = ucException;

    return (EXCEPTION_PACKET_LEN);
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
    int16_t  sNumOfData         = 0;
    uint16_t usStartAddress     = 0;
    uint16_t usMbapLen          = 0;
    uint16_t usResponseLen      = 0;
    uint8_t  *pucBuffer         = NULL;

    usDataStartAddress  = (uint16_t)(pucQuery[DATA_START_ADDRESS_OFFSET] << 8);
    usDataStartAddress |= (uint16_t)(pucQuery[DATA_START_ADDRESS_OFFSET + 1]);
    sNumOfData          = (uint16_t)(pucQuery[NO_OF_DATA_OFFSET] << 8);
    sNumOfData         |= (uint16_t)(pucQuery[NO_OF_DATA_OFFSET + 1]);

    usStartAddress = usDataStartAddress - m_ModbusData->usCoilsStartAddress;

    //Copy MBAP Header and function code into respone
    memcpy(pucResponse, pucQuery, (MBAP_HEADER_LEN + 1));

    usMbapLen = MBAP_LEN_READ_COILS(sNumOfData);

    if (0 != (sNumOfData & MULTIPLE_OF_8))
    {
        usMbapLen = usMbapLen + 1;
    }
    //Modify Information in MBAP Header for response
    pucResponse[MBAP_LEN_OFFSET]     = (uint16_t)(usMbapLen << 8);
    pucResponse[MBAP_LEN_OFFSET + 1] = (uint16_t)(usMbapLen & 0xFF);
    pucResponse[BYTE_COUNT_OFFSET]   = (uint8_t)(sNumOfData / 8);
    pucBuffer                        = &pucResponse[DATA_VALUES_OFFSET];

    usResponseLen = READ_COILS_RESPONSE_LEN(sNumOfData);

    if (0 != (sNumOfData & MULTIPLE_OF_8))
    {
        usResponseLen                  += 1;
        pucResponse[BYTE_COUNT_OFFSET] += 1;
    }

    while (sNumOfData > 0)
    {
        uint16_t  usTmpBuf;
        uint16_t  usMask;
        uint16_t  usByteOffset;
        uint16_t  usNPreBits;

        usByteOffset = usStartAddress  / 8 ;
        usNPreBits   = usStartAddress - usByteOffset * 8;
        usMask       = (1 << sNumOfData)  - 1 ;
        usTmpBuf     = m_ModbusData->pucCoils[usByteOffset];
        usTmpBuf    |= m_ModbusData->pucCoils[usByteOffset + 1] << 8;
        // throw away unneeded bits
        usTmpBuf     = usTmpBuf >> usNPreBits;
       // mask away bits above the requested bitfield
        usTmpBuf     = usTmpBuf & usMask;
        *pucBuffer++ = (uint8_t)usTmpBuf;

        sNumOfData         = sNumOfData - 8;
        usDataStartAddress = usDataStartAddress - 8;
    }

    return usResponseLen;
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
    int16_t  sNumOfData         = 0;
    uint16_t usStartAddress     = 0;
    uint16_t usMbapLen          = 0;
    uint16_t usResponseLen      = 0;
    uint8_t  *pucBuffer         = NULL;

    usDataStartAddress  = (uint16_t)(pucQuery[DATA_START_ADDRESS_OFFSET] << 8);
    usDataStartAddress |= (uint16_t)(pucQuery[DATA_START_ADDRESS_OFFSET + 1]);
    sNumOfData          = (uint16_t)(pucQuery[NO_OF_DATA_OFFSET] << 8);
    sNumOfData         |= (uint16_t)(pucQuery[NO_OF_DATA_OFFSET + 1]);

    usStartAddress = usDataStartAddress - m_ModbusData->usDiscreteInputStartAddress;

    //Copy MBAP Header and function code into respone
    memcpy(pucResponse, pucQuery, (MBAP_HEADER_LEN + 1));

    usMbapLen = MBAP_LEN_READ_DISCRETE_INPUTS(sNumOfData);

    if (0 != (sNumOfData & MULTIPLE_OF_8))
    {
        usMbapLen = usMbapLen + 1;
    }
    //Modify Information in MBAP Header for response
    pucResponse[MBAP_LEN_OFFSET]     = (uint16_t)(usMbapLen << 8);
    pucResponse[MBAP_LEN_OFFSET + 1] = (uint16_t)(usMbapLen & 0xFF);
    pucResponse[BYTE_COUNT_OFFSET]   = (uint8_t)(sNumOfData / 8);
    pucBuffer                        = &pucResponse[DATA_VALUES_OFFSET];

    usResponseLen = READ_DISCRETE_INPUTS_RESPONSE_LEN(sNumOfData);

    if (0 != (sNumOfData & MULTIPLE_OF_8))
    {
        usResponseLen                  += 1;
        pucResponse[BYTE_COUNT_OFFSET] += 1;
    }

    while (sNumOfData > 0)
    {
        uint16_t  usTmpBuf;
        uint16_t  usMask;
        uint16_t  usByteOffset;
        uint16_t  usNPreBits;

        usByteOffset = usStartAddress  / 8 ;
        usNPreBits   = usStartAddress - usByteOffset * 8;
        usMask       = (1 << sNumOfData)  - 1 ;
        usTmpBuf     = m_ModbusData->pucDiscreteInputs[usByteOffset];
        usTmpBuf    |= m_ModbusData->pucDiscreteInputs[usByteOffset + 1] << 8;
        // throw away unneeded bits
        usTmpBuf     = usTmpBuf >> usNPreBits;
       // mask away bits above the requested bitfield
        usTmpBuf     = usTmpBuf & usMask;
        *pucBuffer++ = (uint8_t)usTmpBuf;

        sNumOfData         = sNumOfData - 8;
        usDataStartAddress = usDataStartAddress - 8;
    }

    return usResponseLen;
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
    uint16_t usNumOfData        = 0;
    uint16_t usPduLength        = 0;
    uint16_t usStartAddress     = 0;
    uint16_t usResponseLen      = 0;
    uint8_t  *pucRegBuffer      = NULL;

    usDataStartAddress  = (uint16_t)(pucQuery[DATA_START_ADDRESS_OFFSET] << 8);
    usDataStartAddress |= (uint16_t)(pucQuery[DATA_START_ADDRESS_OFFSET + 1]);
    usNumOfData         = (uint16_t)(pucQuery[NO_OF_DATA_OFFSET] << 8);
    usNumOfData        |= (uint16_t)(pucQuery[NO_OF_DATA_OFFSET + 1]);

    usStartAddress = (usDataStartAddress - m_ModbusData->usHoldingRegisterStartAddress);
    usPduLength    = MBAP_LEN_READ_INPUT_REGISTERS(usNumOfData);

    //Copy MBAP Header and function code into respone
    memcpy(pucResponse, pucQuery, (MBAP_HEADER_LEN + 1));

    //Modify Information in MBAP Header for response
    pucResponse[MBAP_LEN_OFFSET]     = (uint16_t)(usPduLength << 8);
    pucResponse[MBAP_LEN_OFFSET + 1] = (uint16_t)(usPduLength & 0xFF);
    pucResponse[BYTE_COUNT_OFFSET]   = (uint8_t)(usNumOfData * 2);
    pucRegBuffer                     = &pucResponse[DATA_VALUES_OFFSET];

    usResponseLen = READ_HOLDING_REGISTERS_RESPONSE_LEN(usNumOfData);

    while (usNumOfData > 0)
    {
        *pucRegBuffer++ = (uint8_t)(m_ModbusData->psHoldingRegisters[usStartAddress] >> 8);
        *pucRegBuffer++ = (uint8_t)(m_ModbusData->psHoldingRegisters[usStartAddress] & 0xFF);
        usStartAddress++;
        usNumOfData--;
    }

    return (usResponseLen);
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
    uint16_t usNumOfData        = 0;
    uint16_t usMbapLength       = 0;
    uint16_t usStartAddress     = 0;
    uint16_t usResponseLen      = 0;
    uint8_t  *pucRegBuffer      = NULL;

    usDataStartAddress  = (uint16_t)(pucQuery[DATA_START_ADDRESS_OFFSET] << 8);
    usDataStartAddress |= (uint16_t)(pucQuery[DATA_START_ADDRESS_OFFSET + 1]);
    usNumOfData         = (uint16_t)(pucQuery[NO_OF_DATA_OFFSET] << 8);
    usNumOfData        |= (uint16_t)(pucQuery[NO_OF_DATA_OFFSET + 1]);

    usStartAddress = (usDataStartAddress - m_ModbusData->usInputRegisterStartAddress);
    usMbapLength   = MBAP_LEN_READ_INPUT_REGISTERS(usNumOfData);

    //Copy MBAP Header and function code into response
    memcpy(pucResponse, pucQuery, (MBAP_HEADER_LEN + 1));

    //Modify Information in MBAP Header for response
    pucResponse[MBAP_LEN_OFFSET]     = (uint16_t)(usMbapLength << 8);
    pucResponse[MBAP_LEN_OFFSET + 1] = (uint16_t)(usMbapLength & 0xFF);
    pucResponse[BYTE_COUNT_OFFSET]   = (uint8_t)(usNumOfData * 2);
    pucRegBuffer                     = &pucResponse[DATA_VALUES_OFFSET];

    usResponseLen = READ_INPUT_REGISTERS_RESPONSE_LEN(usNumOfData);

    while (usNumOfData > 0)
    {
        *pucRegBuffer++ = (uint8_t)(m_ModbusData->psInputRegisters[usStartAddress] >> 8);
        *pucRegBuffer++ = (uint8_t)(m_ModbusData->psInputRegisters[usStartAddress] & 0xFF);
        usStartAddress++;
        usNumOfData--;
    }

    return (usResponseLen);
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
    int16_t  sCoilValue         = 0;
    uint16_t usStartAddress     = 0;
    uint16_t usResponseLen      = 0;

    usDataStartAddress  = (uint16_t)(pucQuery[DATA_START_ADDRESS_OFFSET] << 8);
    usDataStartAddress |= (uint16_t)(pucQuery[DATA_START_ADDRESS_OFFSET + 1]);
    sCoilValue          = (uint16_t)(pucQuery[COIL_VALUE_OFFSSET] << 8);
    sCoilValue         |= (uint16_t)(pucQuery[COIL_VALUE_OFFSSET + 1]);

    usStartAddress = usDataStartAddress - m_ModbusData->usCoilsStartAddress;

    if (0xFF00 == sCoilValue)
    {
        //Turn on coil
        sCoilValue = 1;
    }
    else if (0x0000 == sCoilValue)
    {
        //Turn off coil
        sCoilValue = 1;
    }
    else
    {
        //discard
    }

    //Copy same data in response as received in query
    usResponseLen = WRITE_SINGLE_COIL_RESPONSE_LEN;
    memcpy(pucResponse, pucQuery, usResponseLen);

     uint16_t usByteOffset = usStartAddress  / 8 ;
     uint16_t usNPreBits   = usStartAddress - usByteOffset * 8;
     uint16_t usMask       = (1 << sCoilValue)  - 1 ;
     uint16_t usTmpBuf     = m_ModbusData->pucCoils[usByteOffset];
     usTmpBuf    |= m_ModbusData->pucCoils[usByteOffset + 1] << 8;
     // throw away unneeded bits
     usTmpBuf     = usTmpBuf >> usNPreBits;
     // mask away bits above the requested bitfield
     usTmpBuf     = usTmpBuf & usMask;
     *pucBuffer++ = (uint8_t)usTmpBuf;

    return usResponseLen;
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
    uint16_t usResponseLen      = 0;

    usDataStartAddress  = (uint16_t)(pucQuery[DATA_START_ADDRESS_OFFSET] << 8);
    usDataStartAddress |= (uint16_t)(pucQuery[DATA_START_ADDRESS_OFFSET + 1]);
    usRegisterValue     = (uint16_t)(pucQuery[REGISTER_VALUE_OFFSET] << 8);
    usRegisterValue    |= (uint16_t)(pucQuery[REGISTER_VALUE_OFFSET + 1]);

    usStartAddress = usDataStartAddress - m_ModbusData->usHoldingRegisterStartAddress;

    if ((m_ModbusData->psHoldingRegisterHigherLimit[usStartAddress] >= (int16_t) usRegisterValue) &&
        (m_ModbusData->psHoldingRegisterLowerLimit[usStartAddress] <= (int16_t) usRegisterValue))
    {
        m_ModbusData->psHoldingRegisters[usStartAddress] = usRegisterValue;
        //Copy same data in response as received in query
        usResponseLen = WRITE_SINGLE_REGISTER_RESPONSE_LEN;
        memcpy(pucResponse, pucQuery, usResponseLen);
    }
    else
    {
        usPduLength                                 = MBAP_LEN_IN_EXCEPTION_PACKET;
        pucResponse[MBAP_LEN_OFFSET]                = (uint16_t)(usPduLength << 8);
        pucResponse[MBAP_LEN_OFFSET + 1]            = (uint16_t)(usPduLength & 0xFF);
        pucResponse[EXCEPTION_FUNCTION_CODE_OFFSET] = 0x80 + pucQuery[FUNCTION_CODE_OFFSET];
        pucResponse[EXCEPTION_TYPE_OFFSET]          = ILLEGAL_DATA_VALUE;
        usResponseLen                               = EXCEPTION_PACKET_LEN;
    }

    return usResponseLen;
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
    uint16_t usNumOfData        = 0;

    usDataStartAddress  = (uint16_t)(pucQuery[DATA_START_ADDRESS_OFFSET] << 8);
    usDataStartAddress |= (uint16_t)(pucQuery[DATA_START_ADDRESS_OFFSET + 1]);
    usNumOfData         = (uint16_t)(pucQuery[NO_OF_DATA_OFFSET] << 8);
    usNumOfData        |= (uint16_t)(pucQuery[NO_OF_DATA_OFFSET + 1]);

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
    uint16_t usNumOfData        = 0;

    usDataStartAddress  = (uint16_t)(pucQuery[DATA_START_ADDRESS_OFFSET] << 8);
    usDataStartAddress |= (uint16_t)(pucQuery[DATA_START_ADDRESS_OFFSET + 1]);
    usNumOfData         = (uint16_t)(pucQuery[NO_OF_DATA_OFFSET] << 8);
    usNumOfData        |= (uint16_t)(pucQuery[NO_OF_DATA_OFFSET + 1]);

    return 0;
}//end WriteMultipleHoldingRegisters

/******************************************************************************
 *                             End of file
 ******************************************************************************/
/** @}*/
