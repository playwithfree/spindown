#include <pthread.h>

class Thread
{
   public:
      Thread();
      
      void start();
      static void* run(void* pContext);
      
      virtual int execute(){};
};
