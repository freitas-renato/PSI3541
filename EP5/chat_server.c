/**
 *  @file: chat_server.c
 *  @brief: Servidor chat UDP
            
 *  @note: para compilar:  gcc -o chat_server chat_server.c
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

#define MAXUSERS 4

typedef struct user {
    char name[11];
    struct sockaddr_in user_address;
} user_t;

typedef struct user_table {
    int state[MAXUSERS];
    user_t registered_users[MAXUSERS];
    int users_count;
} user_table_t;

typedef enum _resp {
    OKOK = 0,
    BUSY,
    BYE
} response_t;


user_table_t main_user_table;
user_t null_user = { 0 };


int register_user(char* username, struct sockaddr_in user_addr) {
    user_t new_user;
    
    if (main_user_table.users_count == MAXUSERS) {
        return BUSY;
    }

    strcpy(new_user.name, username);
    new_user.user_address = user_addr;

    for (int i = 0; i < MAXUSERS; i++) {
        if (main_user_table.state[i] == OKOK) {
            main_user_table.registered_users[i] = new_user;
            main_user_table.state[i] = BUSY;
            main_user_table.users_count++;

            return OKOK;
        }
    }

    return BUSY;
}

int remove_user(struct sockaddr_in user_addr) {
    for (int i = 0; i < MAXUSERS; i++) {
        if (main_user_table.registered_users[i].user_address.sin_port == user_addr.sin_port) {
            main_user_table.registered_users[i] = null_user;
            main_user_table.state[i] = OKOK;
            main_user_table.users_count--;

            return OKOK;
        }
    }

    return BUSY;
}

void print_current_users() {
    printf("Lista de usuários: ");
    for (int i = 0; i < MAXUSERS; i++) {
        printf("%s  |  ", main_user_table.registered_users[i].name);
    }
    printf("\n\n");
}

void write_response(int sd, struct sockaddr_in client_addr, response_t resp) {
    char msg[6];
    switch (resp) {
        case OKOK:
            strcpy(msg, "OKOK:");
        break;

        case BUSY:
            strcpy(msg, "BUSY:");
        break;

        case BYE:
            strcpy(msg, "BYE :");
        break;

        default:
        break;
    }

    sendto(sd, msg, strlen(msg)+1, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
}

void send_message(int sd, struct sockaddr_in origin, char* msg) {
    char down_msg[100];
    char _username[11];
    
    for (int i = 0; i < MAXUSERS; i++) {
        if (main_user_table.registered_users[i].user_address.sin_port == origin.sin_port) {
            strcpy(_username, main_user_table.registered_users[i].name);
            // _username[11] = '\0';
            sprintf(down_msg, "DOWN:%s: %s\n", _username, msg);
            printf("MENSAGEM:  %s\n", down_msg);
            break;
        }  
    }

    for (int i = 0; i < MAXUSERS; i++) {
        if (main_user_table.state[i] == BUSY && main_user_table.registered_users[i].user_address.sin_port != origin.sin_port) {
            sendto(sd, down_msg, strlen(down_msg)+1, 0, (struct sockaddr *)&main_user_table.registered_users[i].user_address, sizeof(main_user_table.registered_users[i].user_address));
        }
    }
}


void parse_msg(int sd, char* msg, struct sockaddr_in client_addr) {
    char command[5];
    char username[11];
    char message_text[80];

    // Pega o comando mandado
    strncpy(command, msg, 4);
    command[4] = '\0';
    printf("Command: %s\n", command);


    if (strcmp(command, "USER") == 0) {
        // Adiciona usuário na tabela
        strcpy(username, msg+sizeof(command));
        printf("username: %s\n", username);
        
        if (register_user(username, client_addr) == OKOK) {
            printf("Usuario registrado com sucesso\n");
            printf("Numero de usuarios: %d\n", main_user_table.users_count);
            print_current_users();
            write_response(sd, client_addr, OKOK);
        
        } else {
            printf("Numero máximo de usuários atingido!\n");
            printf("Numero de usuarios: %d\n", main_user_table.users_count);
            print_current_users();
            write_response(sd, client_addr, BUSY);
        }

    } else if (strcmp(command, "TEST") == 0) {
        // Responde teste com OKOK
        write_response(sd, client_addr, OKOK);
    
    } else if (strcmp(command, "UP  ") == 0) {
        // Manda mensagem para todos os outros usuários
        send_message(sd, client_addr, msg+sizeof(command));

    } else if (strcmp(command, "EXIT") == 0) {
        // Remover user da lista
        if (remove_user(client_addr) == OKOK) {
            printf("Usuário removido\n");
            printf("Numero de usuarios: %d\n", main_user_table.users_count);
            print_current_users();
            write_response(sd, client_addr, BYE);
        }
    }
}


int main(void) {
    int                sd;                 // socket descriptor  
    int                status;             // estado
    int                size;

    char               buffer[80]; // buffer temporario
    char               msg[512];       // buffer temporario

    int server_port;         // porta (formato host)
    struct sockaddr_in server_socket_address;
    struct sockaddr_in fromaddr;
    struct hostent   * hostentryp;         // host entry (p/ traducao nome<->ip)
    struct servent   * serventryp;         // host entry (p/ traducao nome<->ip)

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
        if ((serventryp = getservbyname(buffer, "udp")) != NULL) {
            server_socket_address.sin_port = serventryp->s_port;
        } else {
            printf("Nome do servico (ou porta) invalido. \n");
        }
    }

    printf("IP do servidor: %s\n", inet_ntop(AF_INET, &server_socket_address.sin_addr, buffer, sizeof(buffer)));
    printf("Porta do servidor: %hu\n", ntohs(server_socket_address.sin_port));

    
    if ((sd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Erro na criação do socket \n");
        exit(-1);
    }

    if ((bind(sd, (struct sockaddr*)&server_socket_address, sizeof(struct sockaddr_in))) < 0) {
        printf("Erro na chamada bind()\n");
        exit(-1);
    }



    for (;;) {
        size = sizeof(fromaddr);
        status = recvfrom(sd, msg, 512, 0, (struct sockaddr*)&fromaddr, &size);
        if (status < 0) {
            printf("Erro no recebimento de datagramas UDP\n");
            exit(-1);
        }

        parse_msg(sd, msg, fromaddr);
    }
    return 0;
}