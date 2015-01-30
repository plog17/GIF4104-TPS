#include "ThreadManager.h"

ThreadManager::ThreadManager(int nThread) : nThread(nThread) {
    this->threads = new std::thread[nThread];
    this->HighestOdd = this->getHighestOdd(nThread);
    for (int i = 0; i < nThread; ++i) {
        threads[i] = std::thread(&ThreadManager::worker,this, i + 1);
    }
}

ThreadManager::~ThreadManager() {
    delete[] this->threads;
}

void ThreadManager::joinEven() {
    for (int i = 1; i < this->nThread; i += 2) {
        this->threads[i].join();
    }
}

void ThreadManager::joinOdd() {
    this->OddReady = true;
    this->OddCond.notify_all();

    for (int i = 0; i < this->nThread; i += 2) {
        this->threads[i].join();
    }
}

void ThreadManager::printHello(int tid) {
    this->Cout.lock();
    std::cout << "Je suis le fil d'execution #" << tid << std::endl;
    this->Cout.unlock();
}

void ThreadManager::worker(int tid) {
    if (tid % 2 != 0) {
        std::unique_lock<std::mutex> locker2(this->LockOdd);
        this->OddCond.wait(locker2, [=]() {
            return tid >= this->HighestOdd - 1 && this->OddReady;
        });
        this->HighestOdd -= 2;
        printHello(tid);
        this->OddCond.notify_all();
    }
    else {
        std::unique_lock<std::mutex> locker(this->LockEven);
        this->EvenCond.wait(locker, [=]() {
            return tid <= this->LowestEven;
        });
        this->LowestEven += 2;
        printHello(tid);
        this->EvenCond.notify_all();
    }
}

int ThreadManager::getHighestOdd(int n) {
    if (n % 2 == 0)
        return n - 1;
    else
        return n;
}