#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "lodepng.h"
#include "PngImage.h"
#include "Filter.h"
#include "Tokenizer.hpp"
#include <sys/time.h>

#ifdef __APPLE__
    #include "OpenCL/cl.hpp"
#else
    #include "CL/cl.h"
#endif

using namespace std;

const char* pathToImage;
const char* pathToFilter;
const char* pathToConvolve;


//Aide pour le programme
void usage(char* inName) {
    cout << endl << "Utilisation> " << inName << " fichier_image fichier_noyau [fichier_convolve=convolve.cl]" << endl;
    exit(1);
}

/* Troube le premier GPU ou CPU et le retourne */
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

/* Compile le programme dans le fichier avec possibilité d'ajouter des options */
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename, const char* options) {

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
    err = clBuildProgram(program, 0, NULL, options, NULL, NULL);
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

int loadArguments(int argc, char **argv) {
    if(argc<3 || argc>4){
        std::cout<<"Utilisation incorrecte."<<std::endl;
        usage("TP4");
    }
    else if (argc==3){
        pathToImage=argv[1];
        pathToFilter=argv[2];
        pathToConvolve="src/convolve.cl";
    }
    else{
        pathToImage=argv[1];
        pathToFilter=argv[2];
        pathToConvolve=argv[3];
    }
}

double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

int main(int argc, char** argv) {
    /* OpenCL structures */
    cl_device_id device;
    cl_context context;
    cl_int i, j, err;
    size_t local_size, global_size;
    loadArguments(argc, argv);

    /* Préparations des données */
    PngImage exampleImg(pathToImage);
    PngImage filteredImage(*exampleImg.getData(), exampleImg.getWidth(), exampleImg.getHeight());
    Filter filter(pathToFilter);
    int d_filterSize = filter.size();
    int d_imWidth = exampleImg.getWidth();
    int d_imHeight = exampleImg.getHeight();

    /* Création du device et du context*/
    device = create_device();
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if(err < 0) {
        perror("Couldn't create a context");
        exit(1);
    }

    /* Compilation du programme*/
    int localSize = 32;
    cl_program program;
    char options[50];
    sprintf(options, "-DBLOCK_SIZE=%d", localSize);
    program = build_program(context, device, pathToConvolve, options);

    /* Préparation du Kernel*/
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

    /* Création des buffers */
    size_t datasize = sizeof(char)*exampleImg.getData()->size();
    size_t filterSize = sizeof(float)*(d_filterSize*d_filterSize);
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

    /* Création des arguments pour le Kernel*/
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_inputImage);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_inputFilter);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_output);
    err |= clSetKernelArg(kernel, 3, sizeof(int), &d_imWidth);
    err |= clSetKernelArg(kernel, 4, sizeof(int), &d_imHeight);
    err |= clSetKernelArg(kernel, 5, sizeof(int), &d_filterSize);
    if(err != CL_SUCCESS){
        printf("Set kernel argument fail");
        exit(-1);
    }

    double startTime = get_wall_time();

    /* Formatage du NDRange : 2D, local = 32x32 (architecture),  global= multiple de 32 en fonction de la taille de l'image*/
    size_t localWorkSize[2], globalWorkSize[2];
    localWorkSize[0] = localSize;
    localWorkSize[1] = localSize;
    int globalW = localSize * (exampleImg.getWidth()/localSize) + localSize;
    int globalH = localSize * (exampleImg.getHeight()/localSize) + localSize;
    globalWorkSize[0] = globalW;
    globalWorkSize[1] = globalH;
    clEnqueueNDRangeKernel(cmdQueue, kernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);

    clEnqueueReadBuffer(cmdQueue, d_output, CL_TRUE, 0, datasize, &filteredImage.getData()->at(0), 0 , NULL, NULL);

    std::cout << "Total Time: " << get_wall_time() - startTime << " sec" << std::endl;

    filteredImage.writeToDisk("output.png");

    /* Clean Up*/
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseMemObject(d_inputImage);
    clReleaseMemObject(d_inputFilter);
    clReleaseMemObject(d_output);
    clReleaseContext(context);
}
