#include "Helper.h"

Helper::Helper(int nbThreads, int sleepTime){
  this->NUM_THREADS=nbThreads;
  this->SLEEP_TIME = sleepTime;
   calculateSynchronisationConditions();
}

Helper::~Helper(){
}


void Helper::calculateSynchronisationConditions(){
  if(this->NUM_THREADS % 2 == 0){
     this->lastEvenThreadToExecute= this->NUM_THREADS;
     this->firstOddThreadToExecute= this->NUM_THREADS-1;
    std::cout<<"lastEvenThreadToExecute: "<<this->lastEvenThreadToExecute<<std::endl;
  }
  else{
    this->lastEvenThreadToExecute= NUM_THREADS-1;
    this->firstOddThreadToExecute= NUM_THREADS;
    std::cout<<"lastEvenThreadToExecute: "<<this->lastEvenThreadToExecute<<std::endl;
  }
}

void Helper::printMessage(int id){
  std::cout << "Bonjour je suis: " << id << std::endl;
  this->lastThreadExecuted=id;

  if(id==this->lastEvenThreadToExecute && !this->evenThreadsDone){
    std::cout<<"Last even"<<std::endl;
    this->evenThreadsDone=true;
    this->lastThreadExecuted= this->firstOddThreadToExecute+2;
    evenDone.notify_all();
    std::cout<<"notify_all"<<std::endl;
  }
  else{
    nextElem.notify_one();
    std::cout<<"notify_one"<<std::endl;
  }
}


void Helper::executeThread(int id){
  if(id%2!=0){
    std::unique_lock<std::mutex> lockEven(m);
    while(!evenThreadsDone){
      evenDone.wait(lockEven);
    }
    lockEven.unlock();

    std::unique_lock<std::mutex> lock(m);
    while(lastThreadExecuted-2 != id){
      nextElem.wait(lock);
    }
    printMessage(id);
    lock.unlock();
  }

  else{
    std::unique_lock<std::mutex> lock(m);
    while(lastThreadExecuted+2 != id ){
      nextElem.wait(lock);
    }
    printMessage(id);
    lock.unlock();
  }
}

void Helper::createThreads(){
  std::cout <<  this->NUM_THREADS << " threads will be created."<<std::endl;
  for(int i=1; i<=this->NUM_THREADS;++i){
     v.push_back(std::thread(&Helper::executeThread,this, i));
    usleep(this->SLEEP_TIME);
  }

  for(int i=1; i<=NUM_THREADS;++i){
     v[i].join();
  }
}
