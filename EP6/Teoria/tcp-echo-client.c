//###############################################################
//
// Programa tcp-echo-client.c
//
//###############################################################

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <strings.h>
#include <stdlib.h>

extern int    errno;

#define IPMAXSTRSIZE   16   

//###############################################################
// openActiveTCPport
//     Descricao  
//         Abre uma porta UDP no modo ativo
//     Argumentos 
//         hostnamep   : nome do host ou endereco ip 
//         servicenamep: nome do servico ou numero da porta 
//     Retorno    
//         sucesso: socket descriptor
//         erro   : < 0
//###############################################################

int openActiveTCPport(char *hostnamep, char *servicenamep)
    {
    int                 serverport;
    unsigned short int  netbyteordered_serverport;
    unsigned long  int  ipaddr;
    int                 socketdescriptor;
    int                 status;
    char                ipstring[IPMAXSTRSIZE];
    struct sockaddr_in  socketaddress;
    struct servent    * serviceentryp;
    struct hostent    * hostentryp;

    /************************************/
    /* Obtem o numero da porta do servidor */
    /************************************/
   
    serviceentryp = getservbyname(servicenamep,"tcp");
    if (serviceentryp == NULL)
        {
        // Nao conseguiu converter o nome do servico para numero de porta.
	// Verifica se foi informado numero da porta */
        serverport = atoi(servicenamep);
        if (serverport <= 0)
            {
            printf("Nome do servico (ou porta) invalido. \n");
            return(-1);
            }
        netbyteordered_serverport = htons(serverport);
        }
    else
        {
        serverport = ntohs(serviceentryp->s_port);
        if (serverport <= 0)
            {
            printf("Numero de porta invalida. \n");
            return(-1);
            }
        netbyteordered_serverport = htons(serverport);
        }
    printf("Porta do servidor    = %hu \n",serverport);

    /************************************/
    /* Converte endereco IP              */
    /************************************/
  
    hostentryp = gethostbyname(hostnamep); 
    if (hostentryp == NULL)
        {
	// Nao conseguiu converter nome para endereco IP
        // Verifica se hostname informado � um endere�o IP
        status = inet_pton(AF_INET,hostnamep,&socketaddress.sin_addr);
        if (status <=0)
            {
            perror("ERRO: intet_pton()");
            return(-1);
            }
        }
    else
        {
        bcopy(hostentryp->h_addr, (char *)&socketaddress.sin_addr, hostentryp->h_length);
        }
     inet_ntop(AF_INET,&socketaddress.sin_addr,ipstring,IPMAXSTRSIZE);
     printf("Endereco IP do servidor = %s \n",ipstring);

    /****************************************************************/
    /* Socket(): Criacao do socket                                  */
    /****************************************************************/
    socketdescriptor = socket(PF_INET,SOCK_STREAM,6); // 6="tcp"
    if (socketdescriptor < 0)
        {
        printf("Erro na cria��o do socket. \n");
        perror("Descricao do erro");
        return (-1);
        }

    /****************************************************************/
    /* connect(): */
    /****************************************************************/
    socketaddress.sin_family = AF_INET;  /* address family = internet */
    socketaddress.sin_port   = netbyteordered_serverport;   /* porta */

    status = connect(socketdescriptor,(struct sockaddr*)&socketaddress,sizeof(socketaddress));
    if (status != 0)
        {
        printf("Erro na chamada Connect().\n");
        perror("Descricao do erro");
        return(-1);
        }
    return(socketdescriptor);
    }


int main()
	{
        int  sd;
        char buffer[15000];
        int  status;
	int  n;

        char hostnamep[80];
	char servicenamep[80];

	printf("HOSTNAME: ");
	scanf("%s",hostnamep);

	printf("SERVICENAME: ");
	scanf("%s",servicenamep);

        sd = openActiveTCPport(hostnamep, servicenamep);
        if (sd < 0)
           {
           printf("ERRO no estabelecimento da conex�o TCP \n");
           exit(1); 
           }
        
        fgets(buffer,80,stdin); //descarta lixo
        while (1)
           {
           printf(">> ");
           fgets(buffer,80,stdin);
           buffer[strlen(buffer)-1]='\r';
           buffer[strlen(buffer)-1]='\n';
           buffer[strlen(buffer)-1]='\0';


           status =  send(sd, buffer, 80, 0);
           if (status < 0)
              {
              printf("ERRO no envido de mensagens\n");
              exit(1); 
              }
            bzero(buffer, sizeof(buffer));

           n =  recv(sd, buffer, 15000, 0);
           if (n < 0)
              {
              printf("ERRO no envido de datagramas UDP \n");
              exit(1); 
              }

            // while (fgets(tmp, sizeof(tmp), fp) != NULL) {
            //     write(*sd, tmp, sizeof(tmp));
            // }
            // printf(">>> %s \n",buffer);
            fputs(buffer, stdout);
            // printf(">>> ");
            // fwrite(buffer, sizeof(buffer), 1, stdout);
           }        
	}