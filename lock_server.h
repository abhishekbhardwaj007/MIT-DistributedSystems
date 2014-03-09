// this is the lock server
// the lock client has a similar interface

#ifndef lock_server_h
#define lock_server_h

#include <string>
#include "lock_protocol.h"
#include "lock_client.h"
#include "rpc.h"
#include <pthread.h>
#include <map>

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


class lock_server {

 protected:
  int nacquire;
  pthread_mutex_t rpc_lock_mutex;
  static std::map<lock_protocol::lockid_t,rpc_lock*> rpc_lock_map;

 public:
  lock_server();
  ~lock_server();
  lock_protocol::status stat(int clt, lock_protocol::lockid_t lid, int &);
  lock_protocol::status acquire(lock_protocol::lockid_t lid, int &);
  lock_protocol::status release(lock_protocol::lockid_t lid, int &);
};



#endif 







