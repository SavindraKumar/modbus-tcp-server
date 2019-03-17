/** @addtogroup ModbusTCP
 *  @{
 */
/*****************************************************************************/
/** @file mbtcp.h
 *  @brief This contains the prototypes, macros, constants or global variables
 *  for the ModbusTCP Application
 *  @author Savindra Kumar(savindran1989@gmail.com)
 *  @bug No known bugs.
 */
/*****************************************************************************/
#ifndef _MBTCP_H
#define _MBTCP_H

/******************************************************************************
 *                           Includes
 *****************************************************************************/

/******************************************************************************
 *                           Constants and typedefs
 *****************************************************************************/
//!Modbus Function Codes
enum FunctionCode
{
    FC_READ_COILS              = 1,  //!< Read Coils Function Code
    FC_READ_DISCRETE_INPUTS    = 2,  //!< Read Discrete Inputs Function Code
    FC_READ_HOLDING_REGISTERS  = 3,  //!< Read Holding Registers Function Code
    FC_READ_INPUT_REGISTERS    = 4,  //!< Read Read Input Registers Function Code
    FC_WRITE_COIL              = 5,  //!< Write Single Coil Function Code
    FC_WRITE_HOLDING_REGISTER  = 6,  //!< Write Single Holding Register Function Code
    FC_WRITE_COILS             = 15, //!< Write Multiple Coils Function Code
    FC_WRITE_HOLDING_REGISTERS = 16  //!< Write Multiple Holding Registers Function Code
};

//!Modbus Exception
enum Exception
{
    NO_EXCEPTION          = 0,     //!< No Exception
    ILLEGAL_FUNCTION_CODE = 1,     //!< Illegal Function Code
    ILLEGAL_DATA_ADDRESS  = 2,     //!< Illegal Data Address
    ILLEGAL_DATA_VALUE    = 3      //!< Illegal Data Value
};

//Whether Read Coils Function Code enable or not
#ifdef MBTP_CONF_FC_READ_COILS_ENABLE
#define FC_READ_COILS_ENABLE	MBTP_CONF_FC_READ_COILS_ENABLE
#else // MBTP_CONF_FC_READ_COILS_ENABLE
#define FC_READ_COILS_ENABLE	0
#endif // MBTP_CONF_FC_READ_COILS_ENABLE

//Whether Read Discrete Inputs Function Code enable or not
#ifdef MBTP_CONF_FC_READ_DISCRETE_INPUTS_ENABLE
#define FC_READ_DISCRETE_INPUTS_ENABLE	MBTP_CONF_FC_READ_DISCRETE_INPUTS_ENABLE
#else // MBTP_CONF_FC_READ_DISCRETE_INPUTS_ENABLE
#define FC_READ_DISCRETE_INPUTS_ENABLE	0
#endif // MBTP_CONF_FC_READ_DISCRETE_INPUTS_ENABLE

//Whether Read Holding Registers Function Code enable or not
#ifdef MBTP_CONF_FC_READ_HOLDING_REGISTERS_ENABLE
#define FC_READ_HOLDING_REGISTERS_ENABLE	MBTP_CONF_FC_READ_HOLDING_REGISTERS_ENABLE
#else // MBTP_CONF_FC_READ_HOLDING_REGISTERS_ENABLE
#define FC_READ_HOLDING_REGISTERS_ENABLE	0
#endif // MBTP_CONF_FC_READ_HOLDING_REGISTERS_ENABLE

//Whether Read Input Registers Function Code enable or not
#ifdef MBTP_CONF_FC_READ_INPUT_REGISTERS_ENABLE
#define FC_READ_INPUT_REGISTERS_ENABLE	MBTP_CONF_FC_READ_INPUT_REGISTERS_ENABLE
#else // MBTP_CONF_FC_READ_INPUT_REGISTERS_ENABLE
#define FC_READ_INPUT_REGISTERS_ENABLE	0
#endif // MBTP_CONF_FC_READ_INPUT_REGISTERS_ENABLE

//Whether Write Single Coil Function Code enable or not
#ifdef MBTP_CONF_FC_WRITE_COIL_ENABLE
#define FC_WRITE_COIL_ENABLE	MBTP_CONF_FC_WRITE_COIL_ENABLE
#else //MBTP_CONF_FC_WRITE_COIL_ENABLE
#define FC_WRITE_COIL_ENABLE	0
#endif //MBTP_CONF_FC_WRITE_COIL_ENABLE

//Whether Write Single Holding Register Function Code enable or not
#ifdef MBTP_CONF_FC_WRITE_HOLDING_REGISTER_ENABLE
#define FC_WRITE_HOLDING_REGISTER_ENABLE	MBTP_CONF_FC_WRITE_HOLDING_REGISTER_ENABLE
#else // MBTP_CONF_FC_WRITE_HOLDING_REGISTER_ENABLE
#define FC_WRITE_HOLDING_REGISTER_ENABLE	0
#endif // MBTP_CONF_FC_WRITE_HOLDING_REGISTER_ENABLE

//Whether Write Multiple Coils Function Code enable or not
#ifdef MBTP_CONF_FC_WRITE_COILS_ENABLE
#define FC_WRITE_COILS_ENABLE	MBTP_CONF_FC_WRITE_COILS_ENABLE
#else // MBTP_CONF_FC_WRITE_COILS_ENABLE
#define FC_WRITE_COILS_ENABLE	0
#endif // MBTP_CONF_FC_WRITE_COILS_ENABLE

//Whether Write Multiple Holding Registers Function Code enable or not
#ifdef MBTP_CONF_FC_WRITE_HOLDING_REGISTERS_ENABLE
#define FC_WRITE_HOLDING_REGISTERS_ENABLE	MBTP_CONF_FC_WRITE_HOLDING_REGISTERS_ENABLE
#else // MBTP_CONF_FC_WRITE_HOLDING_REGISTERS_ENABLE
#define FC_WRITE_HOLDING_REGISTERS_ENABLE	0
#endif // MBTP_CONF_FC_WRITE_HOLDING_REGISTERS_ENABLE

/******************************************************************************
 *                           Global variables
 *****************************************************************************/

/******************************************************************************
 *                           Global Functions
 *****************************************************************************/

#endif // _MBTCP_H
/******************************************************************************
 *                             End of file
 *****************************************************************************/
/** @}*/
