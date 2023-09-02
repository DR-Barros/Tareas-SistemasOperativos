#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "pss.h"
#include "equipo.h"

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
int wait = 0, equipos = 0;
char ***equipo;



char **hay_equipo(char *nombre) {
  pthread_mutex_lock(&m);
  int equipoAct = wait/5;
  int pos = wait%5;
  if (pos == 0){
    equipo[equipoAct] = malloc(5* sizeof(char *));
  }
  equipo[equipoAct][pos] = nombre;
  if (pos == 4){
    wait++;
    equipos++;
    pthread_cond_broadcast(&c);
  } else{
    wait++;
    while (equipoAct == equipos){
      pthread_cond_wait(&c, &m);
    }
  }
  pthread_mutex_unlock(&m);
  return equipo[equipoAct];
}

void init_equipo(void) {
  equipo = malloc(1000000 *sizeof(char **));
}

void end_equipo(void) {
  free(equipo);
}
