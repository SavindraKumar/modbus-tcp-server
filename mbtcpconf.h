/** @addtogroup ModbusTCPConfiguration
 *  @brief Configuration for modbus application
 *  @{
 */
/*****************************************************************************/
/** @file mbtcpconf.h
 *  @brief This contains the prototypes, macros, constants or global variables
 *  for the ModbusTCP Application Configuration
 *  @author Savindra Kumar(savindran1989@gmail.com)
 *  @bug No known bugs.
 */
/*****************************************************************************/
#ifndef _MBTCPCONF_H
#define _MBTCPCONF_H

/******************************************************************************
 *                           Includes
******************************************************************************/

/******************************************************************************
 *                           Constants and typedefs
******************************************************************************/
#define MBTP_CONF_FC_READ_COILS_ENABLE               0
#define MBTP_CONF_FC_READ_DISCRETE_INPUTS_ENABLE     0
#define MBTP_CONF_FC_READ_HOLDING_REGISTERS_ENABLE   0
#define MBTP_CONF_FC_READ_INPUT_REGISTERS_ENABLE     1
#define MBTP_CONF_FC_WRITE_COIL_ENABLE               0
#define MBTP_CONF_FC_WRITE_HOLDING_REGISTER_ENABLE   0
#define MBTP_CONF_FC_WRITE_COILS_ENABLE              0
#define MBTP_CONF_FC_WRITE_HOLDING_REGISTERS_ENABLE  0

/******************************************************************************
 *                           Global variables
******************************************************************************/

/******************************************************************************
 *                           Global Functions
******************************************************************************/

#endif /* _MBTCPCONF_H */
/******************************************************************************
 *                             End of file
******************************************************************************/
/** @}*/
