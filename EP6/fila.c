/**
 *  @file: fila.c
 *  @brief: Biblioteca de fila com recursos gerenciados através de semáforos
 * 
 *  @author: Isabella Bologna - 9267161
 *  @author: Renato Freitas   - 9837708
*/

#include <stdio.h>

#include "fila.h" 

#define  FALSE 0
#define  TRUE  1

void InitFila(struct fila *F, int max_size) {
    F->inicio = 0;
    F->fim    = 0;
    F->nitens = 0;
    F->tammax = max_size;

    sem_init(&F->mutex_semaphore, 0, 1);
    sem_init(&F->slots_semaphore, 0, F->tammax);
    sem_init(&F->itens_semaphore, 0, 0);
}

int FilaVazia(struct fila *F) {
    if (F->nitens == 0)
        return(TRUE);
    else
        return(FALSE);
}

int FilaCheia(struct fila *F) {
    if (F->nitens == F->tammax)
        return(TRUE);
    else
        return(FALSE);
}

void InserirFila(struct fila *F, int item) {

    sem_wait(&F->slots_semaphore);
    sem_wait(&F->mutex_semaphore);
    
    F->buffer[F->fim] = item;
    F->fim = (F->fim + 1) % F->tammax;
    F->nitens = F->nitens + 1;
    
    printf("| Produtor %d inserindo item %d | \t\t\t\t\t |  %d\t| \n", item/1000, item, F->nitens); 

    sem_post(&F->mutex_semaphore);
    sem_post(&F->itens_semaphore);
}

int RetirarFila(struct fila *F) {
    int item;

    sem_wait(&F->itens_semaphore);
    sem_wait(&F->mutex_semaphore);

    item = F->buffer[F->inicio];
    F->inicio = (F->inicio + 1) % F->tammax;
    F->nitens = F->nitens - 1;

    printf("| \t\t\t\t | Consumidor %d consumiu item %d \t |  %d\t| \n", item/1000, item, F->nitens); 

    sem_post(&F->mutex_semaphore);
    sem_post(&F->slots_semaphore);

    return(item);
}
