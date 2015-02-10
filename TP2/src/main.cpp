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
    int tid, x, y = 0;
    #pragma omp parallel shared(exampleImg) private(tid, x, y)
    {
        for (x = filter.size()/2; x < exampleImg.getWidth() - filter.size()/2; ++x){
            #pragma omp for schedule(static)
            for(y = filter.size()/2; y < exampleImg.getHeight() - filter.size()/2; ++y){
                int lR = 0.;
                int lG = 0.;
                int lB = 0.;
                for (int j = -filter.size()/2; j <= filter.size()/2; j++) {
                    int fy = j + filter.size()/2;
                    for (int i = -filter.size()/2; i <= filter.size()/2; i++) {
                        int fx = i + filter.size()/2;
                        //R[x + i, y + j] = Im[x + i, y + j].R * Filter[i, j]
                        lR += double(exampleImg[(y + j)*exampleImg.getWidth()*4 + (x + i)*4]) * filter[fx + fy*filter.size()];
                        lG += double(exampleImg[(y + j)*exampleImg.getWidth()*4 + (x + i)*4 + 1]) * filter[fx + fy*filter.size()];
                        lB += double(exampleImg[(y + j)*exampleImg.getWidth()*4 + (x + i)*4 + 2]) * filter[fx + fy*filter.size()];
                    }
                }
                //Placer le résultat dans l'image.
                exampleImg[y*exampleImg.getWidth()*4 + x*4] = (unsigned char)lR;
                exampleImg[y*exampleImg.getWidth()*4 + x*4 + 1] = (unsigned char)lG;
                exampleImg[y*exampleImg.getWidth()*4 + x*4 + 2] = (unsigned char)lB;
            }
        }
        /*tid = omp_get_thread_num();
        printf("Thread : %d\n",tid);
        if(tid == 0){
            printf("Number of threads : %d\n",omp_get_num_threads());
        }*/
    }
    std::cout << "Total Time: " << chrono.get() - startTime << " sec" << std::endl;
    exampleImg.writeToDisk("../Image/test.jpg");
    return 0;
}

int getArgument(int argc, char **argv) {

}

void usage(char* inName) {
    std::cout << std::endl << "Utilisation> " << inName << " fichier_image fichier_noyau [fichier_sortie=output.png]" << std::endl;
    exit(1);
}
