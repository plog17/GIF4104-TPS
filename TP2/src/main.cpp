#include <iostream>
#include <fstream>
#include "PngImage.h"
#include "Filter.h"
#include "Chrono.hpp"
#include "omp.h"

int getArgument(int argc, char **argv);
void usage(char* inName);

int main(int argc, char **argv) {
    Chrono chrono(false);
    chrono.resume();

    PngImage exampleImg("../Image/exemple.png");
    Filter filter("../filters/noyau_flou");
    double startTime = chrono.get();
    int tid;
    #pragma omp parallel private(tid)
    {
        tid = omp_get_thread_num();
        printf("Thread : %d\n",tid);
        if(tid == 0){
            printf("Number of threads : %d\n",omp_get_num_threads());
        }
    }
    std::cout << "Total Time: " << chrono.get() - startTime << " sec" << std::endl;

    return 0;
}

int getArgument(int argc, char **argv) {

}

void usage(char* inName) {
    std::cout << std::endl << "Utilisation> " << inName << " fichier_image fichier_noyau [fichier_sortie=output.png]" << std::endl;
    exit(1);
}
