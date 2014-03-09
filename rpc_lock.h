// rpc_lock header

#ifndef rpc_lock_h
#define rpc_lock_h

#include <pthread.h>
#include <map>
#include "lock_protocol.h"

class rpc_lock {

  private:	
  pthread_mutex_t mutex; 
  pthread_cond_t  cond;
  
  typedef enum status { FREE, LOCKED } status;
  status lockstatus;

  lock_protocol::lockid_t id;

  public:
 
  rpc_lock(lock_protocol::lockid_t lockid);
  ~rpc_lock();

  lock_protocol::status acquire(); 
  lock_protocol::status release(); 
};

#endif 
