#include <pthread.h>
#include <stdlib.h>

#include "pss.h"
#include "equipo.h"

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
int wait = 0;
char **equipo;
int *state;



char **hay_equipo(char *nombre) {
  pthread_mutex_lock(&m);
  int pos = wait%5;
  wait++;
  //cuando la posicion es 0 crea un nuevo arreglo
  if (pos == 0){
    equipo = malloc(5*sizeof(char *));
    state = (int *)malloc(2*sizeof(int));
    //lleva la cuenta de cuantos jugadores ya se unieron al equipo
    state[0] = 0;
    //lleva la cuenta de cuantos jugadores ya devolvieron el arreglo para liberar state
    state[1] = 5;
  }
  char **equipo_local = equipo;
  int *local_state = state;
  //se van agregando al arreglo
  equipo[pos] = nombre;
  state[0]++;
  //espera a que todos los jugadores se hayan agregado al equipos para hacer el return
  while (local_state[0] < 5 ){
    pthread_cond_wait(&c, &m);
  }
  local_state[1]--;
  //si ya estan listo todo el equipo listo libera el arreglo
  if (local_state[1] == 0){
    free(local_state);
  }
  pthread_cond_broadcast(&c);
  pthread_mutex_unlock(&m);
  return equipo_local;
}

void init_equipo(void) {
}

void end_equipo(void) {
}
