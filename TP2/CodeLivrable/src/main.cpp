#include <iostream>
#include <fstream>
//#include <ncursesw/ncurses.h>
#include "PngImage.h"
#include "Filter.h"
#include "Chrono.hpp"
#include "omp.h"
#include <thread>

int loadArguments(int argc, char **argv);
void usage(std::string inName);
PngImage convolve(PngImage &exampleImg, Filter &filter);
unsigned getNumberOfCoreOnMachine();

const char* pathToImage;
const char* pathToFilter;
const char* pathToOutput;

int main(int argc, char **argv) {
    Chrono chrono(false);
    chrono.resume();

    loadArguments(argc, argv);

    PngImage exampleImg(pathToImage);
    Filter filter(pathToFilter);
    double startTime = chrono.get();
    PngImage newImage = convolve(exampleImg, filter);
    std::cout << "Total Time: " << chrono.get() - startTime << " sec" << std::endl;
    newImage.writeToDisk(pathToOutput);
    return 0;
}


PngImage convolve(PngImage &exampleImg, Filter &filter) {
    PngImage filteredImage(*exampleImg.getData(), exampleImg.getWidth(), exampleImg.getHeight());
    int hf = filter.size()/2;
    int convoWidth = exampleImg.getWidth() - filter.size();
    int convoHeight = exampleImg.getHeight() - filter.size();
    int blocks = convoWidth * convoHeight;
    int i = 0;

    #pragma omp parallel private(i) num_threads(getNumberOfCoreOnMachine())
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
                filteredImage[y*exampleImg.getWidth()*4 + x*4] = (unsigned char)lR;
                filteredImage[y*exampleImg.getWidth()*4 + x*4 + 1] = (unsigned char)lG;
                filteredImage[y*exampleImg.getWidth()*4 + x*4 + 2] = (unsigned char)lB;
            }
        }
    }
    return filteredImage;
}

unsigned getNumberOfCoreOnMachine(){
  return std::thread::hardware_concurrency();
}

int loadArguments(int argc, char **argv) {
    if(argc<3 || argc>4){
        std::cout<<"Utilisation incorrecte."<<std::endl;
        usage("TP2");
    }
    else if (argc==3){
        pathToImage=argv[1];
        pathToFilter=argv[2];
        pathToOutput="output.png";
    }
    else{
        pathToImage=argv[1];
        pathToFilter=argv[2];
        pathToOutput=argv[3];
    }
}

void usage(std::string inName) {
    std::cout << std::endl << "Utilisation> " << inName << " fichier_image fichier_noyau [fichier_sortie=output.png]" << std::endl;
    exit(1);
}
