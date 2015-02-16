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
PngImage convolveSequential(PngImage &exampleImg, Filter &filter);
unsigned getNumberOfCoreOnMachine();

const char* pathToImage;
const char* pathToFilter;
const char* pathToOutput;
const int nbIterations;
const int nbThreads;

int main(int argc, char **argv) {
  double totalSequentialTime=0;
  double totalOpenMPTime=0;
  Chrono chrono(false);

  for (int i=0; i<=nbIterations;++i){
      chrono.reset();
      chrono.resume();

      loadArguments(argc, argv);

      PngImage exampleImg(pathToImage);
      Filter filter(pathToFilter);
      double startTime = chrono.get();
      PngImage newImage = convolve(exampleImg, filter);
      double elaspedTime = chrono.get() - startTime;
      std::cout << "Total Time for openMP solution: " << elaspedTime << " sec" << std::endl;
      newImage.writeToDisk(pathToOutput);
      totalOpenMPTime +=elaspedTime;

      chrono.reset();
      startTime = chrono.get();
      PngImage newImageSeq = convolveSequential(exampleImg, filter);
      elaspedTime = chrono.get() - startTime;
      std::cout << "Total Time for sequential solution: " << elaspedTime << " sec" << std::endl;
      totalSequentialTime +=elaspedTime;

  }

  std::cout << "------------------------" << std::endl;
  std::cout << "Average Time for sequential solution: " << totalSequentialTime/nbIterations << " sec" << std::endl;
  std::cout << "Average Time for OpenMP solution with " << nbThreads << " cores used by OpenMP: "<<totalOpenMPTime/nbIterations << " sec" << std::endl;

  return 0;
}

unsigned getNumberOfCoreOnMachine(){
  return std::thread::hardware_concurrency();
}

PngImage convolve(PngImage &exampleImg, Filter &filter) {
    PngImage filteredImage(*exampleImg.getData(), exampleImg.getWidth(), exampleImg.getHeight());
    int hf = filter.size()/2;
    int convoWidth = exampleImg.getWidth() - filter.size();
    int convoHeight = exampleImg.getHeight() - filter.size();
    int blocks = convoWidth * convoHeight;
    int i = 0;

    #pragma omp parallel private(i) num_threads(nbThreads)
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
PngImage convolveSequential(PngImage &exampleImg, Filter &filter) {
    PngImage filteredImage(*exampleImg.getData(), exampleImg.getWidth(), exampleImg.getHeight());
    int hf = filter.size()/2;
    int convoWidth = exampleImg.getWidth() - filter.size();
    int convoHeight = exampleImg.getHeight() - filter.size();
    int blocks = convoWidth * convoHeight;
    int i = 0;

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

    return filteredImage;
}


int loadArguments(int argc, char **argv) {
    if(argc<3 || argc>6){
        std::cout<<"Utilisation incorrecte."<<std::endl;
        usage("TP2");
    }
    else if (argc==3){
        pathToImage=argv[1];
        pathToFilter=argv[2];
        pathToOutput="output.png";
        nbThreads=getNumberOfCoreOnMachine();
        nbIterations=1;
    }
    else if (argc==4){
        pathToImage=argv[1];
        pathToFilter=argv[2];
        pathToOutput=argv[3];
        nbThreads=std::stoi(argv[4]);
        nbIterations=1;
    }
    else {
        pathToImage=argv[1];
        pathToFilter=argv[2];
        pathToOutput=argv[3];
        nbThreads=std::stoi(argv[4]);
        nbIterations=std::stoi(argv[5]);
    }
}

void usage(std::string inName) {
    std::cout << std::endl << "Utilisation> " << inName << " fichier_image fichier_noyau [fichier_sortie=output.png] [nombre_threads=nombre_coeur_machine] [nombre_iteration=1]" << std::endl;
    exit(1);
}
