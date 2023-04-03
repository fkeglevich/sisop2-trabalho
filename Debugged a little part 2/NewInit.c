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
//#include "table2.c"


//////TODO: change and organize port numbers///////////////////////////////////
#define PORT 40000
#define PORTBROADCAST 5000
#define PORTSTATUSBASE 5000
#define PORTTHREAD 10000
#define MAXCONNECTIONS 3
#define PORTSENDIPSERVER 6000
#define PORTNEWCONNECTION 7000
#define PORTSENDTABLE 8000
#define CONTROLTIMES 3


//put global value referring to table

int posicao;
int isElecting;
int isServer;
int notExiting;

sem_t semaphore;
/*
typedef struct table_struct
{
	pcInfo tabela[MAXCONNECTIONS];
	int clock;
} fullTable;


fullTable tabelaAtual;
*/



void *electionRoutine()
{
    isElecting = 1;
    //////////////////////////TODO: Implementar

    isElecting = 0;
	pthread_exit(NULL);
}



void *checkCurrentStatus(void *pos)
{
    int *checkPosicao = pos;
	int posAux = *checkPosicao;


	sem_post(&semaphore);
	//TODO:colocar semaforo aqui//////////////////////////////////////////////////////////////////////////////////////////
    int controle;
	char mensagem[256];
    int isIn = 1;

    //time struct to define timeout for receive message
	struct timeval tv;
	tv.tv_sec = 3;
	tv.tv_usec = 0;

    ///////////////////////////////////////////socket initialization///////////////////////////////////////////////
    int sockfd, n;
    socklen_t clilen;
    struct sockaddr_in serv_addr;
    char status[256];
    char *ret;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    	printf("ERROR opening socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORTSTATUSBASE + posAux);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(serv_addr.sin_zero), 8);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0)
    	printf("ERROR on binding 4");

    clilen = sizeof(struct sockaddr_in);
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0)
	    perror("Error");
    //////////////////////////////////////////socket initialization end///////////////////////////////////////////


	controle = CONTROLTIMES;
	while(isServer && isIn)
	{


        n = recvfrom(sockfd, &mensagem, sizeof(mensagem), 0, (struct sockaddr *) &serv_addr, &clilen);
        if(!strcmp(mensagem,"Awake") )
        {
            printf("received in position:%d", posAux);
		    if(controle <= 0)
		    {
                wakeUpHost(posAux);
                printTable();
		    }
            controle = CONTROLTIMES;
            strcpy(mensagem, "");
        }
		else if(!strcmp(mensagem,"Ended") )
        {
            printf("ended connection in position:%d", posAux);
            removeHost(posAux);
            printTable();
            isIn = 0;

        }
        else
		{
            printf("didn't received in position:%d", posAux);
			controle--;
		}

		if(controle <= 0)
		{
		    if(tabelaAtual.tabela[posAux].status)
			{
                sleepHost(posAux);
                printTable();
			}
		}
	}
    //////////////////////////TODO: Terminar
	printf("Exited while\n");
    close(sockfd);
	fflush(stdout);
	pthread_exit(NULL);
}



void *receiveNewConnections()
{

    int isNewConnection = 1;


    /////////////////////////////////Socket Initialization//////////////////////////////////////////
    int sockfd, n;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	char managerIP[256];
	char *ret;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		printf("ERROR opening socket");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORTNEWCONNECTION);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serv_addr.sin_zero), 8);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0)
		printf("ERROR on binding 1");

	clilen = sizeof(struct sockaddr_in);

    //////////////////////////////////////End of Socket Initialization///////////////////////////////

    printf("\nstarted receiving new connections\n");
    while(isServer)
    {
        pcInfo thisPC;
        n = recvfrom(sockfd, &thisPC, sizeof(thisPC), 0, (struct sockaddr *) &cli_addr, &clilen);

        //percorre a tabela e verifica se a informação lida já está nela
        for(int i = 0; i < MAXCONNECTIONS; i++)
        {
            if(!strcmp(thisPC.ipNumber, tabelaAtual.tabela[i].ipNumber))
                isNewConnection = 0;
        }

        if(isNewConnection)
        {
            thisPC.pos = insertHost(thisPC);
            printTable();
            pthread_t tid;
            sem_init(&semaphore,0,1);
            pthread_create( &tid, NULL ,  checkCurrentStatus, &thisPC.pos);
                //starta thread de monitoramento
                /////////////TODO:insere na tabela as informações
        }
	fflush(stdout);
        isNewConnection = 1;
    }



	close(sockfd);



	pthread_exit(NULL);
}


void *send_table(){
    int sockfd, n,i;
	unsigned int length;
	struct sockaddr_in serv_addr, from;
	struct hostent *server;

    printf("reached here");
    fflush(stdout);

    while(1)
    {
        for(i=0; i<TABLE_SIZE; i++)
        {

            if (tabelaAtual.tabela[i].pos > -1)
            {
                if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
                    printf("ERROR opening socket");

                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(PORTSENDTABLE);
                serv_addr.sin_addr.s_addr = inet_addr(tabelaAtual.tabela[i].ipNumber);
                bzero(&(serv_addr.sin_zero), 8);

                n = sendto(sockfd, &tabelaAtual, sizeof(tabelaAtual), 0, (const struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in));
                close(sockfd);

            }
        }
        sleep(1);
    }

}




void *serverRotine()
{
    sem_init(&semaphore,0,1);
    printf("started server function");
    fflush(stdout);
	pthread_t tid[MAXCONNECTIONS];
    if(isServer)
    {
        for(int i = 0; i < MAXCONNECTIONS; i++)
        {
		///////////TODO: verificar se tem alguem nessa posicao da tabela
            if(!tabelaAtual.tabela[i].isServer && tabelaAtual.tabela[i].pos >= 0)
            {
                sem_init(&semaphore,0,1);
                pthread_create( &tid[i], NULL ,  checkCurrentStatus, &i);
            }
        }
    }
    //////////////////////////////////Starting socket initialization//////////////////////////////////////

    pthread_t tid3;
	pthread_create( &tid3, NULL ,  send_table, NULL);
    //TODO: loopar o envio da tabela

	//Creating thread to receive new connections

	pthread_t tid2;
	pthread_create( &tid2, NULL ,  receiveNewConnections, NULL);




    //////////////////////////////////Starting socket initialization//////////////////////////////////////

    /////////////////////////////TODO: mudar de broadcast para multicast(envia uma mensagem para cada endereco)
    int sockfd, n;
	unsigned int length;
	struct sockaddr_in serv_addr, from;
	struct hostent *server;


	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		printf("ERROR opening socket");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORTSENDIPSERVER);
	serv_addr.sin_addr.s_addr = inet_addr("255.255.255.255");
	bzero(&(serv_addr.sin_zero), 8);

	int enabled = 1;
	setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,&enabled, sizeof(enabled));

    ///////////////////////////////////Ending socket Initialization////////////////////////////////////////

    pcInfo thisPC = getIPandName();
    while(isServer)
    {
        printf("is Sending");
        fflush(stdout);
        n = sendto(sockfd, &thisPC.ipNumber, sizeof(thisPC.ipNumber), 0, (const struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in));
        //n = sendto(sockfd, &table, sizeof(table), 0, (const struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in));

		sleep(2);
    }

    printf("ended server function\n");

	sem_destroy(&semaphore);
	pthread_exit(NULL);

}


void *receive_table(){
    struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;
    int sockfd, n;
    socklen_t clilen;
    struct sockaddr_in serv_addr;
    char managerIP[256];
    char *ret;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    	printf("ERROR opening socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORTSENDTABLE);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(serv_addr.sin_zero), 8);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0)
    	printf("ERROR on binding 2");

    clilen = sizeof(struct sockaddr_in);
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0)
	    perror("Error");


    int controle;
    controle = CONTROLTIMES;
    fullTable EmptyTable;

    fullTable tabelaControle;

    while(1)
    {
        tabelaControle = EmptyTable;
        //verifica se recebeu informacao válida
        n = recvfrom(sockfd, &tabelaControle, sizeof(tabelaControle), 0, (struct sockaddr *) &serv_addr, &clilen);
        //n = recvfrom(sockfd, &table, sizeof(table), 0, (struct sockaddr *) &serv_addr, &clilen);


        if(tabelaControle.clock > 0)
        {

            if(tabelaControle.clock > tabelaAtual.clock)
            {
                printf("atualizou a tabela");
                tabelaAtual = tabelaControle;
                printTable();
                if(isServer)
                {
                    //TODO: chama eleição
                    printf("servidor também recebe");
                }

            }

            controle = CONTROLTIMES;
        }
        else
        {
            printf("couldn't receive table");
            controle--;
        }

        if(!isServer)
        {

        //se não receber inicia uma tabela vazia e se torna server
            if(controle < 0)
            {
                //TODO: inicia algoritmo de eleição


                controle = CONTROLTIMES;
            }
        }
    }

}

void *sendCurrentStatus()
{
    char ipServer[256];

    char auxServer[256];

    int sockfdSendStatus, n2;
	unsigned int length;
	struct sockaddr_in serv_addrServer;
	struct hostent *server;
	pthread_t tid;


    int controle = CONTROLTIMES;

    while(controle > 0)
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

        if(notExiting)
            n2 = sendto(sockfdSendStatus, "Awake", sizeof("Awake"), 0, (const struct sockaddr *) &serv_addrServer, sizeof(struct sockaddr_in));
        else
        {
            controle--;
            n2 = sendto(sockfdSendStatus, "Ended", sizeof("Ended"), 0, (const struct sockaddr *) &serv_addrServer, sizeof(struct sockaddr_in));
        }
        sleep(1);


    }
    //pegar o ip do server e enviar mensagens de status

    printf("\\n ended while \n\n");

	pthread_exit(NULL);

}

void *monitoring()
{
    pcInfo thisPC = getIPandName();



    char ipServer[256] = "";

    fullTable EmptyTable;
    EmptyTable.clock = -1;

    fullTable tabelaControle;
    int waitingServerIp;
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
    serv_addr.sin_port = htons(PORTSENDIPSERVER);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(serv_addr.sin_zero), 8);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0)
    	printf("ERROR on binding 3");

    clilen = sizeof(struct sockaddr_in);
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0)
	    perror("Error");
    //////////////////////////////////////////socket initialization end///////////////////////////////////////////

    pthread_t tid3;
    pthread_create( &tid3, NULL ,  receive_table, NULL);

    while (posicao < 0)
    {

        waitingServerIp = 1;
        controle = CONTROLTIMES;
        char enderecoServer[256]  = "";

        //receber mensagem do servidor com a tabela
        while(waitingServerIp)
        {

            //verifica se recebeu informacao válida
            n = recvfrom(sockfd, &enderecoServer, sizeof(enderecoServer), 0, (struct sockaddr *) &serv_addr, &clilen);
            //n = recvfrom(sockfd, &table, sizeof(table), 0, (struct sockaddr *) &serv_addr, &clilen);


            if(strcmp(enderecoServer, ""))
            {
                waitingServerIp = 0;
            }
            else
            {
                controle--;
            }


            //se não receber inicia uma tabela vazia e se torna server
            if(controle < 0)
            {
                isServer = 1;

                pcInfo thisC  = getIPandName();
                thisC.isServer = 1;
                posicao = 0;

                init_table();
                insertHost(thisC);

                printTable();

	            pthread_t tid;
	            pthread_create( &tid, NULL ,  serverRotine, NULL);

                waitingServerIp = 0;
                controle = CONTROLTIMES;
            }
        }


        printf("ended waiting server ip loop");
        for (int i = 0; i < MAXCONNECTIONS; i++)
        {

            if(!strcmp(tabelaAtual.tabela[i].ipNumber, thisPC.ipNumber))
            {
                printf("entrou no if\n\n");
                thisPC.pos = tabelaAtual.tabela[i].pos;
                posicao = tabelaAtual.tabela[i].pos;
                printTable();
                break;
            }
        }

        if(posicao < 0)
        {

            if(enderecoServer != "")
            {
                //Send this pc data for server to be included in table

                ///////////////////////////////initializing socket///////////////////////////////////////
                int sockfdSendInfo, n2;
	            unsigned int length;
	            struct sockaddr_in serv_addrServer;
	            struct hostent *server;
	            pthread_t tid;


	            server = gethostbyname(enderecoServer);
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

            	printf("\nstarted sending info:%s\n", thisPC.ipNumber);
	            n2 = sendto(sockfdSendInfo, &thisPC, sizeof(thisPC), 0, (const struct sockaddr *) &serv_addrServer, sizeof(struct sockaddr_in));

            }

        }



    }



    // Caso estejam: Recebe e armazena posição, envia mensagem de status ao servidor na porta X+posição passando como parâmetro o ip do servidor

	pthread_t tid;
    pthread_create( &tid, NULL ,  sendCurrentStatus, NULL);

    // Loop waiting for user input
    char input[256];
    while(strcmp(input, "EXIT\n"))
    {
    	bzero(input, 256);
    	fgets(input, 256, stdin);
    }

    notExiting = 0;
    sleep(3);
	pthread_exit(NULL);

}


int main(int argc, char *argv[])
{
    notExiting = 1;
    posicao = -1;
    isElecting = 0;
    isServer = 0;

	pcInfo newCon;
	pthread_t tid;
	void *ret;

	pthread_create( &tid, NULL ,  monitoring, NULL);

	//pthread_create( &tid, NULL ,  checkCurrentStatus, &posicao);

    pthread_join(tid, ret);
    printf("waited");
    sleep(1);
    printf("\n %d", posicao);

    return 0;
}










