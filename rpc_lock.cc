// rpc_lock implementation

#include "rpc_lock.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <assert.h>

lock_server::lock_server():
  nacquire (0)
{
}

lock_protocol::status
lock_server::stat(int clt, lock_protocol::lockid_t lid, int &r)
{
  lock_protocol::status ret = lock_protocol::OK;
  printf("stat request from clt %d\n", clt);
  r = nacquire;
  return ret;
}


// Initializing the per-lock mutex and
// setting initial state to FREE
rpc_lock(lock_protocol::lockid_t lockid)
{
  pthread_mutex_init(&internal_mutex, NULL);
  pthread_cond_init(&internal_cond, NULL);
  
  id = lockid;
  
  lockstatus = FREE;
}


lock_protocol::status
rpc_acquire()
{
  lock_protocol::status ret = OK;

  // Use conditional variables to sleep if lock is locked
  pthread_mutex_lock(&internal_mutex);
  
  while (lockstatus == LOCKED)
  {
    pthread_cond_wait(&internal_cond, &internal_mutex);
  }

  // At this point it is safe to grab the lock
  lockstatus = LOCKED;

  pthread_cond_signal(&internal_cond);

  pthread_mutex_unlock(&internal_mutex);

  return ret;
}


lock_protocol::status
rpc_release()
{
  lock_protocol::status ret = OK;

  // Change state to free
  pthread_mutex_lock(&internal_mutex);
 
  lockstatus = FREE; 

  pthread_mutex_unlock(&internal_mutex);
  
  pthread_cond_signal(&internal_cond);

  return ret;
}
