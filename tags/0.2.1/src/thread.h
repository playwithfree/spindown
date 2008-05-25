#include <pthread.h>

class Thread
{
  public:
    Thread();
      
    void start();
    static void* run(void* pContext);
    static void* handleSignals(void* pContext);
      
    virtual int execute(){};
    virtual int signalHandler(){};
};
