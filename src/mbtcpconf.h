//! @addtogroup ModbusTCPConfiguration
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
#ifndef _MBTCPCONF_H
#define _MBTCPCONF_H

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
    uint16_t usNumOfInputRegisters;         //!< Number of Input Registers
    uint16_t usNumOfHoldingRegisters;       //!< Number of Holding Registers
    uint16_t usNumOfCoils;                  //!< Number of Coils
    uint16_t usNumDiscreteInputs;           //!< Number of Discrete Inputs
} ModbusData_t;

//!Enable or Disable Read Coils  Function Code
#define MBT_CONF_FC_READ_COILS_ENABLE               0

//!Enable or Disable Read Discrete Inputs Function Code
#define MBT_CONF_FC_READ_DISCRETE_INPUTS_ENABLE     0

//!Enable or Disable Read Holding Registers Function Code
#define MBT_CONF_FC_READ_HOLDING_REGISTERS_ENABLE   1

//!Enable or Disable Read Input Registers Function Code
#define MBT_CONF_FC_READ_INPUT_REGISTERS_ENABLE     1

//!Enable or Disable Write Single Coil Function Code
#define MBT_CONF_FC_WRITE_COIL_ENABLE               0

//!Enable or Disable Write Single Holding Register Function Code
#define MBT_CONF_FC_WRITE_HOLDING_REGISTER_ENABLE   1

//!Enable or Disable Write Multiple Coil Function Code
#define MBT_CONF_FC_WRITE_COILS_ENABLE              0

//!Enable or Disable Write Single Holding Registers Function Code
#define MBT_CONF_FC_WRITE_HOLDING_REGISTERS_ENABLE  0

//****************************************************************************
//                           Global variables
//****************************************************************************

//****************************************************************************
//                           Global Functions
//****************************************************************************
void mbtcp_DataInit (const ModbusData_t *ModbusData);
uint16_t mbtcp_ProcessRequest (const uint8_t *pucQuery, uint8_t ucQueryLength, uint8_t *pucResponse);


#endif // _MBTCPCONF_H
//****************************************************************************
//                             End of file
//****************************************************************************
//! @}
