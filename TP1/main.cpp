#include <iostream>
#include <cstdlib>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
#include "Helper.h"


int main (int argc, char* argv[])
{
  if (argc == 3) {
    int NUM_THREADS=atoi(argv[1]);
    int sleepTimeMilliSeconds=atoi(argv[2])*1000;

    Helper helper(NUM_THREADS,sleepTimeMilliSeconds);
    helper.createThreads();
    helper.joinThreads();
  }

  else {
    std::cout << "SVP fournir le NOMBRE DE THREADS et le DÉLAI de production en paramètres" << std::endl;
    std::cout << "ex: TP1 50 10\nCréera 50 threads avec 10 millisecondes d'attente entre les productions" << std::endl;
  }

  return 0;
}
