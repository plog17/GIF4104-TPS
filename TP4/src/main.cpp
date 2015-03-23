#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#ifdef __APPLE__
    #include "OpenCL/opencl.h"
#else
    #include "CL/cl.h"
#endif

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
}