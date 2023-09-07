#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "pss.h"
#include "equipo.h"

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
int wait = 0, equipos = 0;
//Buffer de equipos
char **equipo;
int estado = 0, eFaltante =5;



char **hay_equipo(char *nombre) {
  pthread_mutex_lock(&m);
  int equipoAct = wait/5;
  int pos = wait%5;
  wait++;
  // pasan solo si les corresponde armar el equipo
  while (equipoAct > equipos){
    pthread_cond_wait(&c, &m);
  }
  char **equipo_local = equipo;
  //se van agregando al arreglo
  equipo[pos] = nombre;
  estado++;
  //espera a que todos los jugadores se hayan agregado al equipos para hacer el return
  while (estado < 5 ){
    pthread_cond_wait(&c, &m);
  }
  eFaltante--;
  //si ya estan listo todos los equipos permite la entrada del resto
  if (eFaltante == 0){
    equipo = malloc(5*sizeof(char *));
    equipos++;
    estado = 0;
    eFaltante = 5;
    //printf("%d \n", equipos);
  }
  pthread_cond_broadcast(&c);
  pthread_mutex_unlock(&m);
  return equipo_local;
}

void init_equipo(void) {
  equipo = malloc(5*sizeof(char *));
}

void end_equipo(void) {
  free(equipo);
}
