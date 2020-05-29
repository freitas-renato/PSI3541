/**
 *  @file: main.c
 *  @brief: Decodificador simplificado de URL
            Utiliza a biblioteca netdb.
 *  @note: para compilar:  gcc -o main main.c
 * 
 *  @author: Isabella Bologna - 9267161
 *  @author: Renato Freitas   - 9837708
*/

#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>

int main(void) {
    char input[100];

    char scheme[20];
    char domain[80];
    int port;
    char path[100];

    struct hostent *host_entry;
    struct servent *serv_entry;
    struct protoent *prot_entry;


    printf("Digite a url no formato \"esquema://dominio:porta/caminho\": ");
    scanf("%s", input);

    if (sscanf(input, "%[^:]://%[^:]:%d/%s", scheme, domain, &port, path) != 4) {
        printf("URL no formato inválido!\n");
        printf("Formato correto: \"esquema://dominio:porta/caminho\"\n");

        exit(-1);
    }

    // printf("%s\n%s\n%d\n%s\n", scheme, domain, port, path);

    host_entry = gethostbyname(domain);
    serv_entry = getservbyname(scheme, "tcp");


    if (host_entry == NULL){
        printf("Host nao encontrado.\n");
        exit(-1);
    } else if (serv_entry == NULL) {
        printf("Protocolo invalido.\n");
        exit(-1);
    } else {
        printf("\nProtocolo: %s (%d)\n", scheme, ntohs(serv_entry->s_port));
        printf("Nome do host: %s\n", host_entry->h_name);
        printf("Endereço IP do host: %d.%d.%d.%d\n", (uint8_t)host_entry->h_addr[0], (uint8_t)host_entry->h_addr[1], (uint8_t)host_entry->h_addr[2], (uint8_t)host_entry->h_addr[3]);
        printf("Porta: %d\n", port);
        printf("Caminho: %s\n", path);
    }

    return 0;
}
