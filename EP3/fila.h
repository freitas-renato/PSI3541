/**
 *  @file: fila.h
 *  @brief: Biblioteca de fila com recursos gerenciados através de semáforos
 * 
 *  @author: Isabella Bologna - 9267161
 *  @author: Renato Freitas   - 9837708
*/

#include <semaphore.h>

#define TAMMAXFILA 4

struct fila {
    int buffer[TAMMAXFILA];
    int inicio;
    int fim;
    int nitens;
    int tammax;

    sem_t mutex_semaphore;
    sem_t slots_semaphore;
    sem_t itens_semaphore;
};

void InitFila(struct fila *F);
int  FilaVazia(struct fila *F);
int  FilaCheia(struct fila *F);
void InserirFila(struct fila *F, int item);
int  RetirarFila(struct fila *F);
