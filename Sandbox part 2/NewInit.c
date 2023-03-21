#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/select.h>
#include <semaphore.h>
//#include "hosts.c"
#include "client_udp.c"
#include "table.c"


//////TODO: change and organize port numbers///////////////////////////////////
#define PORT 40000
#define PORTBROADCAST 5000
#define PORTSTATUSBASE 5000
#define PORTTHREAD 10000
#define MAXCONNECTIONS 3
#define PORTTABLEATUALIZATION 6000
#define PORTNEWCONNECTION 7000
#define CONTROLTIMES 3


//put global value referring to table

int posicao;
int isElecting;

typedef struct table_struct
{
	pcInfo tabela[MAXCONNECTIONS];
	int clock;
} fullTable;


fullTable tabelaAtual;


//rotina servidor
// se a tabela estiver vazia, inicia uma tabela e se insere nela como servidor
// se a tabela já existir ele vai estar nela (necessita estar na tabela para concorrer) e seta para true o seu status de servidor
// inicia rotina de recebimento de novas entradas
// percorre a tabela iniciando Thread para cada cliente na tabela (mesmo os que estejam dormindo)

void *sendCurrentStatus()
{
    char ipServer[256];

    char auxServer[256];

    int sockfdSendStatus, n2;
	unsigned int length;
	struct sockaddr_in serv_addrServer;
	struct hostent *server;
	pthread_t tid;

    while(1)
    {
        for (int i = 0; i < MAXCONNECTIONS; i++)
        {
            if(tabelaAtual.tabela[i].isServer)
            {
                strcpy(auxServer, tabelaAtual.tabela[i].ipNumber);
                break;
            }

        }


        if(strcmp(auxServer, ipServer))
        {
            strcpy(ipServer, auxServer);

            ///////////////////////////////initializing socket///////////////////////////////////////
	        server = gethostbyname(ipServer);
	        if (server == NULL) {
                fprintf(stderr,"ERROR, no such host\n");
                exit(0);
            }	
        
	        if ((sockfdSendStatus = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	        	printf("ERROR opening socket");
        
	        serv_addrServer.sin_family = AF_INET;
	        serv_addrServer.sin_port = htons(PORTSTATUSBASE + posicao);	
	        serv_addrServer.sin_addr = *((struct in_addr *)server->h_addr);
        
	        bzero(&(serv_addrServer.sin_zero), 8);
        
            //////////////////////////////end of socket initialization
         
        }

        n2 = sendto(sockfdSendStatus, "Awake", sizeof("Awake"), 0, (const struct sockaddr *) &serv_addrServer, sizeof(struct sockaddr_in));


        sleep(1);


    }
    //pegar o ip do server e enviar mensagens de status

    

	pthread_exit(NULL);

}

void *monitoring()
{
    pcInfo thisPC = getIPandName();

    char ipServer[256] = "";

    fullTable EmptyTable;
    EmptyTable.clock = -1;

    fullTable tabelaControle;
    int waitingTable;
    int controle;

    //time struct to define timeout for receive message
	struct timeval tv;	
	tv.tv_sec = 2;
	tv.tv_usec = 0;


    
    ///////////////////////////////////////////socket initialization///////////////////////////////////////////////
    int sockfd, n;
    socklen_t clilen;
    struct sockaddr_in serv_addr;
    char managerIP[256];
    char *ret;
    	
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
    	printf("ERROR opening socket");
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORTTABLEATUALIZATION);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(serv_addr.sin_zero), 8);    
     
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0) 
    	printf("ERROR on binding");
       
    clilen = sizeof(struct sockaddr_in);
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) 
	    perror("Error");	
    //////////////////////////////////////////socket initialization end///////////////////////////////////////////
         


    while (posicao < 0)
    {

        //////////////TODO: verificar esse controle de tabela para ocorrer normalmente, não apenas para inicializar a posição talvez fazer um while true e um if depois para a posição////////////////////
        tabelaControle = EmptyTable;
        waitingTable = 1;
        controle = CONTROLTIMES;



        
        //receber mensagem do servidor com a tabela
        while(waitingTable)
        {
            //verifica se recebeu informacao válida
            n = recvfrom(sockfd, &tabelaControle, sizeof(tabelaControle), 0, (struct sockaddr *) &serv_addr, &clilen);
            if(tabelaControle.clock >= 0)
            {
                waitingTable = 0;
            }
            else
            {
                controle--;
            }

            //se não receber inicia uma tabela vazia e chama algoritmo de eleição
            if(controle < 0)
            {
                isElecting = 1;
                /////////////////////TODO:start thread de eleição que, ao terminar, muda o is electing para false
                controle = CONTROLTIMES;
                
            }

            //espera a eleicao terminar
            while(isElecting)
            {
                sleep(1);
            }
        }   

        //atualiza a tabela local
        tabelaAtual = tabelaControle;

        
        for (int i = 0; i < MAXCONNECTIONS; i++)
        {
            if(tabelaAtual.tabela[i].ipNumber == thisPC.ipNumber)
            {
                thisPC.pos = tabelaAtual.tabela[i].pos;
                posicao = tabelaAtual.tabela[i].pos;
                break;
            }
        }

        if(posicao < 0)
        {
            for (int i = 0; i < MAXCONNECTIONS; i++)
            {
                if(tabelaAtual.tabela[i].isServer)
                {
                    strcpy(ipServer, tabelaAtual.tabela[i].ipNumber);
                    break;
                }
            }
            
            if(ipServer != "")
            {
                ///////////////////////////////initializing socket///////////////////////////////////////
                int sockfdSendInfo, n2;
	            unsigned int length;
	            struct sockaddr_in serv_addrServer;
	            struct hostent *server;
	            pthread_t tid;
            
            
	            server = gethostbyname(ipServer);
	            if (server == NULL) {
                    fprintf(stderr,"ERROR, no such host\n");
                    exit(0);
                }	
            
	            if ((sockfdSendInfo = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	            	printf("ERROR opening socket");
            
	            serv_addrServer.sin_family = AF_INET;
	            serv_addrServer.sin_port = htons(PORTNEWCONNECTION);	
	            serv_addrServer.sin_addr = *((struct in_addr *)server->h_addr);
            
	            bzero(&(serv_addrServer.sin_zero), 8);
            
                //////////////////////////////end of socket initialization

            
	            n2 = sendto(sockfdSendInfo, &thisPC, sizeof(thisPC), 0, (const struct sockaddr *) &serv_addrServer, sizeof(struct sockaddr_in));
            
            }
        
        }
    
        

    }


    // Caso estejam: Recebe e armazena posição, envia mensagem de status ao servidor na porta X+posição passando como parâmetro o ip do servidor

    // INICIA THREAD DE VERIFICAÇÃO

    // Loop waiting for user input
    char input[256];
    while(strcmp(input, "EXIT\n"))
    {
    	bzero(input, 256);
    	fgets(input, 256, stdin);
    }

    //TODO: send disconnection message
	pthread_exit(NULL);

}


int main(int argc, char *argv[])
{
    posicao = -1;
    isElecting = 0;

	pcInfo newCon;
	pthread_t tid;
	void *ret;

	pthread_create( &tid, NULL ,  monitoring, NULL);

    pthread_join(tid, ret);
    printf("waited");
    sleep(1);
    printf("\n %d", posicao);

    return 0;
}

