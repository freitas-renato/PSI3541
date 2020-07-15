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
#include <signal.h>

#include "tcp_server.h"
#include "fila.h"
#include "http_responses.h"
#include "utils.h"

#include "sensor.h"
#include "led_control.h"

#define BUFFERSIZE 15000
#define LINESIZE  80
#define PATHSIZE  80

#define MAXCLIENTS 4

struct fila F;
char basedir[100];


static int server_sd;


int parse_request(int sd, char* req, char* path, char* query) {
    char request[10000];       // buffer temporario
    char header[500];
    char response[1000];
    // char path[500];

    recv(sd, request, sizeof(request), 0);
    // printf ("\nFULL REQUEST: %s\n\n", request);

    // char *full_header = strtok(request, "\n\n\n");
    // printf ("FULL HEADER: %s\n", full_header);
    // full_header = strtok(NULL, "\n\n");
    // printf ("CONTENT: %s\n", full_header);

    char *ptr = strtok(request, "\n");
    strcpy(header, ptr);

    printf("request: %s\n", header);

    char* p = strtok(header, " ");
    p = strtok(NULL, " ");

    // printf("path: %s \n", p);

    char* q = strtok(p, "?");
    q = strtok(NULL, " ");
    printf("query: %s\n", q);
    
    strcpy(path, p);
    strcpy(req, header);
    printf("path: %s \n", path);

    if (q != NULL) {
        strcpy(query, q);
    }


    return 0;
}

void* server_thread(void* arg) {
    // int thread_id = * ((int*)arg);
    // char               msg[15000];       // buffer temporario
    int client_sd;
    int msg_size = 0;
    char response[1000];
    char request[10];
    char path[100];
    char query[100];
    char tmp[512];

    char _resp[100];

    // int server_sd = *((int*)arg);

    // printf("Thread %d iniciada\n", thread_id);



    for (;;) {
        if ( (client_sd = accept(server_sd, NULL, NULL)) < 0) {
            // printf("accept failed \n");
            perror("accept failed \n");
            // exit(1);
        } 

        // client_sd = RetirarFila(&F);

        if (client_sd != 0) {
            
            parse_request(client_sd, request, path, query);

            if (strcmp(request, "GET") == 0) {
                if (strcmp(path, "/") == 0) {
                    strcpy(tmp, basedir);
                    append(tmp, sizeof(tmp), "index.html");
                    printf("File path: %s\n", tmp);

                    if (!send_response(tmp, client_sd)) {
                        // transferfile(response, client_sd);
                        not_found(response);
                        send(client_sd, response, strlen(response), 0);
                    }

                    set_op_mode(STANDBY);

                } else if (strcmp(path, "/luminosidade") == 0) {
                    ok(_resp, "text/html");
                    write(client_sd, _resp, strlen(_resp));
                    fsync(client_sd);
                    bzero(_resp, sizeof(_resp));

                    sprintf(_resp, "%d", get_ambient_luminosity());
                    write(client_sd, _resp, strlen(_resp));
                    fsync(client_sd);
                    bzero(_resp, sizeof(_resp));


                } else if (strcmp(path, "/opmode") == 0) {
                    set_op_mode(atoi(query));
                    printf("Opmode set: %d\n", get_op_mode());

                    ok(_resp, "text/html");
                    write(client_sd, _resp, strlen(_resp));
                    fsync(client_sd);
                    bzero(_resp, sizeof(_resp));
                    sprintf(_resp, "OP mode set to: %d \r\n", get_op_mode());
                    write(client_sd, _resp, strlen(_resp));
                    fsync(client_sd);
                    bzero(_resp, sizeof(_resp));

                } else if (strcmp(path, "/intensity") == 0) {
                    set_led_intensity(atoi(query));

                    printf("Intensity set: %d\n", get_led_intensity());

                    ok(_resp, "text/html");
                    write(client_sd, _resp, strlen(_resp));
                    fsync(client_sd);
                    bzero(_resp, sizeof(_resp));
                    sprintf(_resp, "Intensity set to: %d \r\n", get_led_intensity());
                    write(client_sd, _resp, strlen(_resp));
                    fsync(client_sd);
                    bzero(_resp, sizeof(_resp));
                }
                
                else {
                    strcpy(tmp, basedir);
                    append(tmp, sizeof(tmp), path+1);
                    printf("File path: %s\n", tmp);

                    if (access(tmp, F_OK) != -1) {
                        if (!send_response(tmp, client_sd)) {
                            not_found(response);
                            send(client_sd, response, strlen(response), 0);
                        }
                    } else {
                        not_found(response);
                        send(client_sd, response, strlen(response), 0);
                    }

                }
            } else {
                bad_request(response);
                send(client_sd, response, strlen(response), 0);
            }
           
            bzero(response, sizeof(response));
            bzero(request, sizeof(request));
            bzero(path, sizeof(path));
            bzero(tmp, sizeof(tmp));

            response[0] = '\0';
            tmp[0] = '\0';

            close(client_sd);
            printf("Response sent, closing socked descriptor\n\n");
            // client_sd = 0;
        }

        // close(client_sd);
        // sleep(1);
    }
}

void sig_handler(int  sig) {
    //
}


pthread_t init_server(void) {

    char               buffer[80]; // buffer temporario
    char               msg[512];       // buffer temporario
    
    // int server_sd;
    int server_port;         // porta (formato host)
    struct sockaddr_in server_socket_address;
    struct sockaddr_in fromaddr;
    // struct hostent   * hostentryp;         // host entry (p/ traducao nome<->ip)
    // struct servent   * serventryp;         // host entry (p/ traducao nome<->ip)

    struct sockaddr_in client_socket_address;
    int client_sd;
    int addr_len;

    // pthread_t s_thread[MAXCLIENTS];
    pthread_t s_thread;
    pthread_t client_accept;

    char* cfg_file = "./server.cfg";
    int port;
    // char basedir[100];

    signal(SIGSEGV, sig_handler);

    
    InitFila(&F, MAXCLIENTS);

    /**< Server Address and Port setup */
    bzero(&server_socket_address, sizeof(server_socket_address));
    server_socket_address.sin_family        = AF_INET;
    server_socket_address.sin_addr.s_addr   = htonl(INADDR_LOOPBACK);
    // server_socket_address.sin_port       = htons(SERVER_PORT);

    
    FILE *fd = fopen(cfg_file, "r+");

    fscanf(fd, "%s", buffer);
    fscanf(fd, "%d", &server_port);

    fscanf(fd, "%s", buffer);
    fscanf(fd, "%s", basedir);

    printf("Config: %d \n%s \n\n", server_port, basedir);
    fflush(fd);
    fclose(fd);

    lista_diretorio(basedir, msg, sizeof(msg));

    printf("Diretorio: %s\n", msg);

    if (server_port > 65535) {
        printf("Valor de porta invalido. \n");
        exit(-1);
    } else if (server_port > 0) {
        server_socket_address.sin_port = htons((in_port_t) server_port);
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


    addr_len = sizeof(client_socket_address);

    if (listen(server_sd, 10) < 0) {
        perror("Erro na chamada listen\n");
        exit(-1);
    }

    printf("Servidor: esperando conexão... \n");

    pthread_create(&s_thread, NULL, server_thread, NULL);
    pthread_join(s_thread, NULL);


    return s_thread;

}

