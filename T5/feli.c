#define _XOPEN_SOURCE 500
#include "nthread-impl.h"
#include "pss.h"
#include "h2o.h"

int OCount = 0;
int HCount = 0;

nThread OWaiting = NULL;
nThread HWaiting1 = NULL;
nThread HWaiting2 = NULL;

NthQueue *HQueue;
NthQueue *OQueue;

NthQueue *qh; // Declaración de la cola de hidrógeno
NthQueue *qo; // Declaración de la cola de oxígeno

// Función para cancelar el timeout y limpiar el thread de oxígeno
void cancelTimeout(nThread th) {
        th->ptr = NULL;
        nth_delQueue(OQueue, th);  // Eliminar el hilo de la cola de oxígeno
}
void initH2O(void) {
    HQueue = nth_makeQueue();
    OQueue = nth_makeQueue();
}

void endH2O(void) {
    nth_destroyQueue(HQueue);
    nth_destroyQueue(OQueue);
}

H2O *nCombineOxy(Oxygen *o, int timeout) {
    START_CRITICAL
    nThread this_thread = nSelf();
    this_thread->ptr = o;
    if (nth_queueLength(HQueue) < 2) {// Si no hay suficientes átomos de hidrógeno, esperar en la cola de oxígeno

        nth_putBack(OQueue, this_thread);
        //this_thread->status = (timeout < 0) ? WAIT_H2O : WAIT_H2O_TIMEOUT;
        if(timeout<0){
            suspend(WAIT_H2O);

        }
        else{
            long long timeout_ns = timeout * 1000000LL;
            suspend(WAIT_H2O_TIMEOUT);
            nth_programTimer(timeout_ns, cancelTimeout);
        }
        
    } else {
        HWaiting1 = nth_getFront(HQueue);
        HWaiting2 = nth_getFront(HQueue);
        H2O *molecule = makeH2O(HWaiting1->ptr, HWaiting2->ptr, o);

        HCount -= 2;
        OCount -= 1;

        HWaiting1->ptr = molecule;
        HWaiting2->ptr = molecule;
        this_thread->ptr = molecule;
        
        setReady(HWaiting1);
        setReady(HWaiting2);
    }
    schedule();
    END_CRITICAL
    return this_thread->ptr;
}

H2O *nCombineHydro(Hydrogen *h) {
    START_CRITICAL
    H2O *molecule = NULL; 
    nThread thisTh = nSelf();
    thisTh->ptr = h;
    if (nth_emptyQueue(OQueue)) { // No hay oxígeno, debo esperar
        nth_putBack(HQueue, thisTh);
        suspend(WAIT_H2O);
    } else { // Debería haber al menos un oxígeno
        if (nth_queueLength(HQueue) >= 1) { 
            nThread th2 = nth_getFront(HQueue);
            nThread th3 = nth_getFront(OQueue);
            if (th3->status == WAIT_H2O_TIMEOUT) { //si el oxígeno estaba esperando con timeout, debo cancelarlo
                nth_cancelThread(th3);
            }
            molecule = makeH2O(th2->ptr, thisTh->ptr, th3->ptr);
            thisTh->ptr = molecule;
            th2->ptr = molecule;
            th3->ptr = molecule;

            setReady(th2);
            setReady(th3);
        } else { //Falta hidrógeno
            nth_putBack(HQueue, thisTh);
            suspend(WAIT_H2O);
        }
    }
    schedule();
    END_CRITICAL
    return thisTh->ptr; //Retorno la molécula
}