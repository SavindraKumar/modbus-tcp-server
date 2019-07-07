//! @addtogroup ModbusTCPUserFunction
//! @{
//
//****************************************************************************
//! @file mbap_user.h
//! @brief This contains the prototypes, macros, constants or global variables
//!        for the user configuration of mbap
//! @author Savindra Kumar(savindran1989@gmail.com)
//! @bug No known bugs.
//
//****************************************************************************
#ifndef MBAP_USER_H
#define MBAP_USER_H

//****************************************************************************
//                           Includes
//****************************************************************************


//****************************************************************************
//                           Constants and typedefs
//****************************************************************************
#define INPUT_REGISTER_START_ADDRESS      (0u)
#define MAX_INPUT_REGISTERS               (15u)
#define HOLDING_REGISTER_START_ADDRESS    (0u)
#define MAX_HOLDING_REGISTERS             (15u)
#define DISCRETE_INPUTS_START_ADDRESS     (0u)
#define MAX_DISCRETE_INPUTS               (15u)
#define COILS_START_ADDRESS               (0u)
#define MAX_COILS                         (15u)
#define DISCRETE_INPUT_BUF_SIZE           (MAX_DISCRETE_INPUTS / 8u + 1u)
#define COILS_BUF_SIZE                    (MAX_COILS / 8u + 1u)

//****************************************************************************
//                           Global variables
//****************************************************************************
extern int16_t g_sInputRegsBuf[MAX_INPUT_REGISTERS];
extern int16_t g_sHoldingRegsBuf[MAX_HOLDING_REGISTERS];
extern uint8_t g_ucDiscreteInputsBuf[DISCRETE_INPUT_BUF_SIZE];
extern uint8_t g_ucCoilsBuf[COILS_BUF_SIZE];
extern int16_t g_sHoldingRegsLowerLimitBuf[MAX_HOLDING_REGISTERS];
extern int16_t g_sHoldingRegsHigherLimitBuf[MAX_HOLDING_REGISTERS];

//****************************************************************************
//                           Global Functions
//****************************************************************************
//
//! @brief Initialize modbus data
//! @param[in]   None
//! @param[out]  None
//! @return      None
//
void mu_Init(void);

#endif // MBAP_USER_H
//****************************************************************************
//                             End of file
//****************************************************************************
//! @}
