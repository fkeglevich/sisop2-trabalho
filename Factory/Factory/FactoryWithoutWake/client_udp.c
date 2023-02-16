#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/if.h>

#define PORT 40000
#define PORTBROADCAST 5000
#define PORTTHREAD 10000

int VariavelDeControle = 1;

struct pcInfo
{
	char hostName[256];
	char ipNumber[256];
    char macAddress[256];
	int pos;
} ;

void checkHostName(int hostname)
{
    if (hostname == -1)
    {
        perror("gethostname");
        exit(1);
    }
}

char* getipNumber()
{
    int n;

    struct ifreq ifr;

    char array[] = "eth0";
    n = socket(AF_INET, SOCK_DGRAM, 0);

    //Type of address to retrieve - IPv4 IP address

    ifr.ifr_addr.sa_family = AF_INET;

    //Copy the interface name in the ifreq structure

    strncpy(ifr.ifr_name , array , IFNAMSIZ - 1);


    ioctl(n, SIOCGIFADDR, &ifr);

    close(n);

    char *apapap = inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr);
    return apapap;
}

char* getMac()
{
    struct ifreq s;
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    char *macAd = "";

    strcpy(s.ifr_name, "eth0");
    if (0 == ioctl(fd, SIOCGIFHWADDR, &s)) {
        int i;
	    unsigned char* r = (unsigned char *) s.ifr_addr.sa_data;
        sprintf(r,"%02x:%02x:%02x:%02x:%02x:%02x",(s.ifr_addr.sa_data[0] & 0xff),(s.ifr_addr.sa_data[1] & 0xff),(s.ifr_addr.sa_data[2] & 0xff),(s.ifr_addr.sa_data[3] & 0xff),(s.ifr_addr.sa_data[4] & 0xff),(s.ifr_addr.sa_data[5] & 0xff));
        puts(r);
        return r;
    }

    return macAd;

}

char* getHost()
{
    char hostbuffer[256]; //This will store the hostname of this pc
    int hostname;  
    // To retrieve hostname
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    checkHostName(hostname);
    char *hostNew = hostbuffer;
    return hostNew;
}

// Function that get the hostname and the ip address and (in the future) the mac address and returns them in a struct for it to be sent to the server
struct pcInfo getIPandName()
{

    struct pcInfo newCon;

    strcpy(newCon.hostName, getHost());
    strcpy(newCon.ipNumber, getipNumber());
    strcpy(newCon.macAddress, getMac());
	newCon.pos = -1;
    return newCon;
}



void *sendStatus(void* pcDetails)
{
	struct pcInfo *clientInfo = pcDetails;


	int currentPort = PORTTHREAD + (clientInfo->pos * 2000);


    int k = 0;
	int sockfd3, sockfd, n3;
	socklen_t clilen3;
	struct sockaddr_in serv_addr3, serv_addr, cli_addr3;
	char buffer[256];
	struct hostent *server;
		
	server = gethostbyname(clientInfo->ipNumber);
    if ((sockfd3 = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
		printf("ERROR opening socket");

	serv_addr3.sin_family = AF_INET;
	serv_addr3.sin_port = htons(currentPort);
	serv_addr3.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serv_addr3.sin_zero), 8);    
	 
	if (bind(sockfd3, (struct sockaddr *) &serv_addr3, sizeof(struct sockaddr)) < 0) 
		printf("ERROR on binding");

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
		printf("ERROR opening socket");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(currentPort + 1000);
	serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
	bzero(&(serv_addr.sin_zero), 8);   

	
	//if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0) 
	//	printf("ERROR on binding hereeeee");
	
	clilen3 = sizeof(struct sockaddr_in);
	


	while(VariavelDeControle)
	{
		
    	n3 = recvfrom(sockfd3, buffer, sizeof(buffer), 0, (struct sockaddr *) &cli_addr3, &clilen3);
		//printf("Received a datagram: %s\n", buffer);

			n3 = sendto(sockfd, "I'm awake", sizeof("I'm awake"), 0, (const struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in));
	
	}

	printf("\nEnding conection.");
	n3 = sendto(sockfd, "End", sizeof("End"), 0, (const struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in));
	printf(".");
	n3 = sendto(sockfd, "End", sizeof("End"), 0, (const struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in));
	printf(".");	
	n3 = sendto(sockfd, "End", sizeof("End"), 0, (const struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in));
	

	pthread_exit(NULL);

}

//int main(int argc, char *argv[])
int clientUDP()
{
    // This first part of the program will get the message broadcasted by the manager to get the ip of the machine
    //int i = 0;
	int sockfd, n;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	char managerIP[256];
	char *ret;
		
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
		printf("ERROR opening socket");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORTBROADCAST);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serv_addr.sin_zero), 8);    
	 
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0) 
		printf("ERROR on binding");
	
	clilen = sizeof(struct sockaddr_in);
	
    int f = 1;
    n = recvfrom(sockfd, managerIP, sizeof(managerIP), 0, (struct sockaddr *) &cli_addr, &clilen);


 

	close(sockfd);
    ////////////////////////////////end of first receive, start of first send////////////////////////////////


   	struct pcInfo newCon = getIPandName();


    int sockfd2, n2;
	unsigned int length;
	struct sockaddr_in serv_addr2, from;
	struct hostent *server;
	pthread_t tid;


	server = gethostbyname(managerIP);
	if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }	
	
	if ((sockfd2 = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		printf("ERROR opening socket");
	
	serv_addr2.sin_family = AF_INET;
	serv_addr2.sin_port = htons(PORT);	
	serv_addr2.sin_addr = *((struct in_addr *)server->h_addr);
	
	bzero(&(serv_addr2.sin_zero), 8);

	int availablePos;

	n2 = sendto(sockfd2, &newCon, sizeof(newCon), 0, (const struct sockaddr *) &serv_addr2, sizeof(struct sockaddr_in));

			
	n = recvfrom(sockfd2, &availablePos, sizeof(availablePos), 0, (struct sockaddr *) &cli_addr, &clilen);

	newCon.pos = availablePos;
	strcpy(newCon.ipNumber, managerIP); //gambiarra aqui

	close(sockfd2);

	///////////////////////////////// end of first send, start of second receive////////////////////////////////////

	if(newCon.pos < 0)
	{
		printf("The server is already full, Please try to connect again later.\nExiting process...");
	}
	else
	{
		// Started thread to send messages while the machine is awake
		pthread_create( &tid, NULL ,  sendStatus , (void *)&newCon);

		// while que recebe inputs do usuário até que ele digite exit
		char input[256];
		while(strcmp(input, "exit\n"))
		{
			bzero(input, 256);
			fgets(input, 256, stdin);
		}

		VariavelDeControle = 0;
	}

	pthread_join(tid, (void **)&ret);

	return 0;
}
