/**
 *  @file: tcp_server.c
 *  @brief: Servidor concorrente TCP
            
 *  @note: para compilar:  gcc -o tcp_server tcp_server.c -lpthread
 * 
 *  @author: Isabella Bologna - 9267161
 *  @author: Renato Freitas   - 9837708
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

#include <pthread.h>

#include <sys/stat.h>
#include <fcntl.h>

#include "fila.h"

#define BUFFERSIZE 15000
#define LINESIZE  80
#define PATHSIZE  80

#define MAXCLIENTS 4

struct fila F;

int transferfile(char *path, int output_fd) {
    int          input_fd;     // input file descriptor
    int          status;
    int          n;
    char         buffer[BUFFERSIZE];
    char         str[10];
    struct stat  statp;
   
    input_fd = open(path,O_RDONLY);
    if (input_fd < 0) {
         perror("ERRO chamada open(): Erro na abertura do arquivo: ");
        return(-1);
    }

    // obtem tamanho do arquivo
    status = fstat(input_fd,&statp);
    if (status != 0) {
        perror("ERRO chamada stat(): Erro no acesso ao arquivo: ");
        status = close(input_fd);
        return(-1);
    }
    // sprintf(str,"SIZE=%d\n",statp.st_size);
    // write(output_fd,str,strlen(str));

    // le arquivo , por partes 
    do {
        n = read(input_fd,buffer,BUFFERSIZE);
        if (n < 0) {
            perror("ERRO: chamada read(): Erro na leitura do arquivo: ");
            status = close(input_fd);
            return(-1);
        }
        write(output_fd,buffer,n);
    } while(n > 0);
    // write(output_fd, "\n", 1);

    status = close(input_fd);
   
    if (status == -1)  {
            perror("ERRO: chamada close(): Erro no fechamento do arquivo: " );
            return(-1);
        }

    bzero(buffer, BUFFERSIZE);

    return(0);
}

void parse_msg(int *sd, char* msg) {
    char output_msg[1024];
    char tmp[15000];
    
    
    if (strcmp(msg, "date") == 0) {
        FILE* fp = popen("date", "r");
        while (fgets(tmp, sizeof(tmp), fp) != NULL) {
            write(*sd, tmp, sizeof(tmp));
        }
        bzero(tmp, sizeof(tmp));
        pclose(fp);

    } else if (strcmp(msg, "version") == 0) {
        transferfile("/proc/version", *sd);

    } else if (strcmp(msg, "cpu") == 0) {
        transferfile("/proc/cpuinfo", *sd);

    } else if (strcmp(msg, "memory") == 0) {
        transferfile("/proc/meminfo", *sd);

    } else if (strcmp(msg, "partitions") == 0) {
        transferfile("/proc/partitions", *sd);

    } else if (strcmp(msg, "interfaces") == 0) {
        FILE* fp = popen("ifconfig", "r");
        while (fgets(tmp, sizeof(tmp), fp) != NULL) {
            write(*sd, tmp, sizeof(tmp));
        }
        bzero(tmp, sizeof(tmp));
        pclose(fp);

    } else if (strcmp(msg, "route") == 0) {
        transferfile("/proc/net/route", *sd);

    } else if (strcmp(msg, "exit") == 0){
        strcpy(output_msg, "Servidor encerrando a conexão\n");
        write(*sd, output_msg, sizeof(output_msg));
        close(*sd);
        *sd = -1;

    } else { 
        strcpy(output_msg, "Comando invalido\n");
        write(*sd, output_msg, sizeof(output_msg));
    }

    bzero(output_msg, sizeof(output_msg));
}

void* server_thread(int thread_id) {
    char               msg[512];       // buffer temporario
    int client_sd;
    printf("Thread %d iniciada\n", thread_id);

    for(;;) {
        client_sd = RetirarFila(&F);

        while (client_sd != -1) {
            read(client_sd, msg, sizeof(msg));
            printf("Message from client %d: %s\n", thread_id, msg);

            parse_msg(&client_sd, msg);
           
            bzero(msg, sizeof(msg));
            sleep(1);
        }

        // close(client_sd);
        sleep(1);
    }
}


int main(void) {

    char               buffer[80]; // buffer temporario
    char               msg[512];       // buffer temporario
    
    int server_sd;
    int server_port;         // porta (formato host)
    struct sockaddr_in server_socket_address;
    struct sockaddr_in fromaddr;
    struct hostent   * hostentryp;         // host entry (p/ traducao nome<->ip)
    struct servent   * serventryp;         // host entry (p/ traducao nome<->ip)

    struct sockaddr_in client_socket_address;
    int client_sd;
    int addr_len;

    pthread_t s_thread;

    InitFila(&F, MAXCLIENTS);

    server_socket_address.sin_family = AF_INET;
    server_socket_address.sin_addr.s_addr = INADDR_ANY;

    printf("Digite a porta para o servidor: ");
    scanf("%s", buffer);
    server_port = atoi(buffer);

    if (server_port > 65535) {
        printf("Valor de porta invalido. \n");
        exit(-1);
    } else if (server_port > 0) {
        server_socket_address.sin_port = htons((in_port_t) server_port);
    } else {
        if ((serventryp = getservbyname(buffer, "tcp")) != NULL) {
            server_socket_address.sin_port = serventryp->s_port;
        } else {
            printf("Nome do servico (ou porta) invalido. \n");
        }
    }

    

    printf("IP do servidor: %s\n", inet_ntop(AF_INET, &server_socket_address.sin_addr, buffer, sizeof(buffer)));
    printf("Porta do servidor: %hu\n", ntohs(server_socket_address.sin_port));


    if ((server_sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Erro na criação do socket \n");
        exit(-1);
    }

    if ((bind(server_sd, (struct sockaddr*)&server_socket_address, sizeof(struct sockaddr_in))) < 0) {
        printf("Erro na chamada bind()\n");
        exit(-1);
    }

    for (int i = 0; i < MAXCLIENTS; i ++) {
        pthread_create(&s_thread, NULL, server_thread, i);
    }

    addr_len = sizeof(client_socket_address);

    for (;;) {
        if (listen(server_sd, 6) < 0) {
            printf("Erro na chamada listen\n");
            exit(-1);
        }
        printf("Servidor: esperando conexão... \n");
        


        if ( (client_sd = accept(server_sd, (struct sockaddr*) &client_socket_address, &addr_len)) < 0) {
            printf("accept failed \n");
            exit(1);
        } else { 
            printf("client accepted\n");

            InserirFila(&F, client_sd);
        }
    }

    close(server_sd);

    return 0;
}

