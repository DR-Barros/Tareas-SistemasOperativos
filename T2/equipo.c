#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "pss.h"
#include "equipo.h"

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
int wait = 0;
Queue *q;


char **hay_equipo(char *nombre) {
  char **equipoLocal = malloc(5*sizeof(char *));
  printf("%d \n", wait);
  pthread_mutex_lock(&m);
  char **e = get(q);
  for (int i = 0; i < 5; i++){
    if(e[i] == NULL){
      e[i] = nombre;
      put(q, e);
      break;
    }
  }


  pthread_mutex_unlock(&m);
  return equipoLocal;
}

void init_equipo(void) {
  char **team = malloc(5*sizeof(char *));
  for (int i = 0; i < 5; i++){
    team[i] = NULL;
  }
  q = makeQueue();
  put(q, team);
}

void end_equipo(void) {
  destroyQueue(q);
}
