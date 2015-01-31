#include <iostream>
#include <cstdlib>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
#include <unistd.h>

using namespace std;

mutex m;
condition_variable evenDone;
condition_variable nextElem;

int NUM_THREADS=-1;
bool evenThreadsDone = false;
bool oddThreadsDone = false;
bool done=false;
bool notified=false;

int lastEvenThreadToExecute = -1;
int lastThreadExecuted = 0;
int nextThreadToExecute = 2;
int firstOddThreadToExecute = -1;

vector<thread> v;

void printMessage(int id){
  cout << "Bonjour je suis: " << id << endl;
  lastThreadExecuted=id;

  if(id==lastEvenThreadToExecute && !evenThreadsDone){
    cout<<"Last even"<<endl;
    evenThreadsDone=true;
    lastThreadExecuted=firstOddThreadToExecute+2;
    evenDone.notify_all();
    cout<<"notify_all"<<endl;
  }
  else{
    nextElem.notify_one();
    cout<<"notify_one"<<endl;
  }
}


void createThread(int id){
  if(id%2!=0){
    unique_lock<mutex> lockEven(m);
    while(!evenThreadsDone){
      evenDone.wait(lockEven);
    }
    lockEven.unlock();

    unique_lock<mutex> lock(m);
    while(lastThreadExecuted-2 != id){
      nextElem.wait(lock);
    }
    printMessage(id);
    lock.unlock();
  }

  else{
    unique_lock<mutex> lock(m);
    while(lastThreadExecuted+2 != id ){
      nextElem.wait(lock);
    }
    printMessage(id);
    lock.unlock();
  }
}

void calculateSynchronisationConditions(){
  if(NUM_THREADS % 2 == 0){
    lastEvenThreadToExecute=NUM_THREADS;
    firstOddThreadToExecute=NUM_THREADS-1;
    cout<<"lastEvenThreadToExecute: "<<lastEvenThreadToExecute<<endl;
  }
  else{
    lastEvenThreadToExecute=NUM_THREADS-1;
    firstOddThreadToExecute=NUM_THREADS;
    cout<<"lastEvenThreadToExecute: "<<lastEvenThreadToExecute<<endl;
  }
}

int main (int argc, char* argv[])
{
  NUM_THREADS=atoi(argv[1]);
  int sleepTimeMilliSeconds=atoi(argv[2])*1000;
  cout << NUM_THREADS << " threads will be created."<<endl;
  calculateSynchronisationConditions();

  for(int i=1; i<=NUM_THREADS;++i){
    v.push_back(std::thread(createThread, i));
    usleep(sleepTimeMilliSeconds);
  }

  for(int i=1; i<=NUM_THREADS;++i){
    v[i].join();
  }
}
