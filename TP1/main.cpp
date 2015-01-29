#include <iostream>
#include <thread>
#include <mutex>

std::mutex gMutex;

void printHello(int tid)
{
    gMutex.lock();
    std::cout << "Hello World! Thread ID, " << tid << std::endl;
    gMutex.unlock();
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
    
    std::thread threads[nThread];
    for(int i = 0; i < nThread; ++i){
        threads[i] = std::thread(printHello, i);
    }

    for (int i = 0; i < nThread; ++i) {
        threads[i].join();
    }
    return 0;
}