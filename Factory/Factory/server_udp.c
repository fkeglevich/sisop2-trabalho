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
	struct pcInfo *clientInfo = pcDetails;

	

	printf(clientInfo->hostName);
	printf(clientInfo->ipNumber);
	printf(clientInfo->macAddress);
	printf("%i",clientInfo->pos);
	fflush(stdout);




    int sockfd, n;
	unsigned int length;
	struct sockaddr_in serv_addr, from;
	struct hostent *server;

	char buffer[256];

	//if(newFunction())

	//int currentPort = 

	server = gethostbyname(clientInfo->ipNumber);
	

	if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }	
	
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		printf("ERROR opening socket");
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORTTHREAD);	
	serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
	
	//serv_addr.sin_addr.s_addr = inet_addr("172.26.209.226");
	bzero(&(serv_addr.sin_zero), 8);


	printf("\n Started Sending\n");
	fflush(stdout);
	n = sendto(sockfd, "its sending Back", sizeof("its sending Back"), 0, (const struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in));

	pthread_exit(NULL);

}


int serverUDP()
{
	//creating();

	struct pcInfo newCon;
	pthread_t tid, tidWait;//, manut[MAXCONNECTIONS];

	void *ret;
	pthread_create( &tid, NULL ,  caller , NULL);//pthread_create( &tid, NULL ,  requestStatus , (void *)newCon);


	


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

	while (1) {
		HOST currentHost;
		
		n = recvfrom(sockfd2, &newCon, sizeof(newCon), 0, (struct sockaddr *) &cli_addr, &clilen);
	
		//printf("Received a datagram: %s\n", buf);
		//struct pcInfo aaaa = *newCon;
		printf("Hostname is: %s\n", newCon.hostName);//newCon.hostName);
		fflush(stdout);
 	    //printf("IP Address is: %s\n" , newCon.ipNumber);
		//fflush(stdout);
        //printf("Mac Address is: %s\n" , newCon.macAddress);
		//fflush(stdout);
		
		pthread_create( &tidWait, NULL ,  requestStatus , (void *)&newCon);
	}	
	




	return 0;
}



