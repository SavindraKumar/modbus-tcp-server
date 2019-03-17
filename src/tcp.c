/** @addtogroup TCPServerSocket
 *  @brief Intialise TCP Server Socket
 *  @{
 */
/*****************************************************************************/
/** @file tcp.c
 *  @brief TCP Server Socket
 *  @author Savindra Kumar(savindran1989@gmail.com)
 *  @bug No known bugs.
 */
/*****************************************************************************/
/******************************************************************************
 *                           Includes
 ******************************************************************************/
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "mbtcpconf.h"

/******************************************************************************
 *                           Defines and typedefs
 ******************************************************************************/
#define BUFF_SIZE_IN_BYTES   256
#define PORT_NUMBER          50001

/******************************************************************************
 *                           external variables
 ******************************************************************************/

/******************************************************************************
 *                           Local variables
 ******************************************************************************/


/******************************************************************************
 *                           Local Functions
 ******************************************************************************/

/******************************************************************************
 *                    G L O B A L  F U N C T I O N S
 ******************************************************************************/
/**@brief Initalise TCP Server Socke and Call Modbus Application
 * @param[in] None
 * @return    None
 */
void TcpInit(void)
{
    uint8_t pucQuery[BUFF_SIZE_IN_BYTES];
    uint8_t pucResponse[BUFF_SIZE_IN_BYTES];
    int16_t sReturn;
    socklen_t len;
    int sock_desc;
	int temp_sock_desc;
    struct sockaddr_in server;
	struct sockaddr_in client;

    memset(&server, 0, sizeof(server));
    memset(&client, 0, sizeof(client));

    sock_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_desc == -1)
    {
        printf("Error in socket creation");
        exit(1);
    }

    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port        = PORT_NUMBER;

    sReturn = bind(sock_desc, (struct sockaddr*)&server, sizeof(server));

    if (-1 == sReturn)
    {
        printf("Error in binding");
        exit(1);
    }

    sReturn = listen(sock_desc, 20);

    if (-1 == sReturn)
    {
        printf("Error in listening");
        exit(1);
    }

    len            = sizeof(client);
    temp_sock_desc = accept(sock_desc, (struct sockaddr*)&client, &len);

    if (-1 == temp_sock_desc)
    {
        printf("Error in temporary socket creation");
        exit(1);
    }


    while (1)
    {
    	uint16_t usResponseLength = 0;

        sReturn = recv(temp_sock_desc, pucQuery, BUFF_SIZE_IN_BYTES, 0);

        if (sReturn == -1)
        {
            printf("Error in receiving");
            exit(1);
        }

        usResponseLength = MBT_ProcessRequest(pucQuery, sReturn, pucResponse);

        if (0 != usResponseLength)
        {
            sReturn = send(temp_sock_desc, pucResponse, usResponseLength, 0);
        }


        if(sReturn==-1)
        {
            printf("Error in sending");
            exit(1);
        }
    }
    close(temp_sock_desc);

    exit(0);
}//end TcpInit

/******************************************************************************
 *                           L O C A L  F U N C T I O N S
 ******************************************************************************/

/******************************************************************************
 *                             End of file
 ******************************************************************************/
/** @}*/

