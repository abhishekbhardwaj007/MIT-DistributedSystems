// rpc_lock implementation

#include "rpc_lock.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <assert.h>


// Initializing the per-lock mutex and
// setting initial state to FREE
rpc_lock::rpc_lock(lock_protocol::lockid_t lockid)
{
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);
  
  id = lockid;
  
  lockstatus = FREE;
}


rpc_lock::~rpc_lock()
{
  pthread_mutex_destroy(&mutex);
}


lock_protocol::status
rpc_lock::acquire()
{
  lock_protocol::status ret = lock_protocol::OK;

  // Use conditional variables to sleep if lock is locked
  pthread_mutex_lock(&mutex);
  
  while (lockstatus == LOCKED)
  {
    pthread_cond_wait(&cond, &mutex);
  }

  // At this point it is safe to grab the lock
  lockstatus = LOCKED;

  pthread_cond_signal(&cond);

  pthread_mutex_unlock(&mutex);

  return ret;
}


lock_protocol::status
rpc_lock::release()
{
  lock_protocol::status ret = lock_protocol::OK;

  // Change state to free
  pthread_mutex_lock(&mutex);
 
  lockstatus = FREE; 

  pthread_mutex_unlock(&mutex);
  
  pthread_cond_signal(&cond);

  return ret;
}
