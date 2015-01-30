#include "ThreadManager.h"


int getArgument(int argc, char **argv);


int main(int argc, char **argv) {

    int nThread = getArgument(argc, argv);
    std::cout.sync_with_stdio(true);

    ThreadManager tManager(nThread);

    tManager.joinEven();
    tManager.joinOdd();

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
