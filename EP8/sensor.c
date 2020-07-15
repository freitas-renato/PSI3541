// Arquivo: sensor.c
// Utiliza a biblioteca pthreads.
// Incluir arquivo na compilação do programa
// Para compilar:  cc -o <executavel> <modulo.c> sensor-luz.c -lpthread

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "sensor.h"

#define  TAMVETOR 30 

// Vetor de niveis de luminosidade
int       valores[TAMVETOR] = {0,10,20,40,60,80,90,100,100,90,80,60,40,30,20,30,40,50,40,60,80,90,100,90,80,60,40,20,10,0};
int       nivel_sensor = 0;
pthread_t threademulador;

// thread de emulacao do sensor de luminosidade
void emuladorsensor(void) {
    int i = 0;

    while(1) {
       nivel_sensor = valores[i];
       i = (i + 1) % TAMVETOR;
       printf("NIVEL SENSOR: %d\n", nivel_sensor);
       sleep(PERIODO);
    }
}

void init_sensor(void) {
    pthread_create(&threademulador, NULL, (void*) emuladorsensor,NULL);
    // pthread_join(threademulador, NULL);
}

int get_valor_sensor(void) {
    return(nivel_sensor);
}


// Descomentar para testar
//
//int main()
//    { 
//    printf("Programa teste emulador do sensor \n");
//    initsensor();
//    while (1)
//      {
//      sleep (1);
//      printf("%d\n",valorsensor());
//      }
//    pthread_join(threademulador,NULL);
//    printf("Terminado processo Produtor-Consumidor.\n\n");
//    }

