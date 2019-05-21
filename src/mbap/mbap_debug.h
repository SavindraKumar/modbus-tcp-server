//! @addtogroup ModbusTCPProtocolConfiguration
//! @{
//
//****************************************************************************
//! @file mbap_debug.h
//! @brief This contains the prototypes, macros, constants or global variables
//!        for the debug configuration of mbap
//! @author Savindra Kumar(savindran1989@gmail.com)
//! @bug No known bugs.
//
//****************************************************************************
#ifndef MBAP_DEBUG_H
#define MBAP_DEBUG_H

//****************************************************************************
//                           Includes
//****************************************************************************


//****************************************************************************
//                           Constants and typedefs
//****************************************************************************
//NOTE: Debug mask is chosen based on warning and msg level debug
//      If warning leve and msg level debug changed mask should be changed
#define MBT_CONF_DEBUG_MASK                         0x06
#define MBT_DEBUG                                   1
#define MBT_CONF_DEBUG_WARNING_ENABLE               0
#define MBT_CONFIG_DEBUG_MSG_ENABLE                 0


#if MBT_CONF_DEBUG_WARNING_ENABLE
#define MBT_CONF_DEBUG_LEVEL_WARNING                0x02
#else //MBT_CONF_DEBUG_WARNING_ENABLE
#define MBT_CONF_DEBUG_LEVEL_WARNING                0x00
#endif //MBT_CONF_DEBUG_WARNING_ENABLE

#if MBT_CONFIG_DEBUG_MSG_ENABLE
#define MBT_CONF_DEBUG_LEVEL_MSG                    0x04
#else //MBT_CONF_DEBUG_DEBUG_ENABLE
#define MBT_CONF_DEBUG_LEVEL_MSG                    0x00
#endif //MBT_CONF_DEBUG_DEBUG_ENABLE


#ifdef MBT_DEBUG
//NOTE: standard I/O header file is chosen because of printf
//      If some other function is used for printing debug then
//      header file of that function should be used instead of stdio.h
#include <stdio.h>
// print debug message only if debug message type is enabled
#define MBT_DEBUGF(debug, message)    do { \
                                             if (debug & MBT_CONF_DEBUG_MASK)\
                                             {\
                                                 printf(message);\
                                             }\
                                        }while(0)

#else  //MBT_DEBUG
#define MBT_DEBUGF(debug, message)
#endif //MBT_DEBUG
//****************************************************************************
//                           Global variables
//****************************************************************************

//****************************************************************************
//                           Global Functions
//****************************************************************************

#endif // MBAP_DEBUG_H
//****************************************************************************
//                             End of file
//****************************************************************************
//! @}
