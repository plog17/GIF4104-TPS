#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadManager{
public:
    ThreadManager(int nThread);
    ~ThreadManager();
    void joinEven();
    void joinOdd();

private:
    void createThreads();
    void worker(int tid);
    void printHello(int tid);
    int getHighestOdd(int n);

    std::thread *threads;

    std::mutex LockOdd;
    std::mutex LockEven;
    std::mutex Cout;
    std::condition_variable OddCond;
    std::condition_variable EvenCond;

    int nThread;
    bool OddReady = false;
    int LowestEven = 2;
    int HighestOdd;
};