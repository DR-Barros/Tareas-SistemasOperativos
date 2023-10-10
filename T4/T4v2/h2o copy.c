#define _XOPEN_SOURCE 500
#include "nthread-impl.h"
#include <nthread.h>

#include "pss.h"
#include "h2o.h"

//... defina aca sus variables globales ...

Queue *q;
Queue *qh;

void initH2O(void) {
  //... inicialice aca las variables globales ...
  q = makeQueue();
  qh = makeQueue();
}

void endH2O(void) {
  //... libere los recursos pedidos por ejemplo con malloc ...
  destroyQueue(q);
}

H2O *nCombineOxy(Oxygen *o, int timeout) {
  //... implemente esta funcion ...
  // Para crear la molecula de agua invoque makeH2O(h1, h2, o)
  START_CRITICAL
  nThread this_th = nSelf();
  put(q, this_th);
  // chequear si hay hidrogenos en espera, si hay los despierta
  if (!emptyQueue(qh)){
    nThread h_th = get(qh);
    setReady(h_th);
    if (!emptyQueue(qh)){
      h_th = get(qh);
      setReady(h_th);
    }
  }
  

  // Si no hay mensajes en cola
  if (nth_emptyQueue(this_th->sendQueue)){
      // esperar a que llegue mensaje
      suspend(WAIT_PUB);
      schedule();
  }

  // tomar 1er thread de la cola
  nThread sender_th1 = nth_getFront(this_th->sendQueue);
  
  // Si no hay mensajes en cola
  if (nth_emptyQueue(this_th->sendQueue)){
      // esperar a que llegue mensaje
      suspend(WAIT_PUB);
      schedule();
  }

  // tomar 1er thread de la cola y retornar su mensaje
  nThread sender_th2 = nth_getFront(this_th->sendQueue);
  Hydrogen *h1 = sender_th1->ptr;
  Hydrogen *h2 = sender_th2->ptr;
  //crear agua
  H2O *h20 = makeH2O(h1, h2, o);
  get(q);
  this_th->ptr = h20;
  setReady(sender_th1);
  schedule();
  setReady(sender_th2);
  schedule();


  END_CRITICAL
  return h20;
}

H2O *nCombineHydro(Hydrogen *h) {
  //... implemente esta funcion ...
  // Para crear la molecula de agua invoque makeH2O(h1, h2, o)
  

  START_CRITICAL
  nThread this_th = nSelf();
  if (peek(q) == NULL){
    put(qh, this_th);
    suspend(WAIT_PUB);
    schedule();
  }
  nThread th = peek(q);
  // si el thread th esta en nRecieve, se pone en ready
  if (th->status == WAIT_PUB){
    setReady(th);
  }
  // ponerme en la cola de threads que quiere mandar un mensaje
  nth_putBack(th->sendQueue, this_th);
  this_th->ptr = h; // guardar mensaje

  // esperar respuesta
  suspend(WAIT_REPLY);
  schedule();

  H2O *rc = th->ptr;
  // retornar rc
  END_CRITICAL
  return rc;

}
