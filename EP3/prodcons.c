/**
 *  @file: prodcons.c
 *  @brief: Programa produtor-consumidor com semáforos
            Utiliza a biblioteca pthreads.
 *  @note: para compilar:  gcc -o prodcons fila.c prodcons.c -lpthread
 * 
 *  @author: Isabella Bologna - 9267161
 *  @author: Renato Freitas   - 9837708
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#include "fila.h"

#define  FALSE 0
#define  TRUE  1


// Fila
struct fila F;


void produtor(int id) {
    int item;

    printf("| Inicio produtor %d \t\t | \t\t\t\t\t | \t| \n", id);

    /**< Produz 10 itens */
    for (int i = 0; i < 10; i++) {
        //* produzir item
        item = i + (id*1000);

        /**< Inserir item na fila */
        InserirFila(&F, item);

        sleep(2);
    }

    printf("| Produtor %d terminado \t\t | \t\t\t\t\t | \t| \n", id);
}

void consumidor(int id) {
    int item;

    printf("| \t\t\t\t | Inicio consumidor %d \t\t\t | \t| \n", id);
    
    /**< Loop infinito, consumidor nunca acaba */
    /**< Retira itens da fila */
    while (1) {
        item = RetirarFila(&F);

        // processar item
        sleep(2);
    }

    printf("| \t\t\t\t | Consumidor %d terminado \t\t | \t| \n", id);
}

int main(void) {
    pthread_t prod1;
    pthread_t prod2;
    pthread_t prod3;
    pthread_t cons1;
    pthread_t cons2;

    printf("Programa Produtor-Consumidor\n");

    printf("Iniciando fila\n");
    InitFila(&F);

    printf("\n| \t   Produtores \t\t | \t\t Consumidores \t\t | Fila\t|\n");
    printf("|--------------------------------|---------------------------------------|------|\n");

    // printf("Disparando threads produtores\n");
    pthread_create(&prod1, NULL, (void*) produtor,1);
    pthread_create(&prod2, NULL, (void*) produtor,2);
    pthread_create(&prod3, NULL, (void*) produtor,3);

    // printf("Disparando threads consumidores\n");
    pthread_create(&cons1, NULL, (void*) consumidor,1);
    pthread_create(&cons2, NULL, (void*) consumidor,2);

    pthread_join(prod1,NULL);
    pthread_join(prod2,NULL);
    pthread_join(prod3,NULL);

    pthread_join(cons1,NULL);
    pthread_join(cons2,NULL);

    printf("Terminado processo Produtor-Consumidor.\n\n");

    return 0;
}
