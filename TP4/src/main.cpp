#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "lodepng.h"
#include "Tokenizer.hpp"

#ifdef __APPLE__
    #include "OpenCL/cl.hpp"
#else
    #include "CL/cl.h"
#endif

using namespace std;

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

/* Find a GPU or CPU associated with the first available platform */
cl_device_id create_device() {

    cl_platform_id platform;
    cl_uint nPlatforms;
    cl_int err;

    err = clGetPlatformIDs(0, NULL, &nPlatforms);
    if(err < 0) {
        perror("Couldn't identify a platform");
        exit(1);
    }

    if(nPlatforms == 0){
        printf("No Plateforms detected!");
        exit(-1);
    }

    printf("Found %d plateforms.\n", nPlatforms);

    /* Identify a platform */
    err = clGetPlatformIDs(1, &platform, NULL);
    if(err < 0) {
        perror("Couldn't identify a platform");
        exit(1);
    }

    cl_uint nGPU = 0;
    cl_uint nCPU = 0;
    cl_device_id dev;

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &nGPU);
    if(err == CL_DEVICE_NOT_FOUND) {
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 0, NULL, &nCPU);
    }
    if(err < 0) {
        perror("Couldn't access any devices");
        exit(1);
    }

    printf("Found %d GPU.\n", nGPU);
    printf("Found %d CPU.\n", nCPU);

    /* Access a device */
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
    if(err == CL_DEVICE_NOT_FOUND) {
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
    }
    if(err < 0) {
        perror("Couldn't access any devices");
        exit(1);
    }

    return dev;
}

/* Create program from a file and compile it */
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {

    cl_program program;
    FILE *program_handle;
    char *program_buffer, *program_log;
    size_t program_size, log_size;
    int err;

    /* Read program file and place content into buffer */
    program_handle = fopen(filename, "r");
    if(program_handle == NULL) {
        perror("Couldn't find the program file");
        exit(1);
    }
    fseek(program_handle, 0, SEEK_END);
    program_size = ftell(program_handle);
    rewind(program_handle);
    program_buffer = (char*)malloc(program_size + 1);
    program_buffer[program_size] = '\0';
    fread(program_buffer, sizeof(char), program_size, program_handle);
    fclose(program_handle);

    /* Create program from file */
    program = clCreateProgramWithSource(ctx, 1,
            (const char**)&program_buffer, &program_size, &err);
    if(err < 0) {
        perror("Couldn't create the program");
        exit(1);
    }
    free(program_buffer);

    /* Build program */
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if(err < 0) {

        /* Find size of log and print to std output */
        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
                0, NULL, &log_size);
        program_log = (char*) malloc(log_size + 1);
        program_log[log_size] = '\0';
        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
                log_size + 1, program_log, NULL);
        printf("%s\n", program_log);
        free(program_log);
        exit(1);
    }

    return program;
}

int main(int argc, char** argv) {
    /* OpenCL structures */
    cl_device_id device;
    cl_context context;
    cl_int i, j, err;
    size_t local_size, global_size;

    /* Create device and context */
    device = create_device();
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if(err < 0) {
        perror("Couldn't create a context");
        exit(1);
    }
    char buf[100];
    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(buf), buf, NULL);
    printf("Using device: %s", buf);

    cl_program program;

    program = build_program(context, device, "src/test.cl");

    cl_kernel kernel;
    kernel = clCreateKernel(program, "test", &err);
    if(err != CL_SUCCESS){
        printf("Kernel Creation Fail");
        exit(-1);
    }

    cl_command_queue cmdQueue = clCreateCommandQueue(context, device, 0, &err);
    if(err != CL_SUCCESS){
        printf("Command Queue Creation fail");
        exit(-1);
    }

    const int ELEMENTS = 10;
    size_t datasize = sizeof(float)*ELEMENTS;
    cl_mem d_input, d_output;
    float input[ELEMENTS];
    for(int i = 0; i < ELEMENTS; ++i){
        input[i] = i*0.5;
    }

    d_input = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, datasize, input, &err);
    if(err != CL_SUCCESS || d_input == NULL){
        printf("Input Buffer Allocation Error");
        exit(-1);
    }
    d_output = clCreateBuffer(context, CL_MEM_READ_WRITE, datasize, NULL, &err);
    if(err != CL_SUCCESS || d_input == NULL){
        printf("Output Buffer Allocation Error");
        exit(-1);
    }

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_input);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_output);
    if(err != CL_SUCCESS){
        printf("Set kernel argument fail");
        exit(-1);
    }


    size_t globalWorkSize[1];
    globalWorkSize[0] = ELEMENTS;
    clEnqueueNDRangeKernel(cmdQueue, kernel, 1, NULL, globalWorkSize, NULL, 0, NULL, NULL);

    float output[ELEMENTS];
    clEnqueueReadBuffer(cmdQueue, d_output, CL_TRUE, 0, datasize, output, 0 , NULL, NULL);

    for(int i = 0; i < ELEMENTS; ++i){
        printf("value %d = %f \n", i, output[i]);
    }

    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseMemObject(d_input);
    clReleaseMemObject(d_output);
    clReleaseContext(context);


    //MAIN SEQ
    /*
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

    */
}
