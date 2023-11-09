#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#include "pss.h"
#include "batch.h"
#include "spinlocks.h"

#define FALSE 0
#define TRUE 1

struct job {
  // Defina aca esta estructura
  JobFun f;
  void *ptr;
  void *res;
  int s;
};

// Defina sus variables globales
Queue *waitQueue, *coreQueue;
int spinlockG;    

void initBatch(void) {
  spinlockG = OPEN;
  waitQueue = makeQueue();
  coreQueue = makeQueue();
}

void cleanBatch(void) {
  destroyQueue(waitQueue);
  destroyQueue(coreQueue);
}

Job *submitJob(JobFun fun, void *input) {
  spinLock(&spinlockG);
  Job *Job = malloc(sizeof(struct job));
  *Job = (struct job){fun, input, NULL, CLOSED};
  put(waitQueue, Job);         
  if (!emptyQueue(coreQueue)){    
    int *core = get(coreQueue);
    spinUnlock(core);           
  }
  spinUnlock(&spinlockG);             
  return Job;                 
}

void *waitJob(Job *job) {
  spinLock(&(job->s)); 
  spinLock(&spinlockG);       
  void *res = job->res; 
  free(job);           
  spinUnlock(&spinlockG);   
  return res;         
}

void batchServerFun(void) {
  while (TRUE) {
    spinLock(&spinlockG);                   
    if (emptyQueue(waitQueue)) {
      int core = CLOSED;
      put(coreQueue, &core);      
      spinUnlock(&spinlockG);             
      spinLock(&core);             
    } else {                        
      Job *job = get(waitQueue);    
      spinUnlock(&spinlockG);               
      if (job->f == NULL) {         
        free(job);
        return;
      }
      job->res = job->f(job->ptr); 
      spinUnlock(&job->s);         
    }
  }
}
