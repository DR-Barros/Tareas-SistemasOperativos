#include <stdio.h>
#include <pthread.h>

#include "integral.h"

// Defina aca las estructuras y funciones que necesite
// ...
typedef struct {
  Funcion f;
  void *ptr;
  double i, j;
  int n;
  double res;
} Args;

void *thread_function(void *p){
  Args *arg = (Args *)p;
  arg->res = integral(arg->f,  arg->ptr, arg->i, arg->j, arg->n);
  return NULL;
}

double integral_par(Funcion f, void *ptr, double xi, double xf, int n, int p) {
  // Programe aca la funcion integral_par solicitada
  // ...

  //creamos variables para calcular las integrales
  Args args[p];
  pthread_t pid[p];
  double interval = (xf-xi)/p;
  double res = 0;
  
  for (int i = 0; i < p; i++){
    //asignamos valores a los argumentos
    args[i].f = f;
    args[i].ptr = ptr; 
    args[i].i = xi+i*interval; 
    args[i].j = xi+(i+1)*interval; 
    args[i].n = n/p; 
    args[i].res = 0; 
    //creamos los threads
    pthread_create(&pid[i], NULL, thread_function, &args[i]);
  } 
  //Una  vez los threads creados podemos extraer los resultados y matar el thread
  for (int i = 0; i < p; i++){
    pthread_join(pid[i], NULL);
    res+= args[i].res;
  }

  return res;
}

