//! @addtogroup ModbusTCPProtocolConfiguration
//! @brief Configuration for modbus application
//! @{
//
//****************************************************************************
//! @file mbap_conf.h
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
typedef void(*pfnReadDiscreteInputs)(uint16_t usStartAddress,
                                     int16_t usNumOfData,
                                     uint8_t *pucRecBuf);

typedef void(*pfnReadCoils)(uint16_t usStartAddress,
                            int16_t usNumOfData,
                            uint8_t *pucRecBuf);

typedef void(*pfnReadInputRegisters)(uint16_t usStartAddress,
                                     uint16_t usNumOfData,
                                     uint8_t *pucRecBuf);

typedef void(*pfnReadHoldingRegisters)(uint16_t usStartAddress,
                                       uint16_t usNumOfData,
                                       uint8_t *pucRecBuf);

typedef void(*pfnWriteSingleHoldingRegister)(uint16_t usStartAddress,
                                             uint16_t usRegisterValue);
typedef struct ModbusData
{
    int16_t                       *psHoldingRegisterLowerLimit;  //!<Pointer to Holding Register Lower Limits
    int16_t                       *psHoldingRegisterHigherLimit; //!<Pointer to Holding Register Lower Limits
    uint16_t                      usInputRegisterStartAddress;   //!<Input Register Start Address
    uint16_t                      usHoldingRegisterStartAddress; //!<Holding Register Start Address
    uint16_t                      usCoilsStartAddress;           //!<Coil Start Address
    uint16_t                      usDiscreteInputStartAddress;   //!<Discrete Input Start Address
    uint16_t                      usMaxInputRegisters;           //!<Number of Input Registers
    uint16_t                      usMaxHoldingRegisters;         //!<Number of Holding Registers
    uint16_t                      usMaxCoils;                    //!<Number of Coils
    uint16_t                      usMaxDiscreteInputs;           //!<Number of Discrete Inputs
    pfnReadInputRegisters         ptfnReadInputRegisters;        //!<Read Input Registers function
    pfnReadHoldingRegisters       ptfnReadHoldingRegisters;      //!<Read Holding Registers function
    pfnReadDiscreteInputs         ptfnReadDiscreteInputs;        //!<Read Discrete Inputs function
    pfnReadCoils                  ptfnReadCoils;                 //!<Read Coils function
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
#define MBT_CONF_FC_WRITE_COIL_ENABLE               0

//! @brief Enable or Disable Write Single Holding Register Function Code
#define MBT_CONF_FC_WRITE_HOLDING_REGISTER_ENABLE   0

//! @brief Enable or Disable Write Multiple Coil Function Code
#define MBT_CONF_FC_WRITE_COILS_ENABLE              0

//! @brief Enable or Disable Write Single Holding Registers Function Code
#define MBT_CONF_FC_WRITE_HOLDING_REGISTERS_ENABLE  0

//****************************************************************************
//                           Global variables
//****************************************************************************

//****************************************************************************
//                           Global Functions
//****************************************************************************
//
//! @brief Intialise Modbus Data
//! @param[in]  ModbusData  Modbus data structure
//! @return     None
//
void mbap_DataInit(ModbusData_t ModbusData);

//
//! @brief Process Modbus TCP Application request
//! @param[in]   pucQuery      Pointer to Modbus TCP Query buffer
//! @param[in]   ucQueryLen    Modbus TCP Query Length
//! @param[out]  pucResponse   Pointer to Modbus TCP Response buffer
//! @return      uint16_t      Modbus TCP Response Length
//
uint16_t mbap_ProcessRequest(const uint8_t *pucQuery, uint8_t ucQueryLen, uint8_t *pucResponse);


#endif // MBAP_CONF_H
//****************************************************************************
//                             End of file
//****************************************************************************
//! @}
