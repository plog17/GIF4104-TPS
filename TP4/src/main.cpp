#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#ifdef __APPLE__
    #include "OpenCL/cl.hpp"
#else
    #include "CL/cl.hpp"
#endif

//Encoder à partir de pixels bruts sur le disque en un seul appel de fonction
//L'argument inImage contient inWidth * inHeight pixels RGBA ou inWidth * inHeight * 4 octets
void encode(const char* inFilename, vector<unsigned char>& inImage, unsigned inWidth, unsigned inHeight)
{
    //Encoder l'image
    unsigned lError = lodepng::encode(inFilename, inImage, inWidth, inHeight);

    //Montrer l'erreur s'il y en a une.
    if(lError)
        cout << "Erreur d'encodage " << lError << ": "<< lodepng_error_text(lError) << endl;
}

//Décoder à partir du disque dans un vecteur de pixels bruts en un seul appel de fonction
void decode(const char* inFilename,  vector<unsigned char>& outImage, unsigned int& outWidth, unsigned int& outHeight)
{
    //Décoder
    unsigned int lError = lodepng::decode(outImage, outWidth, outHeight, inFilename);

    //Montrer l'erreur s'il y en a une.
    if(lError)
        cout << "Erreur de décodage " << lError << ": " << lodepng_error_text(lError) << endl;

    //Les pixels sont maintenant dans le vecteur outImage, 4 octets par pixel, organisés RGBARGBA...
}

//Aide pour le programme
void usage(char* inName) {
    cout << endl << "Utilisation> " << inName << " fichier_image fichier_noyau [fichier_sortie=output.png]" << endl;
    exit(1);
}

int main(int argc, char** argv) {
    cl_int status; // return value for most OpenCL functions
    cl_uint numPlatforms = 0;

    //get all platforms (drivers)
    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);
    if(all_platforms.size()==0){
        std::cout<<" No platforms found. Check OpenCL installation!\n";
        exit(1);
    }
    cl::Platform default_platform=all_platforms[0];
        std::cout << "Using platform: "<<default_platform.getInfo<CL_PLATFORM_NAME>()<< std::endl;

    //get default device of the default platform
    std::vector<cl::Device> all_devices;
    default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
    if(all_devices.size()==0){
        std::cout<<" No devices found. Check OpenCL installation!\n";
        exit(1);
    }
    cl::Device default_device=all_devices[0];
    std::cout << "Found " << all_devices.size() << " device(s)" << std::endl;
    std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << std::endl;
    for(auto device : all_devices) {
        std::cout << "Found devices : " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    }



    //MAIN SEQ

    if(inArgc != 3 or inArgc > 4) usage(inArgv[0]);
    string lFilename = inArgv[1];
    string lOutFilename = "output.png";

    // Lire le noyau.
    ifstream lConfig;
    lConfig.open(inArgv[2]);
    if (!lConfig.is_open()) {
        cerr << "Le fichier noyau fourni (" << inArgv[2] << ") est invalide." << endl;
        exit(1);
    }

    PACC::Tokenizer lTok(lConfig);
    lTok.setDelimiters(" \n","");

    string lToken;
    lTok.getNextToken(lToken);

    int lK = atoi(lToken.c_str());
    int lHalfK = lK/2;

    cout << "Taille du noyau: " <<  lK << endl;

    //Lecture du filtre
    double* lFilter = new double[lK*lK];

    for (unsigned int i = 0; i < lK; i++) {
        for (unsigned int j = 0; j < lK; j++) {
            lTok.getNextToken(lToken);
            lFilter[i*lK+j] = atof(lToken.c_str());
        }
    }

    //Lecture de l'image
    //Variables à remplir
    unsigned int lWidth, lHeight;
    vector<unsigned char> lImage; //Les pixels bruts
    //Appeler lodepng
    decode(lFilename.c_str(), lImage, lWidth, lHeight);

    //Variables contenant des indices
    int fy, fx;
    //Variables temporaires pour les canaux de l'image
    double lR, lG, lB;


}
