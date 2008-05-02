#include <pthread.h>
#include "thread.h"

Thread::Thread(){}

void Thread::start()
{
  pthread_t thread1;
  int pff = pthread_create(&thread1, NULL, run, this);
  pff  = pthread_create(&thread1, NULL, handleSignals, this);
}

void* Thread::run(void* pContext)
{
  int nRes = ((Thread*)pContext)->execute();
}

void* Thread::handleSignals(void* pContext)
{
  int nRes = ((Thread*)pContext)->signalHandler();
}
