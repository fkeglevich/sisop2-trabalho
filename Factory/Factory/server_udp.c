#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/select.h>
//#include "hosts.c"
#include "client_udp.c"
#include "table.c"

#define PORT 40000
#define PORTBROADCAST 5000
#define PORTTHREAD 10000
#define MAXCONNECTIONS 3



void *caller()
{
	char ipAd[256];
    int sockfd, n;
	unsigned int length;
	struct sockaddr_in serv_addr, from;
	struct hostent *server;

	char buffer[256];

    strcpy(ipAd, getipNumber());

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		printf("ERROR opening socket");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORTBROADCAST);
	serv_addr.sin_addr.s_addr = inet_addr("255.255.255.255");
	bzero(&(serv_addr.sin_zero), 8);

	int enabled = 1;
	setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,&enabled, sizeof(enabled));
	while(1)
	{
		n = sendto(sockfd, ipAd, sizeof(ipAd), 0, (const struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in));
		sleep(3);
	}
	pthread_exit(NULL);

}



void *requestStatus(void* pcDetails)
{
	struct pcInfo *clientInfoAux = pcDetails;

	struct pcInfo clientInfo;
	strcpy(clientInfo.hostName, clientInfoAux->hostName);
	strcpy(clientInfo.macAddress, clientInfoAux->macAddress);
	strcpy(clientInfo.ipNumber, clientInfoAux->ipNumber);
	clientInfo.pos = clientInfoAux->pos;

    int sockfd, sockfd2, n;
	unsigned int length;
	struct sockaddr_in serv_addr_send, serv_addr_recv, cli_addr;
	socklen_t clilen;
	struct hostent *server;

	char buffer[256];

	int currentPort = PORTTHREAD + (clientInfo.pos * 2000);

	
	printf("\n%i\n", currentPort);
	fflush(stdout);

	server = gethostbyname(clientInfo.ipNumber);
	

	if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }	
	
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		printf("ERROR opening socket");

	if ((sockfd2 = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		printf("ERROR opening socket");
	
	serv_addr_send.sin_family = AF_INET;
	serv_addr_send.sin_port = htons(currentPort);	
	serv_addr_send.sin_addr = *((struct in_addr *)server->h_addr);
	
	//serv_addr.sin_addr.s_addr = inet_addr("172.26.209.226");
	bzero(&(serv_addr_send.sin_zero), 8);

	serv_addr_recv.sin_family = AF_INET;
	serv_addr_recv.sin_port = htons(currentPort + 1000);	
	serv_addr_recv.sin_addr = *((struct in_addr *)server->h_addr);
	
	//serv_addr.sin_addr.s_addr = inet_addr("172.26.209.226");
	bzero(&(serv_addr_recv.sin_zero), 8);

	if (bind(sockfd2, (struct sockaddr *) &serv_addr_recv, sizeof(struct sockaddr)) < 0) 
		printf("ERROR on binding");

	struct timeval tv;	
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	int control = 0;
	int whileLoop = 1;

	if (setsockopt(sockfd2, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
	    perror("Error");
	}	

	while(whileLoop)
	{
		sleep(2);
		n = sendto(sockfd, "Are you awake?", sizeof("Are you awake?"), 0, (const struct sockaddr *) &serv_addr_send, sizeof(struct sockaddr_in));

		bzero(&(buffer),256);
		n = recvfrom(sockfd2, buffer, sizeof(buffer), 0, (struct sockaddr *) &serv_addr_recv, &clilen);
		if(!(strcmp(buffer, "I'm awake")))
		{
			if(control > 0)
			{
				wakeUpHost(clientInfo.pos);
				printTable();
			}
			control = 0;
			printf("received message: %s\n", buffer);
		}
		else if(strcmp(buffer, "End"))
		{
			control++;
			if(control > 2)
			{
				if(control == 3)
				{
					sleepHost(clientInfo.pos);
					printTable();
				}
			}
		} else
			whileLoop = 0;

		fflush(stdout);
	}

	removeHost(clientInfo.pos);
	printf("\nended connection\n");
	printTable();
	close(sockfd2);
	close(sockfd);
	pthread_exit(NULL);

}


int serverUDP()
{
	
    init_table();
	//creating();

	struct pcInfo newCon;
	pthread_t tid, tidWait;//, manut[MAXCONNECTIONS];

	void *ret;
	pthread_create( &tid, NULL ,  caller , NULL);


	


	int i = 0;
	int sockfd2, n;
	socklen_t clilen;
	struct sockaddr_in serv_addr2, cli_addr;
	char buf[256];


	
    if ((sockfd2 = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
		printf("ERROR opening socket");

	serv_addr2.sin_family = AF_INET;
	serv_addr2.sin_port = htons(PORT);
	serv_addr2.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serv_addr2.sin_zero), 8);    

	if (bind(sockfd2, (struct sockaddr *) &serv_addr2, sizeof(struct sockaddr)) < 0) 
		printf("ERROR on binding");

	clilen = sizeof(struct sockaddr_in);

	int availablePos = -1;// = 2; //Change value later for 0, is going to be changed with new function


	while (1) {
		HOST currentHost;

		n = recvfrom(sockfd2, &newCon, sizeof(newCon), 0, (struct sockaddr *) &cli_addr, &clilen);

		currentHost = create_host(newCon.hostName, newCon.macAddress, newCon.ipNumber, AWAKEN);


		availablePos = insertHost(currentHost);

		newCon.pos = availablePos;

		n = sendto(sockfd2, &availablePos, sizeof(availablePos), 0,(struct sockaddr *) &cli_addr, sizeof(struct sockaddr));


		if(availablePos >= 0)
		{
			pthread_create( &tidWait, NULL ,  requestStatus , (void *)&newCon);
			
			printTable();

			fflush(stdout);
		}
	}	
	


	destroy_mutex();

	return 0;
}



