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





int serverUDP()
{
	//creating();

	struct pcInfo newCon;
	pthread_t tid, tidWait;

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

	while (1) {
		HOST currentHost;
		
		n = recvfrom(sockfd2, &newCon, sizeof(newCon), 0, (struct sockaddr *) &cli_addr, &clilen);
	
		//printf("Received a datagram: %s\n", buf);
		//struct pcInfo aaaa = *newCon;
		printf("Hostname is: %s\n", newCon.hostName);//newCon.hostName);
		fflush(stdout);
 	    printf("IP Address is: %s\n" , newCon.ipNumber);
		fflush(stdout);
        printf("Mac Address is: %s\n" , newCon.macAddress);
		fflush(stdout);
	}	
	


	/* 		
 
//			printf("ERROR on recvfrom");
	
//		pthread_create(&tid, NULL, newConnectionThread, NULL);

		
		n = sendto(sockfd, "Got your message\n", 17, 0,(struct sockaddr *) &cli_addr, sizeof(struct sockaddr));
		if (n  < 0) 
			printf("ERROR on sendto");

	}
	*/	
	
	
	//close(sockfd);

	
	//pthread_join(tid, ret);

	return 0;
}



