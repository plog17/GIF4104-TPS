#include "lodepng.h"
#include <iostream>
#include <fstream>

int getArgument(int argc, char **argv);
void usage(char* inName);

int main(int argc, char **argv) {


    return 0;
}

int getArgument(int argc, char **argv) {

}

void usage(char* inName) {
    std::cout << std::endl << "Utilisation> " << inName << " fichier_image fichier_noyau [fichier_sortie=output.png]" << std::endl;
    exit(1);
}
