#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

#define HOSTNAMESIZE 30
#define BUFFERSIZE   80


int main() {
    int    sd;                      // socket descriptor
    char   txbuffer[BUFFERSIZE];    // buffer de transmissao
    char   rxbuffer[BUFFERSIZE];    // buffer de recepcao
    char   hostnamep[HOSTNAMESIZE]; // nome do servidor
    void * hostaddress;             // ponteiro p/ endereco IP do servido, formato netwok (4 byes) 
    int    serverport;              // porta UDP do servidor
    int    status;                  // resultado da chamada
    struct hostent   * hostentryp;  // estrutura para traducao de nomes
    struct sockaddr_in serversockaddr;  // socket address do servidor
    char * p;

    printf("Entre com um nome ou um endereço IP: ");
    scanf("%s",hostnamep);

    printf("Porta: ");
    scanf("%d",&serverport);

    // SOCKET
    sd = socket(PF_INET, SOCK_DGRAM, 0); 
    if (sd < 0) {
        perror("ERRO na chamada socket()");
        exit(1); 
    }

    //************************************
    //* Converte nome de host ou endereco IP para network byte order 
    //************************************                          
    
    hostentryp = gethostbyname(hostnamep);
    if (hostentryp != NULL) {
        // conseguiu converter nome em endereco IP: copia os 4 bytes do endereço IP
        hostaddress = hostentryp->h_addr;
        bcopy(hostaddress, (char *)&serversockaddr.sin_addr, hostentryp->h_length);
    } else {
        // Nao conseguiu converter nome para endereco IP
        // Verifica se hostname informado é um endereço IP
    
        status = inet_pton(AF_INET,hostnamep,&serversockaddr.sin_addr);
        if (status <= 0) {
            perror("ERRO: inet_pton()");
            printf("Nome de host ou endereço IP inválido \n");
            return(-1);
        }
    }
        
    //********************************
    // CONECT
    //********************************

    // define endereco do parceiro 
    serversockaddr.sin_family = AF_INET;
    serversockaddr.sin_port   = htons(serverport);

    // ativa connect()
    status = connect(sd, (struct sockaddr *) &serversockaddr, sizeof(serversockaddr));
    if (status != 0) {
        perror("ERRO na chamada connect()");
        exit(1); 
    }
    
    inet_ntop(AF_INET,&serversockaddr.sin_addr,hostnamep,HOSTNAMESIZE)  ;
    printf("Cliente UDP ativo e associado ao servidor %s na porta %d.\n\n", 
            hostnamep, ntohs(serversockaddr.sin_port));

    fgets(txbuffer,sizeof(txbuffer),stdin);
    while (1) {
        printf(">> ");
        fgets(txbuffer,sizeof(txbuffer),stdin);

        status =  send(sd, txbuffer, strlen(txbuffer)+1, 0);
        if (status < 0) {
            perror("Erro na chamada send()");
            //exit(1); 
        }

        status =  recv(sd, rxbuffer, sizeof(rxbuffer), 0);
        if (status < 0) {
            perror("ERRO na chamada recv() ");
            //exit(1); 
        }
        rxbuffer[status+1]='\0';
        printf("\n<< %s",rxbuffer);
    }
}