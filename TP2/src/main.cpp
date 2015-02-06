#include <iostream>
#include <fstream>
#include "PngImage.h"
#include "Chrono.hpp"

int getArgument(int argc, char **argv);
void usage(char* inName);

int main(int argc, char **argv) {
    Chrono chrono(false);
    std::cout << "The resolution on this machine is: " << chrono.getRes() << " sec" << std::endl;
    chrono.resume();
    double startTime = chrono.get();

    PngImage exampleImg("../Image/exemple.png");
    PngImage newImg(*exampleImg.getData(), exampleImg.getWidth(), exampleImg.getHeight());

    newImg.writeToDisk("../Image/output.png");

    std::cout << "Total Time: " << chrono.get() - startTime << " sec" << std::endl;
    return 0;
}

int getArgument(int argc, char **argv) {

}

void usage(char* inName) {
    std::cout << std::endl << "Utilisation> " << inName << " fichier_image fichier_noyau [fichier_sortie=output.png]" << std::endl;
    exit(1);
}
