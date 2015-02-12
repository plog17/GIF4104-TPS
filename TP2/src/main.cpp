#include <iostream>
#include <fstream>
#include <ncursesw/ncurses.h>
#include "PngImage.h"
#include "Filter.h"
#include "Chrono.hpp"
#include "omp.h"

int getArgument(int argc, char **argv);
void usage(char* inName);

void convolve(PngImage &exampleImg, Filter &filter);
void convolve2(PngImage &exampleImg, Filter &filter);

int main(int argc, char **argv) {
    Chrono chrono(false);
    chrono.resume();

    PngImage exampleImg("../Image/exemple.png");
    Filter filter("../filters/noyau_flou");
    double startTime = chrono.get();
    convolve2(exampleImg, filter);
    std::cout << "Total Time: " << chrono.get() - startTime << " sec" << std::endl;
    exampleImg.writeToDisk("../Image/test.jpg");
    return 0;
}

void convolve2(PngImage &exampleImg, Filter &filter){
    int hf = filter.size()/2;
    int convoWidth = exampleImg.getWidth() - filter.size();
    int convoHeight = exampleImg.getHeight() - filter.size();
    int blocks = convoWidth * convoHeight;
    int i = 0;
    #pragma omp parallel shared(exampleImg) private(i)
    {
        #pragma omp for schedule(static) nowait
        for (i = 0; i < blocks; ++i) {
            int x = i%convoWidth + hf;
            int y = i/convoWidth + hf;
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
                exampleImg[y*exampleImg.getWidth()*4 + x*4] = (unsigned char)lR;
                exampleImg[y*exampleImg.getWidth()*4 + x*4 + 1] = (unsigned char)lG;
                exampleImg[y*exampleImg.getWidth()*4 + x*4 + 2] = (unsigned char)lB;
            }
        }
    }
}

void convolve(PngImage &exampleImg, Filter &filter) {
    int x, y = 0;
#pragma omp parallel shared(exampleImg) private(x, y)
    {
        for (x = filter.size()/2; x < exampleImg.getWidth() - filter.size()/2; ++x){
            #pragma omp for schedule(static) nowait
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
    }
}

int getArgument(int argc, char **argv) {

}

void usage(char* inName) {
    std::cout << std::endl << "Utilisation> " << inName << " fichier_image fichier_noyau [fichier_sortie=output.png]" << std::endl;
    exit(1);
}
