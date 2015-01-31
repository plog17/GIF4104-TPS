#include <iostream>
#include <cstdlib>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
#include "Helper.h"


int main (int argc, char* argv[])
{
  int NUM_THREADS=atoi(argv[1]);
  int sleepTimeMilliSeconds=atoi(argv[2])*1000;

  Helper helper(NUM_THREADS,sleepTimeMilliSeconds);

  helper.createThreads();
}
