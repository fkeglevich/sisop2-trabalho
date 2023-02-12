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

struct pcInfo
{
	char hostName[256];
	char ipNumber[256];
    char macAddress[256];
};

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
struct pcInfo printIPandName()
{

    struct pcInfo newCon;

    strcpy(newCon.hostName, getHost());
    strcpy(newCon.ipNumber, getipNumber());
    strcpy(newCon.macAddress, getMac());
    
    return newCon;
}


//int main(int argc, char *argv[])
int clientUDP()
{
    // This first part of the program will get the message broadcasted by the manager to get the ip of the machine
    int i = 0;
	int sockfd, n;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	char managerIP[256];
		
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

	printf("Received a datagram: %s\n", managerIP);
   
   if(!strcmp(managerIP,"172.26.209.226"))
   {
		printf("It matches");
   }
   
   // Now we need to get the simple message to transmit back to manager all pcInformation
   
	close(sockfd);
    


   	struct pcInfo newCon = printIPandName();


    int sockfd2, n2;
	unsigned int length;
	struct sockaddr_in serv_addr2, from;
	struct hostent *server;



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
	
	//serv_addr.sin_addr.s_addr = inet_addr("172.26.209.226");
	bzero(&(serv_addr2.sin_zero), 8);


/*
	if (bind(sockfd2, (struct sockaddr *) &serv_addr2, sizeof(struct sockaddr)) < 0) 
		printf("ERROR on binding");
	fflush(stdout);
*/
	n2 = sendto(sockfd2, &newCon, sizeof(newCon), 0, (const struct sockaddr *) &serv_addr2, sizeof(struct sockaddr_in));


	close(sockfd2);
   
/*  
	
 
	struct pcInfo newCon = printIPandName();
        char array[] = "eth0";
        printf("Hostname: %s\n", newCon.hostName);
        printf("IP Address is %s - %s\n" , array , newCon.ipNumber);

        int sockfd, n;
	unsigned int length;
	struct sockaddr_in serv_addr, from;
	struct hostent *server;

	char buffer[256];

	unsigned char *test = (unsigned char *)&newCon;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		printf("ERROR opening socket");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = inet_addr("255.255.255.255");
	bzero(&(serv_addr.sin_zero), 8);

	printf("Enter the message: ");
	bzero(buffer, 256);
	fgets(buffer, 256, stdin);
	int enabled = 1;
	setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,&enabled, sizeof(enabled));

	n = sendto(sockfd, &newCon, sizeof(newCon), 0, (const struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in));
	if (n < 0)
		printf("ERROR sendto");

	length = sizeof(struct sockaddr_in);
	n = recvfrom(sockfd, buffer, 256, 0, (struct sockaddr *) &from, &length);
	if (n < 0)
		printf("ERROR recvfrom");

	printf("Got an ack: %s\n", buffer);

	close(sockfd);
    */
	return 0;
}
