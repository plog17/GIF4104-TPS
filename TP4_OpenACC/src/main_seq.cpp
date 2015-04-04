
#include "lodepng.h"
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <sys/time.h>
#include "Tokenizer.hpp"
#include <openacc.h>
#include <openacc.h>


using namespace std;

//Aide pour le programme
void usage(char* inName) {
    cout << endl << "Utilisation> " << inName << " fichier_image fichier_noyau [fichier_sortie=output.png]" << endl;
    exit(1);
}

double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
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

int main(int inArgc, char *inArgv[])
{
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
    lTok.setDelimiters(" n","");
    
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
    vector<unsigned char> lOutputImage; //Les pixels bruts
    
    //Appeler lodepng
    decode(lFilename.c_str(), lImage, lWidth, lHeight);
    decode(lFilename.c_str(), lOutputImage, lWidth, lHeight);

    unsigned char* lOutputImageBuffer = lOutputImage.data();
    unsigned char* lImageBuffer = lImage.data();

    //début convolution
    double startTime = get_wall_time();
    
    cout << " Debut convolution " << endl; 

  
    #pragma acc data copyin(lImageBuffer[0:lHeight* lWidth]) copyin(lFilter[0:lK* lK]) copyin(lHeight) copyin(lWidth) copyin(lHalfK) copy(lOutputImageBuffer[0:lWidth*lHeight])
    {
        #pragma acc region
	{
        //cout << " Entrer dans la zone ACC " << lWidth - lHalfK << endl;

        //#pragma acc kernels
        #pragma acc loop independent vector(16)
        for(unsigned int x = lHalfK; x < lWidth - lHalfK; x++)
        {
            int bound=lHeight-lHalfK;
            
            //#pragma acc for private(lOutputImageBuffer[0:lWidth*lHeight]) 
            for (unsigned int y = lHalfK; y < bound; y++)
            {
                
                //Variables temporaires pour les canaux de l'image
                double lR, lG, lB;
                lR = 0.;
                lG = 0.;
                lB = 0.;
                unsigned outImBufferIndex = y*lWidth*4 + x;                

                //#pragma acc kernels
                #pragma acc loop gang
                {
                    for (int j = -lHalfK; j <= lHalfK; j++) {
                        int fy = j + lHalfK;

                        //#pragma acc loop vector reduction(+:lR) reduction(+:lG) reduction(+:lB) 
                        for (int i = -lHalfK; i <= lHalfK; i++) {
                            int fx = i + lHalfK;

                            //R[x + i, y + j] = Im[x + i, y + j].R * Filter[i, j]
                            lR += double(lImageBuffer[(y + j)*lWidth*4 + (x + i)*4]) * lFilter[fx + fy*lK];
                            //cout << double(lImageBuffer[(y + j)*lWidth*4 + (x + i)*4]) * lFilter[fx + fy*lK] << endl;

                            lG += double(lImageBuffer[(y + j)*lWidth*4 + (x + i)*4 + 1]) * lFilter[fx + fy*lK];
                            //cout << double(lImageBuffer[(y + j)*lWidth*4 + (x + i)*4 + 1]) * lFilter[fx + fy*lK] << endl;

                            lB += double(lImageBuffer[(y + j)*lWidth*4 + (x + i)*4 + 2]) * lFilter[fx + fy*lK];
                            //cout << double(lImageBuffer[(y + j)*lWidth*4 + (x + i)*4 + 2]) * lFilter[fx + fy*lK] << endl;
                        }
                    }
                    //Placer le résultat dans l'image.

                    //cout << (unsigned char)lR << lR << endl;
                    lOutputImageBuffer[outImBufferIndex] = (unsigned char)lR;

                    //cout << (unsigned char)lG << endl;                    
                    lOutputImageBuffer[outImBufferIndex + 1] = (unsigned char)lG;

                    //cout << (unsigned char)lB << endl;                    
                    lOutputImageBuffer[outImBufferIndex + 2] = (unsigned char)lB;

                }
            }
        }
    }}
    
    cout << "Fin de la convolution" << endl;


    //fin convolution, on s’assure que c’est fini
    #pragma acc wait
    
    double endTime = get_wall_time();

    
    //Sauvegarde de l'image dans un fichier sortie
    encode(lOutFilename.c_str(),  lOutputImage, lWidth, lHeight);
    
    cout << "Temps total: " << endTime - startTime << endl;
    cout << "L'image a été filtrée et enregistrée dans " << lOutFilename << " avec succès!" << endl;

    delete lFilter;
    return 0;
}

