#include <stdio.h>
#include <string.h>

#define ASLEEP 0
#define AWAKEN 1
#define TABLE_SIZE 3
#define CHAR_MAX 256

typedef struct pcInfo_struct
{
	char hostName[256];
	char ipNumber[256];
    char macAddress[256];
	int pos;
	int status;
	int isServer;
} pcInfo;

typedef struct table_struct
{
	pcInfo tabela[TABLE_SIZE];
	int clock;
} fullTable;


fullTable tabelaAtual;

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
        printf("checked position = %d\n",tabelaAtual.tabela[i].pos );
        if (tabelaAtual.tabela[i].pos < 0) {
            return i;
        }
    }
    return -1; //No free ID, tabelaAtual is FULL!
}

pcInfo create_host(const char* hostname, const char* macadd, const char* ipadd, int status)
{
    pcInfo host;
    strcpy(host.hostName, hostname);
    strcpy(host.macAddress, macadd);
    strcpy(host.ipNumber, ipadd);
    host.status = status;
    host.pos = -1;
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
        tabelaAtual.tabela[i] = create_host("", "", "", AWAKEN);
    }
    tabelaAtual.clock = 1;
    pthread_mutex_unlock(&lock);
    // MUTEX CODE END
}

int insertHost(pcInfo host)
{

    // MUTEX CODE BEGIN
    pthread_mutex_lock(&lock);

    int id = get_next_free_id();
    if (id == -1) {
    	pthread_mutex_unlock(&lock);
        return -1; //No free ID, table is FULL!
    }

    tabelaAtual.tabela[id] = host;
    tabelaAtual.tabela[id].pos = id;
    tabelaAtual.tabela[id].status = AWAKEN;
    tabelaAtual.clock++;
    pthread_mutex_unlock(&lock);
    // MUTEX CODE END
    return id;
}

void removeHost(int id)
{
    // MUTEX CODE BEGIN
    pthread_mutex_lock(&lock);
    tabelaAtual.tabela[id] = create_host("", "", "", AWAKEN);
    tabelaAtual.clock++;
    pthread_mutex_unlock(&lock);
    // MUTEX CODE END
}

void printHost(pcInfo host)
{
    printf("Host hostname: %s ipaddress: %s macaddress: %s status: %s index: %d is Server: %d\n", host.hostName, host.ipNumber, host.macAddress, status_to_text(host.status), host.pos, host.isServer);
}

void printTable() {
    int i;

    // MUTEX CODE BEGIN
    pthread_mutex_lock(&lock);

    for (i=0; i<TABLE_SIZE; i++) {
        if(tabelaAtual.tabela[i].pos > -1)
            printHost(tabelaAtual.tabela[i]);
    }
    printf("\n\n\n");
    pthread_mutex_unlock(&lock);
    // MUTEX CODE END
}

void wakeUpHost(int id)
{
    // MUTEX CODE BEGIN
    pthread_mutex_lock(&lock);
    tabelaAtual.tabela[id].status = AWAKEN;
    tabelaAtual.clock++;
    pthread_mutex_unlock(&lock);
    // MUTEX CODE END
}

void sleepHost(int id)
{
    // MUTEX CODE BEGIN
    pthread_mutex_lock(&lock);
    tabelaAtual.tabela[id].status = ASLEEP;
    tabelaAtual.clock++;
    pthread_mutex_unlock(&lock);
    // MUTEX CODE END
}

pcInfo findHostByName(const char* hostname) {
    int i;
    pcInfo result = create_host("", "", "", AWAKEN); // default result if not found
    // MUTEX CODE BEGIN
    pthread_mutex_lock(&lock);
    for (i=0; i<TABLE_SIZE; i++) {
        if(!(strcmp(tabelaAtual.tabela[i].hostName, hostname))) {
            result = tabelaAtual.tabela[i];
            break;
        }
    }
    // MUTEX CODE END
    pthread_mutex_unlock(&lock);
    return result;
}
