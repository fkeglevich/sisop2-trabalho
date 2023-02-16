#include <stdio.h>
#include <string.h>

#define ASLEEP 0
#define AWAKEN 1
#define TABLE_SIZE 3
#define CHAR_MAX 256

typedef struct host_struct {
    char hostname[CHAR_MAX];
    char macadd[CHAR_MAX];
    char ipadd[CHAR_MAX];
    int status;
    int index;
} HOST;

HOST table[TABLE_SIZE];

// MUTEX CODE BEGIN
pthread_mutex_t lock;
// MUTEX CODE END

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
    //Note: no need to lock here!
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

void init_mutex() {
    // MUTEX CODE BEGIN    
    int ret = pthread_mutex_init(&lock, NULL);
    if (ret != 0) {
        printf("Error creating mutex: %d\n", ret);
        fflush(stdout);
    }
    // MUTEX CODE END
}

void destroy_mutex()
{
    // MUTEX CODE BEGIN
    pthread_mutex_destroy(&lock);
    // MUTEX CODE END
}

void init_table()
{
    init_mutex();

    // MUTEX CODE BEGIN
    pthread_mutex_lock(&lock);
    int i;
    for (i=0; i<TABLE_SIZE; i++) {
        table[i] = create_host("", "", "", AWAKEN);
    }
    pthread_mutex_unlock(&lock);
    // MUTEX CODE END
}

int insertHost(HOST host)
{
    // MUTEX CODE BEGIN
    pthread_mutex_lock(&lock);

    int id = get_next_free_id();
    if (id == -1) {

    	pthread_mutex_unlock(&lock);
        return -1; //No free ID, table is FULL!
    }
    table[id] = host;
    table[id].index = id;

    pthread_mutex_unlock(&lock);
    // MUTEX CODE END
    return id;
}

void removeHost(int id)
{
    // MUTEX CODE BEGIN
    pthread_mutex_lock(&lock);
    table[id] = create_host("", "", "", AWAKEN);
    pthread_mutex_unlock(&lock);
    // MUTEX CODE END
}

void printHost(HOST host)
{
    printf("Host hostname: %s ipaddress: %s macaddress: %s status: %s index: %d\n", host.hostname, host.ipadd, host.macadd, status_to_text(host.status), host.index);
}

void printTable() {
    int i;
    // MUTEX CODE BEGIN
    pthread_mutex_lock(&lock);
    for (i=0; i<TABLE_SIZE; i++) {
        if(table[i].index > -1)
            printHost(table[i]);
    }
    printf("\n\n\n");
    pthread_mutex_unlock(&lock);
    // MUTEX CODE END
}

void wakeUpHost(int id)
{
    // MUTEX CODE BEGIN
    pthread_mutex_lock(&lock);
    table[id].status = AWAKEN;
    pthread_mutex_unlock(&lock);
    // MUTEX CODE END
}

void sleepHost(int id)
{
    // MUTEX CODE BEGIN
    pthread_mutex_lock(&lock);
    table[id].status = ASLEEP;
    pthread_mutex_unlock(&lock);
    // MUTEX CODE END
}

