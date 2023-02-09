#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

int main()
{
    struct ifreq s;
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

    strcpy(s.ifr_name, "eth0");
    if (0 == ioctl(fd, SIOCGIFHWADDR, &s)) {
        int i;
	unsigned char* r = (unsigned char *) s.ifr_addr.sa_data;
        sprintf(r,"%02x:%02x:%02x:%02x:%02x:%02x",(s.ifr_addr.sa_data[0] & 0xff),(s.ifr_addr.sa_data[1] & 0xff),(s.ifr_addr.sa_data[2] & 0xff),(s.ifr_addr.sa_data[3] & 0xff),(s.ifr_addr.sa_data[4] & 0xff),(s.ifr_addr.sa_data[5] & 0xff));
        puts(r);
        return 0;
    }

    return 1;
}
