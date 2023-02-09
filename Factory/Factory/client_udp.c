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

#define PORT 4000

struct pcInfo
{
	char hostName[128];
	char ipNumber[128];
};

void checkHostName(int hostname)
{
    if (hostname == -1)
    {
        perror("gethostname");
        exit(1);
    }
}


// Function that get the hostname and the ip address and (in the future) the mac address and returns them in a struct for it to be sent to the server
struct pcInfo printIPandName()
{

    char hostbuffer[256]; //This will store the hostname of this pc
    int hostname;  
    // To retrieve hostname
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    checkHostName(hostname);

    struct pcInfo newCon;

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

    strcpy(newCon.hostName, hostbuffer);
    strcpy(newCon.ipNumber, apapap);
    
    /*
    printf("Hostname: %s\n", newCon.hostName);
    printf("IP Address is %s - %s\n" , array , newCon.ipNumber);*/
    
    return newCon;
}


//int main(int argc, char *argv[])
int clientUDP()
{
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

	n = sendto(sockfd, newCon, sizeof(struct pcInfo), 0, (const struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in));
	if (n < 0)
		printf("ERROR sendto");

	length = sizeof(struct sockaddr_in);
	n = recvfrom(sockfd, buffer, 256, 0, (struct sockaddr *) &from, &length);
	if (n < 0)
		printf("ERROR recvfrom");

	printf("Got an ack: %s\n", buffer);

	close(sockfd);
	return 0;
}
