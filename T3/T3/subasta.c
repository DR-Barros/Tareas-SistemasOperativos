#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "subasta.h"

// =============================================================
// Use esta cola de prioridades para resolver el problema
// =============================================================

// Puede almacenar hasta 100 elementos.  No se necesita mas para el test.

#define MAXQSZ 100

typedef struct {
  void **vec;
  double *ofertas;
  int size, maxsize;
} *PriQueue;

PriQueue MakePriQueue(int size);    // Constructor
void DestroyPriQueue(PriQueue pq);  // Destructor

// Las operaciones
void *PriGet(PriQueue pq);          // Extraer elemento de mejor prioridad
void PriPut(PriQueue pq, void* t, double pri); // Agregar con prioridad
double PriBest(PriQueue pq);           // Prioridad del mejor elemento
int EmptyPriQueue(PriQueue pq);     // Verdadero si la cola esta vacia
int PriLength(PriQueue pq);         // Largo de la cola

// =============================================================
// Implemente a partir de aca el tipo Subasta
// =============================================================

struct subasta {
  int unidades;
  pthread_mutex_t m;
  PriQueue q;
  int terminado;
};

//... programe aca las funciones solicitadas: nuevaSubasta, adjudicar, etc. ...

Subasta nuevaSubasta(int unidades){
  //crear una subasta en que se puedan ir agregando los n elementos
  Subasta subasta = malloc(sizeof(Subasta));
  subasta->unidades;
  subasta ->q = MakePriQueue(subasta);
  subasta ->terminado = FALSE;
}
int ofrecer(Subasta s, double precio){
  //poner en la cola el monto ofrecido si es mayor que el monto menor
  // y ponerme en wait hasta que otro elemento me despierte (me paso en la lista)
  //devolver false si termine de ofrecer antes de que termine la subasta
}
double adjudicar(Subasta s, int *punidades){
  //Terminar la subasta, nadie mas puede ofertar, y devolver en punidades
  // las unidades que no se vendieron
}
void destruirSubasta(Subasta s){
  //libera la memoria de la subasta
  DestroyPriQueue(s->q);
  free(s)
}

// =============================================================
// No toque nada a partir de aca: es la implementacion de la cola
// de prioridades
// =============================================================

PriQueue MakePriQueue(int maxsize) {
  PriQueue pq= malloc(sizeof(*pq));
  pq->maxsize= maxsize;
  pq->vec= malloc(sizeof(void*)*(maxsize+1));
  pq->ofertas= malloc(sizeof(double)*(maxsize+1));
  pq->size= 0;
  return pq;
}

void DestroyPriQueue(PriQueue pq) {
  free(pq->vec);
  free(pq->ofertas);
  free(pq);
}

void *PriGet(PriQueue pq) {
  void *t;
  int k;
  if (pq->size==0)
    return NULL;
  t= pq->vec[0];
  pq->size--;
  for (k= 0; k<pq->size; k++) {
    pq->vec[k]= pq->vec[k+1];
    pq->ofertas[k]= pq->ofertas[k+1];
  }
  return t;
}

void PriPut(PriQueue pq, void *t, double oferta) {
  if (pq->size==pq->maxsize)
    fatalError("PriPut", "Desborde de la cola de prioridad\n");
  int k;
  for (k= pq->size-1; k>=0; k--) {
    if (oferta > pq->ofertas[k])
      break;
    else {
      pq->vec[k+1]= pq->vec[k];
      pq->ofertas[k+1]= pq->ofertas[k];
    }
  }
  pq->vec[k+1]= t;
  pq->ofertas[k+1]= oferta;
  pq->size++;
}

double PriBest(PriQueue pq) {
  return pq->size==0 ? 0 : pq->ofertas[0];
}

int EmptyPriQueue(PriQueue pq) {
  return pq->size==0;
}

int PriLength(PriQueue pq) {
  return pq->size;
}
