//! @addtogroup TCPServerSocket
//! @brief Intialise TCP Server Socket
//! @{
//!
//****************************************************************************/
//! @file tcp.c
//! @brief TCP Server Socket
//! @author Savindra Kumar(savindran1989@gmail.com)
//! @bug No known bugs.
//!
//****************************************************************************/
//****************************************************************************/
//                           Includes
//****************************************************************************/
//standard header files
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
//user defined header files
#include "mbap_conf.h"

//****************************************************************************/
//                           Defines and typedefs
//****************************************************************************/
#define BUFF_SIZE_IN_BYTES   256
#define PORT_NUMBER          502

//****************************************************************************/
//                           external variables
//****************************************************************************/

//****************************************************************************/
//                           Local variables
//****************************************************************************/


//****************************************************************************/
//                           Local Functions
//****************************************************************************/

//****************************************************************************/
//                    G L O B A L  F U N C T I O N S
//****************************************************************************/
//
//! @brief Initalise TCP Server Socket and Call Modbus Application
//! @param[in]  None
//! @param[out] None
//! @return     None
//
void tcp_Init(void)
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
        return;
    }

    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port        = htons(PORT_NUMBER);

    sReturn = bind(sock_desc, (struct sockaddr*)&server, sizeof(server));

    if (-1 == sReturn)
    {
        printf("Error in binding");
        return;
    }

    sReturn = listen(sock_desc, 20);

    if (-1 == sReturn)
    {
        printf("Error in listening");
        return;
    }

    len = sizeof(client);

    CLIENT_REQUEST:
    while ( (temp_sock_desc = accept(sock_desc, (struct sockaddr*)&client, &len)) )
    {
        printf("\nClient connected\n");

        while (1)
        {
            uint16_t usResponseLength = 0;

            sReturn = recv(temp_sock_desc, pucQuery, BUFF_SIZE_IN_BYTES, 0);

            if (0 == sReturn)
            {
                printf("\nConnection closed\n");
                close(temp_sock_desc);
                break;
            }
            else if (sReturn < 0)
            {
                printf("\nConnection reset\n");
                close(temp_sock_desc);
                break;
            }
            else
            {
                printf("Modbus request:\n");

                for (uint16_t usCount = 0; usCount < sReturn; usCount++)
                {
                    printf("%d ", pucQuery[usCount]);
                }

                printf("\n");
            }//end if else

            usResponseLength = mbap_ProcessRequest(pucQuery, sReturn, pucResponse);

            if (0 != usResponseLength)
            {
                sReturn = send(temp_sock_desc, pucResponse, usResponseLength, 0);

                if (sReturn < 0)
                {
                    printf("\nsend failed\n");
                }
                else
                {
                    printf("Modbus response:\n");

                    for (uint16_t usCount = 0; usCount < sReturn; usCount++)
                    {
                        printf("%d ", pucResponse[usCount]);
                    }

                    printf("\n");
                }//end if else
            }//end if
        }//end while
    }//end while


    if (temp_sock_desc < 0)
    {
        printf("accpet failed");
        goto CLIENT_REQUEST;
    }


    exit(0);
}//end TcpInit

//****************************************************************************/
//                           L O C A L  F U N C T I O N S
//****************************************************************************/

//****************************************************************************/
//                             End of file
//****************************************************************************/
/** @}*/
