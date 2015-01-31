#include "Helper.h"

Helper::Helper(int nbThreads, int sleepTime){
  NUM_THREADS=nbThreads;
  SLEEP_TIME = sleepTime;
   calculateSynchronisationConditions();
}

Helper::~Helper(){
  v.clear();
}


void Helper::calculateSynchronisationConditions(){
  if(NUM_THREADS % 2 == 0){
     lastEvenThreadToExecute= NUM_THREADS;
     firstOddThreadToExecute= NUM_THREADS-1;
  }
  else{
    lastEvenThreadToExecute= NUM_THREADS-1;
    firstOddThreadToExecute= NUM_THREADS;
  }
}

void Helper::printMessage(int id){
  std::cout << "Je suis le fil d'execution #" << id << std::endl;
  lastThreadExecuted=id;

  if(id==lastEvenThreadToExecute && !evenThreadsDone){
    evenThreadsDone=true;
    lastThreadExecuted= firstOddThreadToExecute+2;
    evenDone.notify_all();
  }
  else{
    nextElem.notify_all();
  }
}


void Helper::executeThread(int id){
  if(id%2!=0){
    std::unique_lock<std::mutex> lockEven(LockWaitEven);
    while(!evenThreadsDone){
      evenDone.wait(lockEven);
    }
    lockEven.unlock();

    std::unique_lock<std::mutex> lock(LockElem);
    while(lastThreadExecuted-2 != id){
      nextElem.wait(lock);
    }
    printMessage(id);
    lock.unlock();
  }

  else{
    std::unique_lock<std::mutex> lock(LockElem);
    while(lastThreadExecuted+2 != id ){
      nextElem.wait(lock);
    }
    printMessage(id);
    lock.unlock();
  }
}

void Helper::createThreads(){
  std::cout << NUM_THREADS << " threads will be created."<<std::endl;
  for(int i=1; i<=NUM_THREADS;++i){
     v.push_back(std::thread(&Helper::executeThread,this, i));
    usleep(this->SLEEP_TIME);
  }
}

void Helper::joinThreads(){
    for(int i=0; i<=NUM_THREADS;++i){
       v[i].join();
    }
}
