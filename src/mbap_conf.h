//! @addtogroup ModbusTCPProtocolConfiguration
//! @brief Configuration for modbus application
//! @{
//
//****************************************************************************
//! @file mbtcpconf.h
//! @brief This contains the prototypes, macros, constants or global variables
//!        for the ModbusTCP Application Configuration
//! @author Savindra Kumar(savindran1989@gmail.com)
//! @bug No known bugs.
//
//****************************************************************************
#ifndef MBAP_CONF_H
#define MBAP_CONF_H

//****************************************************************************
//                           Includes
//****************************************************************************

//****************************************************************************
//                           Constants and typedefs
//****************************************************************************
typedef struct
{
    int16_t *psInputRegisters;              //!< Pointer to Modbus Input Registers data
    int16_t *psHoldingRegisters;            //!< Pointer to Modbus Holding Registers data
    uint8_t *pucCoils;                      //!< Pointer to Modbus Coils Data
    uint8_t *pucDiscreteInputs;             //!< Pointer to Modbus Discrete Inputs Data
    int16_t *psHoldingRegisterLowerLimit;   //!< Pointer to Holding Register Lower Limits
    int16_t *psHoldingRegisterHigherLimit;  //!< Pointer to Holding Register Lower Limits
    uint16_t usInputRegisterStartAddress;   //!< Input Register Start Address
    uint16_t usHoldingRegisterStartAddress; //!< Holding Register Start Address
    uint16_t usCoilsStartAddress;           //!< Coil Start Address
    uint16_t usDiscreteInputStartAddress;   //!< Discrete Input Start Address
    uint16_t usMaxInputRegisters;           //!< Number of Input Registers
    uint16_t usMaxHoldingRegisters;         //!< Number of Holding Registers
    uint16_t usMaxCoils;                    //!< Number of Coils
    uint16_t usMaxDiscreteInputs;           //!< Number of Discrete Inputs
} ModbusData_t;

//! @brief Enable or Disable Read Coils  Function Code
#define MBT_CONF_FC_READ_COILS_ENABLE               1

//! @brief Enable or Disable Read Discrete Inputs Function Code
#define MBT_CONF_FC_READ_DISCRETE_INPUTS_ENABLE     1

//! @brief Enable or Disable Read Holding Registers Function Code
#define MBT_CONF_FC_READ_HOLDING_REGISTERS_ENABLE   1

//! @brief Enable or Disable Read Input Registers Function Code
#define MBT_CONF_FC_READ_INPUT_REGISTERS_ENABLE     1

//! @brief Enable or Disable Write Single Coil Function Code
#define MBT_CONF_FC_WRITE_COIL_ENABLE               1

//! @brief Enable or Disable Write Single Holding Register Function Code
#define MBT_CONF_FC_WRITE_HOLDING_REGISTER_ENABLE   1

//! @brief Enable or Disable Write Multiple Coil Function Code
#define MBT_CONF_FC_WRITE_COILS_ENABLE              0

//! @brief Enable or Disable Write Single Holding Registers Function Code
#define MBT_CONF_FC_WRITE_HOLDING_REGISTERS_ENABLE  0

//! @brief Enable or Disable Write Single Holding Registers Function Code
#define MBT_CONF_DEBUG_ENABLE                       1

#define MBT_CONF_DEBUG_LEVEL_WARNING                1
#define MBT_CONF_DEBUG_LEVEL_MSG                    1

#if MBT_CONF_DEBUG_ENABLE
#include <stdio.h>
#define MBT_DEBUG(debug_level, message)         if (debug_level) \
                                                { \
                                                    printf(message); \
                                                } \

#else
#define MBT_DEBUG(debug_level, message)
#endif
//****************************************************************************
//                           Global variables
//****************************************************************************

//****************************************************************************
//                           Global Functions
//****************************************************************************
void mbap_DataInit (const ModbusData_t *ModbusData);
uint16_t mbap_ProcessRequest (const uint8_t *pucQuery, uint8_t ucQueryLen, uint8_t *pucResponse);


#endif // MBAP_CONF_H
//****************************************************************************
//                             End of file
//****************************************************************************
//! @}
