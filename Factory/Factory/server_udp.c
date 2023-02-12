#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>
#include "hosts.c"
#include "client_udp.c"

#define PORT 4000
#define MAXCONNECTIONS 3


void *newConnectionThread()
{
printf("oi");
static int connections = 0;

printf("new thread, connections made =%d", ++connections);
pthread_exit(NULL);

}

int serverUDP()
{
	//creating();

	struct pcInfo newCon;
	pthread_t tid;


	int i = 0;
	int sockfd, n;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	char buf[256];
		
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
		printf("ERROR opening socket");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serv_addr.sin_zero), 8);    
	 
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0) 
		printf("ERROR on binding");
	
	clilen = sizeof(struct sockaddr_in);
	
	while (1) {
		HOST currentHost;
		/* receive from socket */
		n = recvfrom(sockfd, &newCon, sizeof(newCon), 0, (struct sockaddr *) &cli_addr, &clilen);

		printf("Received a datagram: %s\n", buf);
	//struct pcInfo aaaa = *newCon;
		printf("Hostname is: %s\n", newCon.hostName);
        printf("IP Address is: %s\n" , newCon.ipNumber);
        printf("Mac Address is: %s\n" , newCon.macAddress);
			
			
			
			
 
//			printf("ERROR on recvfrom");
	
//		pthread_create(&tid, NULL, newConnectionThread, NULL);

		

		/* send to socket */
		n = sendto(sockfd, "Got your message\n", 17, 0,(struct sockaddr *) &cli_addr, sizeof(struct sockaddr));
		if (n  < 0) 
			printf("ERROR on sendto");

	}
	
	close(sockfd);
	return 0;
}



