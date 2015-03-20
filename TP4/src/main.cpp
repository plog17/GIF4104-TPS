#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.hpp>

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
    std::cout << "Using platform: "<<default_platform.getInfo<CL_PLATFORM_NAME>()<<"\n";
}