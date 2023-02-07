#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define ASLEEP 0
#define AWAKEN 1
#define M 19

using namespace std;

typedef struct host{
    char hostname[128];
    char macadd[128];
    char ipadd[128];
    int status;
}HOST;

typedef struct hosts{
    HOST host;
    struct hosts *next;
}HOSTS;

HOSTS* tabela[M];
int serialnumber = 0;

void initialize_table()
{
    int i;
    for (i=0;i<M;i++)
        tabela[i] = 0;
}

int create_key(){
    return 1 + rand() % 18;
}

void inserir(HOST host){
    HOSTS* newhost = NULL;
    int indice = create_key();
    newhost = (HOSTS*) calloc(1, sizeof(HOSTS));
    strcpy(newhost->host.hostname,host.hostname);
    strcpy(newhost->host.macadd,host.macadd);
    strcpy(newhost->host.ipadd,host.ipadd);
    tabela[indice] = newhost;
}


char* convert_text(int val){

    switch(val){
        case 0: return "ASLEEP";
        break;
        case 1: return "AWAKEN";
        break;
    }
}

void imprime(){
    HOSTS* node;
    for(int i = 0; i < M; i++){
        if(tabela[i] != 0){
            for(node = tabela[i]; node != NULL; node = node->next){
                printf("Table[%d] - hostname: %s ipaddress: %s macaddress: %s status: %s\n", i, node->host.hostname, node->host.ipadd, node->host.macadd, convert_text(node->host.status));
            }
        }
    }

}

void remover(char* ipadd){
    HOSTS* node;
    for(int i = 0; i < M; i++)
        if(tabela[i] != NULL)
            if(strcmp(tabela[i]->host.ipadd,ipadd)==0){
                tabela[i]=0;
                break;
            }
}

int main(){
    char buffer[128];
    serialnumber++;
    sprintf(buffer,"host%d",serialnumber);
    char ipaddr[100] = "127.68.90.100";
    HOST host;

    strcpy(host.hostname,buffer);
    strcpy(host.ipadd,"127.68.90.100");
    strcpy(host.macadd,"tal imac");
    host.status = AWAKEN;

    initialize_table();

    inserir(host);

    imprime();

    printf("oi");
    remover(ipaddr);

    printf("\ntabela depois da remocao: \n");

    imprime();

    return 0;
}
