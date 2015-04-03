#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "lodepng.h"
#include "PngImage.h"
#include "Filter.h"
#include "Tokenizer.hpp"
#include "Chrono.hpp"

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
    Chrono chrono(false);
    chrono.resume();

    /* Create device and context */
    device = create_device();
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if(err < 0) {
        perror("Couldn't create a context");
        exit(1);
    }
    char buf[100];
    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(buf), buf, NULL);
    printf("Using device: %s\n", buf);

    cl_program program;

    program = build_program(context, device, "src/convolve.cl");

    cl_kernel kernel;
    kernel = clCreateKernel(program, "convolve", &err);
    if(err != CL_SUCCESS){
        printf("Kernel Creation Fail");
        exit(-1);
    }

    cl_command_queue cmdQueue = clCreateCommandQueue(context, device, 0, &err);
    if(err != CL_SUCCESS){
        printf("Command Queue Creation fail");
        exit(-1);
    }

    PngImage exampleImg("Image/exemple.png");
    PngImage filteredImage(*exampleImg.getData(), exampleImg.getWidth(), exampleImg.getHeight());
    Filter filter("filters/noyau_flou");
    int d_filterSize = filter.size();
    int d_imWidth = exampleImg.getWidth();


    size_t datasize = sizeof(char)*exampleImg.getData()->size();
    size_t filterSize = sizeof(double)*(d_filterSize*d_filterSize);
    cl_mem d_inputImage, d_inputFilter, d_output;


    d_inputImage = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, datasize, &exampleImg.getData()->at(0), &err);
    if(err != CL_SUCCESS || d_inputImage == NULL){
        printf("Input Buffer Allocation Error");
        exit(-1);
    }

    d_inputFilter = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, filterSize, filter.getData(), &err);
    if(err != CL_SUCCESS || d_inputFilter == NULL){
        printf("Input Buffer Allocation Error");
        exit(-1);
    }

    d_output = clCreateBuffer(context, CL_MEM_READ_WRITE, datasize, NULL, &err);
    if(err != CL_SUCCESS || d_inputImage == NULL){
        printf("Output Buffer Allocation Error");
        exit(-1);
    }

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_inputImage);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_inputFilter);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_output);
    err |= clSetKernelArg(kernel, 3, sizeof(int), &d_imWidth);
    err |= clSetKernelArg(kernel, 4, sizeof(int), &d_filterSize);
    if(err != CL_SUCCESS){
        printf("Set kernel argument fail");
        exit(-1);
    }

    double startTime = chrono.get();

    //size_t globalWorkSize[1];
    //globalWorkSize[0] = ELEMENTS;
    size_t localWorkSize[2], globalWorkSize[2]; 
    localWorkSize[0] = 1;
    localWorkSize[1] = 1;
    globalWorkSize[0] = 973 - 5;
    globalWorkSize[1] = 1200 - 5;
    clEnqueueNDRangeKernel(cmdQueue, kernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);

    clEnqueueReadBuffer(cmdQueue, d_output, CL_TRUE, 0, datasize, &filteredImage.getData()->at(0), 0 , NULL, NULL);

    std::cout << "Total Time: " << chrono.get() - startTime << " sec" << std::endl;

    for(int i = 0; i < filteredImage.getData()->size(); ++i){
        //printf("%d\n", filteredImage[i]);
    }

    filteredImage.writeToDisk("test.png");

    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseMemObject(d_inputImage);
    clReleaseMemObject(d_inputFilter);
    clReleaseMemObject(d_output);
    clReleaseContext(context);
}
