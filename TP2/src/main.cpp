#include <iostream>
#include <fstream>
#include "PngImage.h"

int getArgument(int argc, char **argv);
void usage(char* inName);

int main(int argc, char **argv) {
    PngImage exampleImg("../Image/exemple.png");
    PngImage newImg(*exampleImg.getData(), exampleImg.getWidth(), exampleImg.getHeight());

    newImg.writeToDisk("../Image/output.png");
    return 0;
}

int getArgument(int argc, char **argv) {

}

void usage(char* inName) {
    std::cout << std::endl << "Utilisation> " << inName << " fichier_image fichier_noyau [fichier_sortie=output.png]" << std::endl;
    exit(1);
}
