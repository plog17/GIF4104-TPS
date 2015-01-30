/*#include "ThreadManager.h"

std::mutex gLockOdd;
std::mutex gLockEven;
std::mutex gCout;
std::condition_variable gOddCond;
std::condition_variable gEvenCond;

bool gOddReady = false;
int gLowEven = 2;
int gHighOdd = 1;


void printHello(int tid) {
    gCout.lock();
    std::cout << "Je suis le fil d'execution #" << tid << std::endl;
    gCout.unlock();
}

void threadMain(int tid) {
    if (tid % 2 != 0) {
        std::unique_lock<std::mutex> locker2(gLockOdd);
        gOddCond.wait(locker2, [=]() {
            return tid >= gHighOdd - 1 && gOddReady;
        });
        gHighOdd -= 2;
        printHello(tid);
        gOddCond.notify_all();
    }
    else {
        std::unique_lock<std::mutex> locker(gLockEven);
        gEvenCond.wait(locker, [=]() {
            return tid <= gLowEven;
        });
        gLowEven += 2;
        printHello(tid);
        gEvenCond.notify_all();
    }
}

int getArgument(int argc, char **argv);

int highestOdd(int n);

int main(int argc, char **argv) {

    int nThread = getArgument(argc, argv);

    gHighOdd = highestOdd(nThread);
    std::cout.sync_with_stdio(true);
    std::thread threads[nThread];

    for (int i = 0; i < nThread; ++i) {
        threads[i] = std::thread(threadMain, i + 1);
    }

    for (int i = 1; i < nThread; i += 2) {
        threads[i].join();
    }

    gOddReady = true;
    gOddCond.notify_all();

    for (int i = 0; i < nThread; i += 2) {
        threads[i].join();
    }

    std::cout << "Terminating..." << std::endl;
    return 0;
}

int getArgument(int argc, char **argv) {
    int nThread;
    if (argc == 2) {
        nThread = std::stoi(argv[1]);
    }
    else {
        std::cout << "launch file with 1 argument containing the number of thread" << std::endl;
        std::cout << "ex: ./exec 12" << std::endl;
    }
    return nThread;
}

int highestOdd(int n) {
    if (n % 2 == 0)
        return n - 1;
    else
        return n;
}*/