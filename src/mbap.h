//! @addtogroup ModbusTCPProtocol
//! @{
//
//****************************************************************************
//! @file mbap.h
//! @brief This contains the prototypes, macros, constants or global variables
//!        for the ModbusTCP Application
//! @author Savindra Kumar(savindran1989@gmail.com)
//! @bug No known bugs.
//
//****************************************************************************
#ifndef MBAP_H
#define MBAP_H

//****************************************************************************
//                           Includes
//****************************************************************************

//****************************************************************************
//                           Constants and typedefs
//****************************************************************************
//!Modbus Function Codes
enum FunctionCode
{
    eFC_READ_COILS              = 1,  //!< Read Coils Function Code
    eFC_READ_DISCRETE_INPUTS    = 2,  //!< Read Discrete Inputs Function Code
    eFC_READ_HOLDING_REGISTERS  = 3,  //!< Read Holding Registers Function Code
    eFC_READ_INPUT_REGISTERS    = 4,  //!< Read Read Input Registers Function Code
    eFC_WRITE_COIL              = 5,  //!< Write Single Coil Function Code
    eFC_WRITE_HOLDING_REGISTER  = 6,  //!< Write Single Holding Register Function Code
    eFC_WRITE_COILS             = 15, //!< Write Multiple Coils Function Code
    eFC_WRITE_HOLDING_REGISTERS = 16  //!< Write Multiple Holding Registers Function Code
};

//!Modbus Exception
enum Exception
{
    eNO_EXCEPTION          = 0,     //!< No Exception
    eILLEGAL_FUNCTION_CODE = 1,     //!< Illegal Function Code
    eILLEGAL_DATA_ADDRESS  = 2,     //!< Illegal Data Address
    eILLEGAL_DATA_VALUE    = 3      //!< Illegal Data Value
};

//! @brief  Read Coils Function Code enable or not
#ifdef MBT_CONF_FC_READ_COILS_ENABLE
#define FC_READ_COILS_ENABLE    MBT_CONF_FC_READ_COILS_ENABLE
#else // MBT_CONF_FC_READ_COILS_ENABLE
#define FC_READ_COILS_ENABLE    0
#endif // MBT_CONF_FC_READ_COILS_ENABLE

//! @brief  Read Discrete Inputs Function Code enable or not
#ifdef MBT_CONF_FC_READ_DISCRETE_INPUTS_ENABLE
#define FC_READ_DISCRETE_INPUTS_ENABLE  MBT_CONF_FC_READ_DISCRETE_INPUTS_ENABLE
#else // MBT_CONF_FC_READ_DISCRETE_INPUTS_ENABLE
#define FC_READ_DISCRETE_INPUTS_ENABLE  0
#endif // MBT_CONF_FC_READ_DISCRETE_INPUTS_ENABLE

//! @brief Read Holding Registers Function Code enable or not
#ifdef MBT_CONF_FC_READ_HOLDING_REGISTERS_ENABLE
#define FC_READ_HOLDING_REGISTERS_ENABLE    MBT_CONF_FC_READ_HOLDING_REGISTERS_ENABLE
#else // MBT_CONF_FC_READ_HOLDING_REGISTERS_ENABLE
#define FC_READ_HOLDING_REGISTERS_ENABLE    0
#endif // MBT_CONF_FC_READ_HOLDING_REGISTERS_ENABLE

//! @brief Read Input Registers Function Code enable or not
#ifdef MBT_CONF_FC_READ_INPUT_REGISTERS_ENABLE
#define FC_READ_INPUT_REGISTERS_ENABLE  MBT_CONF_FC_READ_INPUT_REGISTERS_ENABLE
#else // MBT_CONF_FC_READ_INPUT_REGISTERS_ENABLE
#define FC_READ_INPUT_REGISTERS_ENABLE  0
#endif // MBT_CONF_FC_READ_INPUT_REGISTERS_ENABLE

//! @brief Write Single Coil Function Code enable or not
#ifdef MBT_CONF_FC_WRITE_COIL_ENABLE
#define FC_WRITE_COIL_ENABLE    MBT_CONF_FC_WRITE_COIL_ENABLE
#else //MBT_CONF_FC_WRITE_COIL_ENABLE
#define FC_WRITE_COIL_ENABLE    0
#endif //MBT_CONF_FC_WRITE_COIL_ENABLE

//! @brief Write Single Holding Register Function Code enable or not
#ifdef MBT_CONF_FC_WRITE_HOLDING_REGISTER_ENABLE
#define FC_WRITE_HOLDING_REGISTER_ENABLE    MBT_CONF_FC_WRITE_HOLDING_REGISTER_ENABLE
#else // MBT_CONF_FC_WRITE_HOLDING_REGISTER_ENABLE
#define FC_WRITE_HOLDING_REGISTER_ENABLE    0
#endif // MBT_CONF_FC_WRITE_HOLDING_REGISTER_ENABLE

//! @brief Write Multiple Coils Function Code enable or not
#ifdef MBT_CONF_FC_WRITE_COILS_ENABLE
#define FC_WRITE_COILS_ENABLE   MBT_CONF_FC_WRITE_COILS_ENABLE
#else // MBT_CONF_FC_WRITE_COILS_ENABLE
#define FC_WRITE_COILS_ENABLE   0
#endif // MBT_CONF_FC_WRITE_COILS_ENABLE

//! @brief Write Multiple Holding Registers Function Code enable or not
#ifdef MBT_CONF_FC_WRITE_HOLDING_REGISTERS_ENABLE
#define FC_WRITE_HOLDING_REGISTERS_ENABLE   MBT_CONF_FC_WRITE_HOLDING_REGISTERS_ENABLE
#else // MBT_CONF_FC_WRITE_HOLDING_REGISTERS_ENABLE
#define FC_WRITE_HOLDING_REGISTERS_ENABLE   0
#endif // MBT_CONF_FC_WRITE_HOLDING_REGISTERS_ENABLE

//****************************************************************************
//                           Global variables
//****************************************************************************

//****************************************************************************
//                           Global Functions
//****************************************************************************

#endif // MBAP_H
//****************************************************************************
//                             End of file
//****************************************************************************
//! @}
