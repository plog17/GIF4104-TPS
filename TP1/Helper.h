#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <unistd.h>

class Helper{
public:
    Helper(int nbThreads, int sleepTime);
    ~Helper();

    void calculateSynchronisationConditions();
    void printMessage(int id);
    void executeThread(int id);
    void createThreads();
    void joinThreads();

private:
    std::mutex LockWaitEven;
    std::mutex LockElem;

    std::condition_variable evenDone;
    std::condition_variable nextElem;
    std::vector<std::thread> v;

    int NUM_THREADS=-1;
    int SLEEP_TIME=1;
    bool evenThreadsDone = false;
    bool oddThreadsDone = false;
    bool done=false;
    bool notified=false;

    int lastEvenThreadToExecute = -1;
    int lastThreadExecuted = 0;
    int nextThreadToExecute = 2;
    int firstOddThreadToExecute = -1;
};
