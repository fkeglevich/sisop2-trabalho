#include <stdio.h>
#include <string.h>

#define ASLEEP 0
#define AWAKEN 1
#define TABLE_SIZE 20
#define CHAR_MAX 256

typedef struct host_struct {
    char hostname[CHAR_MAX];
    char macadd[CHAR_MAX];
    char ipadd[CHAR_MAX];
    int status;
    int index;
} HOST;

HOST table[TABLE_SIZE];

char* status_to_text(int status){

    switch(status){
        case 0: return "ASLEEP";
        break;
        case 1: return "AWAKEN";
        break;
    }
}

int get_next_free_id()
{
    int i;
    for (i = 0; i < TABLE_SIZE; i++) {
        if (table[i].index == -1) {
            return i;
        }
    }
    return -1; //No free ID, table is FULL!
}

HOST create_host(const char* hostname, const char* macadd, const char* ipadd, int status)
{
    HOST host;
    strcpy(host.hostname, hostname);
    strcpy(host.macadd, macadd);
    strcpy(host.ipadd, ipadd);
    host.status = status;
    host.index = -1;
    return host;
}

void init_table()
{
    int i;
    for (i=0; i<TABLE_SIZE; i++) {
        table[i] = create_host("", "", "", AWAKEN);
    }
}

int insertHost(HOST host)
{
    int id = get_next_free_id();
    if (id == -1) {
        return -1; //No free ID, table is FULL!
    }
    table[id] = host;
    table[id].index = id;
    return id;
}

void removeHost(int id)
{
    table[id] = create_host("", "", "", AWAKEN);
}

void printHost(HOST host)
{
    printf("Host hostname: %s ipaddress: %s macaddress: %s status: %s index: %d\n", host.hostname, host.ipadd, host.macadd, status_to_text(host.status), host.index);
}

int main() {

    init_table();
    HOST host = create_host("aaaaasda", "mac", "ipeh", 1);
    // printHost(host);


    int index = insertHost(host);
    printHost(host);
	printHost(table[index]);

    removeHost(0);

    printHost(table[index]);

    return 0;
}