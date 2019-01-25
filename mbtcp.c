/** @addtogroup ModbusTCP
 *  @brief Modbus TCP Application
 *  @{
 */
/*****************************************************************************/
/** @file mbtcp.c
 *  @brief Modbus TCP Application source file
 *  @author Savindra Kumar(savindran1989@gmail.com)
 *  @bug No known bugs.
 */
/*****************************************************************************/
/******************************************************************************
 *                           Includes
 ******************************************************************************/
//standard header files
#include <stdint.h>
#include <stdbool.h>
//user defined header files
#include "mbtcp.h"

/******************************************************************************
 *                           Defines and typedefs
 ******************************************************************************/
#define MBT_PROTOCOL_ID                     0
#define DEVICE_ID                           1
//Modbus Application Protocol Header
#define MBAP_TRANSACTION_ID_OFFSET          0
#define MBAP_PROTOCOL_ID_OFFSET             2
#define MBAP_LENGTH_OFFSET                  4
#define MBAP_UNIT_ID_OFFSET                 6
//PDU Offset
#define MBT_FUNCTION_CODE_OFFSET            7
#define MBT_DATA_START_ADDRESS_OFFSET       8
#define MBT_NO_OF_DATA_OFFSET               10

#define MBT_MAX_PDU_LENGTH                  256
#define MBT_EXCEPTION_PACKET_LENGTH         9




/******************************************************************************
 *                           Private Functions
 ******************************************************************************/
static uint8_t ValidateFunctionCodeAndDataAddress (uint8_t *pucQuery);
static uint8_t HandleRequest (uint8_t *pucQuery, uint8_t *pucResponse, uint16_t *usResponseLength);

/******************************************************************************
 *                           external variables
 ******************************************************************************/

/******************************************************************************
 *                           Private variables
 ******************************************************************************/

/******************************************************************************
 *                    G L O B A L  F U N C T I O N S
 ******************************************************************************/
/** @brief Process Modbus TCP Application request
 *  @param[in]   pucQuery      Pointer to Modbus TCP Query buffer
 *  @param[in]   ucQueryLength Modbus TCP Query Length
 *  @param[out]  pucResponse   Pointer to Modbus TCP Response buffer
 *  @return      uint16_t      Modbus TCP Response Length
 */
uint16_t MBT_ProcessRequest(uint8_t *pucQuery, uint8_t ucQueryLength, uint8_t *pucResponse)
{
	uint16_t pusResponseLength = 0;
	uint16_t usProtocolId      = 0;
	uint16_t usPdulength       = 0;
	uint8_t ucUnitId           = 0;
	uint8_t ucException        = 0;

	//Modbus Application Protocol(MBAP) Header Information
	usProtocolId        = (uint16_t)(pucQuery[MBAP_PROTOCOL_ID_OFFSET] << 8);
	usProtocolId       |= (uint16_t)(pucQuery[MBAP_PROTOCOL_ID_OFFSET + 1]);
	usPdulength         = (uint16_t)(pucQuery[MBAP_LENGTH_OFFSET] << 8);
	usPdulength        |= (uint16_t)(pucQuery[MBAP_LENGTH_OFFSET + 1]);
	ucUnitId            = (uint8_t)(pucQuery[MBAP_UNIT_ID_OFFSET]);

	//check for Modbus TCP/IP protocol
	if (MBT_PROTOCOL_ID != usProtocolId)
	{
		pusResponseLength = 0;
	}

	//check if pdu length exceed
	if (usPdulength > MBT_MAX_PDU_LENGTH )
	{
		pusResponseLength = 0;
	}

	//check for Unit Id
	if (DEVICE_ID != ucUnitId)
	{
		pusResponseLength = 0;
	}

	//If Protocol Id, Pdu length or Unit Id validated sucessfully
	//Proceed for next validation steps
	if (0 != pusResponseLength)
	{
		ucException = ValidateFunctionCodeAndDataAddress(pucQuery);
		ucException = HandleRequest(pucQuery, pucResponse, &pusResponseLength);

		if (ucException)
		{
			pusResponseLength =  MBT_EXCEPTION_PACKET_LENGTH;
		}//end if
	}//end if

	return (pusResponseLength);
}//end MB_ProcessRequest

/******************************************************************************
 *                           L O C A L  F U N C T I O N S
 ******************************************************************************/
/** @brief Validate function code and data address in modbus query
 *  @param[in]  pucQuery Pointer to modbus query buffer
 *  @param[out] None
 *  @return     uint8_t 0 - NoException, nonzero - Exception
 */
static uint8_t ValidateFunctionCodeAndDataAddress(uint8_t *pucQuery)
{
	uint8_t ucFunctionCode      = 0;
	uint16_t usDataStartAddress = 0;
	uint16_t usNumberOfData     = 0;
	uint8_t ucException         = 0;

	//Modbus PDU Information
	ucFunctionCode      = (uint8_t)pucQuery[MBT_FUNCTION_CODE_OFFSET];
	usDataStartAddress  = (uint16_t)(pucQuery[MBT_DATA_START_ADDRESS_OFFSET] << 8);
	usDataStartAddress |= (uint16_t)(pucQuery[MBT_DATA_START_ADDRESS_OFFSET+1]);
	usNumberOfData      = (uint16_t)(pucQuery[MBT_NO_OF_DATA_OFFSET] << 8);
	usNumberOfData     |= (uint16_t)(pucQuery[MBT_NO_OF_DATA_OFFSET+1]);

	switch (ucFunctionCode)
	{
#ifdef FC_READ_COILS_ENABLE
	case FC_READ_COILS:
		if (!((usDataStartAddress >= 0) && ((usDataStartAddress + usNumberOfData) <= 0)))
		{
			ucException = ILLEGAL_DATA_ADDRESS;
		}
		break;
#endif

#ifdef FC_READ_DISCRETE_INPUTS_ENABLE
	case FC_READ_DISCRETE_INPUTS:
		if (!((usDataStartAddress >= 0) && ((usDataStartAddress + usNumberOfData) <= 0)))
		{
			ucException = ILLEGAL_DATA_ADDRESS;
		}
		break;
#endif

#ifdef FC_READ_HOLDING_REGISTERS_ENABLE
	case FC_READ_HOLDING_REGISTERS:
		if (!((usDataStartAddress >= 0) && ((usDataStartAddress + usNumberOfData) <= 0)))
		{
			ucException = ILLEGAL_DATA_ADDRESS;
		}
		break;
#endif

#ifdef FC_READ_INPUT_REGISTERS_ENABLE
	case FC_READ_INPUT_REGISTERS:
		if (!((usDataStartAddress >= 0) && ((usDataStartAddress + usNumberOfData) <= 0)))
		{
			ucException = ILLEGAL_DATA_ADDRESS;
		}
		break;
#endif

#ifdef FC_WRITE_COIL_ENABLE
	case FC_WRITE_COIL:
		if (!((usDataStartAddress >= 0) && ((usDataStartAddress + usNumberOfData) <= 0)))
		{
			ucException = ILLEGAL_DATA_ADDRESS;
		}
		break;
#endif

#ifdef FC_WRITE_HOLDING_REGISTER_ENABLE
	case FC_WRITE_HOLDING_REGISTER:
		if (!((usDataStartAddress >= 0) && ((usDataStartAddress + usNumberOfData) <= 0)))
		{
			ucException = ILLEGAL_DATA_ADDRESS;
		}
		break;
#endif

#ifdef FC_WRITE_COILS_ENABLE
	case FC_WRITE_COILS:
		if (!((usDataStartAddress >= 0) && ((usDataStartAddress + usNumberOfData) <= 0)))
		{
			ucException = ILLEGAL_DATA_ADDRESS;
		}
		break;
#endif

#ifdef FC_WRITE_HOLDING_REGISTERS_ENABLE
	case FC_WRITE_HOLDING_REGISTERS:
		if (!((usDataStartAddress >= 0) && ((usDataStartAddress + usNumberOfData) <= 0)))
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

/** @brief Handle Modbus Request after function code data adddress validated successfully
 *  @param[in]    pucQuery         Pointer to modbus query buffer
 *  @param[out]   pucResponse      Pointer to modbus response buffer
 *  @param[out]   ucResponseLength Pointer to modbus response length
 *  @return       uint8_t          0 - No Exception, nonzero - Exception in Request
 */
static uint8_t HandleRequest (uint8_t *pucQuery, uint8_t *pucResponse, uint16_t *usResponseLength)
{
	return 0;

}//end HandleRequest

/******************************************************************************
 *                             End of file
 ******************************************************************************/
/** @}*/
