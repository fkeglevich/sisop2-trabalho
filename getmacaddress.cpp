#include <iostream>
#include <cstring>
#include <stdlib.h>
using namespace std;
int main ()
{
    //1st way to get the MAC address
    string str1 = "cat /sys/class/net/eth0/address";
    const char *command1 = str1.c_str();
    //2nd way to get MAC address
    string str2 = "ifconfig eth0 | grep -o -E '([[:xdigit:]]{1,2}:){5}[[:xdigit:]]{1,2}' "; 
    const char *command2 = str2.c_str();
    
    system(command1);
    system(command2);
    return 0;
}
