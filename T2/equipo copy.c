#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "pss.h"
#include "equipo.h"

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
int wait = 0, equipos = 0;
//Buffer de equipos
char ***equipo;



char **hay_equipo(char *nombre) {
  pthread_mutex_lock(&m);
  int equipoAct = wait/5;
  int pos = wait%5;
  wait++;
  while (equipoAct > equipos){
    pthread_cond_wait(&c, &m);
  }
  if (pos == 0){
    equipo[equipoAct%5000] = malloc(5* sizeof(char *));
  }
  equipo[equipoAct%5000][pos] = nombre;
  if (pos == 4){
    
    equipos++;
    pthread_cond_broadcast(&c);
  } else{
    while (equipoAct == equipos){
      pthread_cond_wait(&c, &m);
    }
  }
  pthread_mutex_unlock(&m);
  return equipo[equipoAct%5000];
}

void init_equipo(void) {
  //equipo = malloc(5000 *sizeof(char **));
}

void end_equipo(void) {
  //free(equipo);
}
