// lock server implementation

#include "lock_server.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <map>

std::map<lock_protocol::lockid_t,rpc_lock*> lock_server::rpc_lock_map;

lock_server::lock_server():
  nacquire (0)
  
{
  pthread_mutex_init(&rpc_lock_mutex, NULL);
}

lock_protocol::status
lock_server::stat(int clt, lock_protocol::lockid_t lid, int &r)
{
  lock_protocol::status ret = lock_protocol::OK;
  printf("stat request from clt %d\n", clt);
  r = nacquire;
  return ret;
}



lock_server::~lock_server()
{
  // Free memory used by locks
  std::map<lock_protocol::lockid_t, rpc_lock*>::iterator it;

  for (it = rpc_lock_map.begin(); it != rpc_lock_map.end(); it++)
  {
	  assert(it->second != NULL);

	  delete(it->second);
  }

  pthread_mutex_destroy(&rpc_lock_mutex);
}


lock_protocol::status
lock_server::acquire(lock_protocol::lockid_t lid, int &r)
{
  lock_protocol::status ret = lock_protocol::OK;
  rpc_lock* lock_to_acquire;

  pthread_mutex_lock(&rpc_lock_mutex); 
  
  // See if lock already exists in map 
  // else create a new lock and add to map
  if (rpc_lock_map.count(lid) > 0)
  {
	  lock_to_acquire = rpc_lock_map.find(lid)->second;
  }
  else
  {
	  lock_to_acquire = new rpc_lock(lid);
	  rpc_lock_map[lid] = lock_to_acquire; 
  }
  
  pthread_mutex_unlock(&rpc_lock_mutex); 
 
  lock_to_acquire->acquire(); 


  // Increasing the numebr of locks held
  pthread_mutex_lock(&rpc_lock_mutex); 
  
  nacquire++;
  
  pthread_mutex_unlock(&rpc_lock_mutex); 
 
  r = 0;
  return ret;
}


lock_protocol::status
lock_server::release(lock_protocol::lockid_t lid, int &r)
{
  lock_protocol::status ret = lock_protocol::OK;
  rpc_lock* lock_to_release;

  pthread_mutex_lock(&rpc_lock_mutex); 
  
  // Lock should always be present in map
  if (rpc_lock_map.count(lid) > 0)
  {
	  lock_to_release = rpc_lock_map.find(lid)->second;
  }
  else
  {
	  assert(1);
  }
  
  pthread_mutex_unlock(&rpc_lock_mutex); 
 
  lock_to_release->release(); 

  // Decreasing the number of locks held
  pthread_mutex_lock(&rpc_lock_mutex); 
  
  nacquire--;
  
  pthread_mutex_unlock(&rpc_lock_mutex); 

  r = 0;
  return ret;
}







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

