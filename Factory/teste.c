#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "client_udp.c"
#include "server_udp.c"


int main(int argc, char *argv[])
{
if((argc > 1) && (!strcmp(argv[1],"manager")))
	serverUDP();
else
	clientUDP();


return 0;
}
