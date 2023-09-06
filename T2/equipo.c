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
int actual = 0;
int estado = 0, eFaltante =5;



char **hay_equipo(char *nombre) {
  pthread_mutex_lock(&m);
  int equipoAct = wait/5;
  int pos = wait%5;
  int my_num = wait++;
  // pasan en orden 
  while (my_num != actual && equipoAct != equipos){
    pthread_cond_wait(&c, &m);
  }
  //el primero crea el arreglo
  if (pos == 0){
    char **team = malloc(5*sizeof(char *));
    equipo = team;
    estado = 0;
    eFaltante = 5;
  }
  //se van agregando al arreglo
  equipo[pos] = nombre;
  actual++;
  pthread_cond_broadcast(&c);
  char **equipo_local = equipo;
  estado++;
  pthread_mutex_unlock(&m);
  //chequear que avance
  printf("En el jugador: %d \n", my_num);
  pthread_mutex_lock(&m);
  //espera a que todos los jugadores se hayan agregado al equipos para hacer el return
  while (estado < 5){
    pthread_cond_wait(&c, &m);
  }
  eFaltante--;
  //si ya estan listo todos los equipos permite la entrada del resto
  if (eFaltante == 0){
    equipos++;
  }
  pthread_cond_broadcast(&c);
  pthread_mutex_unlock(&m);
  return equipo_local;
}

void init_equipo(void) {
  equipo = malloc(sizeof(char **));
}

void end_equipo(void) {
  free(equipo);
}
