#include <iostream>
#include <cstring>
#include <stdlib.h>
int main(){
	string str = "systemctl hibernate";
	const char *command = str.c_str();
	system(command);
	return 0;
}
