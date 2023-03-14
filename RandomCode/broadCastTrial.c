#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>


static void
broadcast(const char *mess)
{
    #define BROADCAST_PORT 30000u
    struct sockaddr_in s;

    int broadcastSock = socket(AF_INET, SOCK_DGRAM, 0);
    
    if(broadcastSock < 0)
        return;

    memset(&s, '\0', sizeof(struct sockaddr_in));
    s.sin_family = AF_INET;
    s.sin_port = htons(BROADCAST_PORT);
    s.sin_addr.s_addr = INADDR_BROADCAST; /* This is not correct : htonl(INADDR_BROADCAST); */

    cli_dbgmsg("broadcast %s to %d\n", mess, broadcastSock);
    if(sendto(broadcastSock, mess, strlen(mess), 0, (struct sockaddr *)&s, sizeof(struct sockaddr_in)) < 0)
        perror("sendto");
}
