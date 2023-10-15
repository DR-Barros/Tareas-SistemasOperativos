#define _XOPEN_SOURCE 500
#include "nthread-impl.h"
#include <nthread.h>

#include "pss.h"
#include "h2o.h"

//... defina aca sus variables globales ...


NthQueue *qo;
NthQueue *qh;

void initH2O(void) {
  //... inicialice aca las variables globales ...
  qo = nth_makeQueue();
  qh = nth_makeQueue();
}

void endH2O(void) {
  //... libere los recursos pedidos por ejemplo con malloc ...
  nth_destroyQueue(qo);
  nth_destroyQueue(qh);
}

H2O *nCombineOxy(Oxygen *o, int timeout) {
  //... implemente esta funcion ...
  // Para crear la molecula de agua invoque makeH2O(h1, h2, o)
  START_CRITICAL
  H2O *h2o;
  if (nth_queueLength(qh) < 2){
    //no esta listo para crear agua
    nThread this_th = nSelf();
    this_th->ptr = o;
    nth_putBack(qo, this_th);

    // esperar que le manden el agua
    if (timeout > 0){
      suspend(WAIT_H2O_TIMEOUT);
      nth_programTimer(timeout * 1000000, NULL);
    } else {
      suspend(WAIT_H2O);
    }
    schedule();
    if (this_th->ptr == o){
      nth_delQueue(qo, this_th);
      this_th->ptr = NULL;
    } 
    h2o = this_th->ptr;
  } else {
    //se pone a crear agua
    nThread h1, h2;
    h1 = nth_getFront(qh);
    h2 = nth_getFront(qh);
    h2o = makeH2O(h1->ptr, h2->ptr, o);
    //envia el agua a los hidrogenos
    h1->ptr = h2o;
    h2->ptr = h2o;
    //despierta a los hidrogenos
    setReady(h1);
    setReady(h2);
    schedule();
  }
  END_CRITICAL
  return h2o;
}

H2O *nCombineHydro(Hydrogen *h) {
  //... implemente esta funcion ...
  // Para crear la molecula de agua invoque makeH2O(h1, h2, o)
  

  START_CRITICAL
  
  H2O *h2o;
  if (nth_queueLength(qo)< 1){
    //no esta listo para crear agua
    nThread this_th = nSelf();
    this_th->ptr = h;
    nth_putBack(qh, this_th);

    // esperar que le manden el agua
    suspend(WAIT_PUB);
    schedule();
    h2o = this_th->ptr;
  } else{
    if (nth_queueLength(qh)< 1){
      //no esta listo para crear agua
      nThread this_th = nSelf();
      this_th->ptr = h;
      nth_putBack(qh, this_th);

      // esperar que le manden el agua
      suspend(WAIT_PUB);
      schedule();
      h2o = this_th->ptr;
    } else {
    //se pone a crear agua
    nThread h1, o;
    h1 = nth_getFront(qh);
    o = nth_getFront(qo);
    h2o = makeH2O(h1->ptr, h, o->ptr);
    //envia el agua a los hidrogenos
    h1->ptr = h2o;
    o->ptr = h2o;
    if(o->status == WAIT_SEND_TIMEOUT){
      // cancelar timer si tiene timer
      nth_cancelThread(o);
    }
    //despierta a los hidrogenos
    setReady(h1);
    setReady(o);
    schedule();
    }
  }
  END_CRITICAL
  return h2o;
}