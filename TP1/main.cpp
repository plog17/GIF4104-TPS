#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex gLock;
std::mutex gLockUp;
std::mutex gCout;
std::condition_variable gOdd;
std::condition_variable gUp;

bool gOddReady = false;
int gLowEven = 2;
int gHighOdd = 1;

void printHello(int tid)
{
    if(tid % 2 != 0){
        std::unique_lock<std::mutex> locker(gLock);
        while(!gOddReady){
            gOdd.wait(locker);
        }
        std::cout << tid;
        std::unique_lock<std::mutex> locker2(gLockUp);
        while(tid < gHighOdd){
            gUp.wait(locker2);
        }
        gHighOdd -= 2;
    }
    else{
        std::unique_lock<std::mutex> locker(gLockUp);
        while(tid > gLowEven){
            gUp.wait(locker);
        }
        gLowEven += 2;
    }
    gCout.lock();
    std::cout << "Je suis le fil d'execution #" << tid << std::endl;
    gCout.unlock();

    gUp.notify_all();
}

int highestOdd(int n){
    if(n % 2 == 0)
        return n - 1;
    else
        return n;
}

int main (int argc, char **argv)
{
    int nThread = 0;
    if(argc == 2){
        nThread = std::stoi(argv[1]);
    }
    else{
        std::cout << "launch file with 1 argument containing the number of thread" << std::endl;
        std::cout << "ex: ./exec 12" << std::endl;
    }

    gHighOdd = highestOdd(nThread);
    std::cout.sync_with_stdio(true);
    std::thread threads[nThread];
    for(int i = 0; i < nThread; ++i){
        threads[i] = std::thread(printHello, i+1);
    }

    for (int i = 1; i < nThread; i+=2) {
        threads[i].join();
    }

    gOddReady = true;
    gOdd.notify_all();

    for(int i = 0; i < nThread; i+=2){
        threads[i].join();
    }

    std::cout << "Terminating..." << std::endl;
    return 0;
}