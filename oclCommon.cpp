/**
 * @file oclCommon.cpp
 * @author fuliang.zhang
 * @brief  OpenCL 常用函数封装定义( include "oclCommon.h")
 * @version 0.1
 * @date 2021-11-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "oclCommon.h"
using namespace std;

char* ReadKernelSource(const char* fileName, size_t* retLength){
     
    int retlen = 0;//read source code length
    size_t fileLength = 0;// source code length
    char* sourceString;//source code 
    FILE* fw = nullptr;
    fw = fopen(fileName, "rb");
    if(nullptr == fw){
        printf("%s at line:%d. Open file< %s > failed!!!\n", __FILE__, __LINE__, fileName);
        return nullptr;
    }
    fseek(fw, 0, SEEK_END);
    fileLength = ftell(fw);
    fseek(fw, 0, SEEK_SET);

    sourceString = (char*)malloc(sizeof(char) * (fileLength+1));
    sourceString[0] = '\0';// 
    retlen = fread(sourceString, fileLength, sizeof(char), fw);
    if(0 == retlen){
        printf("%s at %d: Read file< %s > failed!!!\n", __FILE__, __LINE__, fileName);
        return nullptr;
    }
    fclose(fw);
    sourceString[fileLength] = '\0';
    *retLength = fileLength;
    return sourceString;
}



bool QueryDeviceInfo(cl_device_id devID){
    cout<<endl<<"-----------The Device info -----------"<<endl;
    //获取设备驱动程序版本
    size_t size;
    cl_int err;
    err = clGetDeviceInfo(devID, CL_DEVICE_VERSION, 0, nullptr, &size);//可提前获取size
    CHECK_OCL(err);
    char* deviceVersion= new char[size];
    err = clGetDeviceInfo(devID, CL_DEVICE_VERSION, size, deviceVersion, nullptr);
    CHECK_OCL(err);
    cout<<"device_verison: "<<deviceVersion<<endl;

    //获取设备信息-设备名字
    char deviceName[128];
    err = clGetDeviceInfo(devID, CL_DEVICE_NAME, 128, deviceName, NULL);
    CHECK_OCL(err);
    cout<<"device_name："<<deviceName<<endl;

    //获取设备驱动程序版本
    char driverVerison[128];
    err = clGetDeviceInfo(devID, CL_DRIVER_VERSION, 128, driverVerison, NULL);
    CHECK_OCL(err);
    cout<<"driver_verison:"<<driverVerison<<endl;

    char c_version[128];
    err = clGetDeviceInfo(devID, CL_DEVICE_OPENCL_C_VERSION, 128, c_version, NULL);
    CHECK_OCL(err);
    cout<<"OPENCL_C_VERSION:"<<c_version<<endl;
    
    //获取设备信息-每个上下文可以创建的设备队列最大数目
    cl_uint maxQueueSize=0;
    err = clGetDeviceInfo(devID, CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE, sizeof(cl_uint), &maxQueueSize, NULL);
    CHECK_OCL(err);
    cout<<"maxQueueSize: "<<maxQueueSize<<endl;


    //获取设备信息-设备最大时钟频率
    cl_uint maxFrequency;
    err = clGetDeviceInfo(devID, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), &maxFrequency, NULL);
    CHECK_OCL(err);
    cout<<"maxFrequency(MHz): "<<maxFrequency<<endl;

    //获取设备信息-全局内存缓存大小
    cl_ulong    maxGlobalMemSize=0;
    err = clGetDeviceInfo(devID, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &maxGlobalMemSize, NULL);
    CHECK_OCL(err);
    cout<<"maxGlobalMemSize(MB): "<<(float)maxGlobalMemSize/1024/1024<<endl;

    
    //获取设备信息-常量缓存大小
    cl_ulong    maxConstantBufferSize=0;
    err = clGetDeviceInfo(devID, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(cl_ulong), &maxConstantBufferSize, NULL);
    CHECK_OCL(err);
    cout<<"maxConstantBufferSize(KB): "<<(float)maxConstantBufferSize/1024<<endl;

    //获取设备信息-局部内存大小
    cl_ulong    maxLocalMemSize=0;
    err = clGetDeviceInfo(devID, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &maxLocalMemSize, NULL);
    CHECK_OCL(err);
    cout<<"maxLocalMemSize(KB): "<<(float)maxLocalMemSize/1024<<endl;
    cout<<endl;
    return true;
}


bool QueryPlatformInfo(cl_platform_id plateform)
{
    cout<<endl<<"-----------The Plateform info -----------"<<endl;
    char name[128];
    cl_int err;

    err = clGetPlatformInfo(plateform,  //指明要查询的平台
                            CL_PLATFORM_NAME,   //cl_platform_info ：枚举常量，标识要查询的平台信息，
                            128,   //param_value_size，指定param_value指向的内存大小
                            name,   //param_value，指向内存位置的指针，存储查询的cl_platform_info信息
                            NULL    //param_value_size_ret，返回param_value查询的实际大小，如果是NULL,则将其忽略
                            );
    CHECK_OCL_ERROR(err);
    cout<<"Platform_Name: "<<name<<endl;

     char platProfile[128];
     CHECK_OCL ( clGetPlatformInfo(plateform, CL_PLATFORM_PROFILE, 128, platProfile, NULL)  );
    cout<<"Platform_Profile: "<<platProfile<<endl;

    char platVersion[128];
    err = clGetPlatformInfo(plateform, CL_PLATFORM_VERSION, 128, platVersion, NULL);
    CHECK_OCL(err);
    cout<<"Platform_Version: "<<platVersion<<endl;

    char platVendor[128];
    err = clGetPlatformInfo(plateform, CL_PLATFORM_VENDOR, 128, platVendor, NULL);
    CHECK_OCL(err);
    cout<<"Platform_Vendor: "<<platVendor<<endl;
    cout<<"-----------------------------------"<<endl;
    return true;
}


cl_context CreateContext(cl_device_id* device_id) {
    cl_context context = NULL;
    cl_int err;
    cl_uint numPlatForm;
    cl_platform_id platform_id;
    err = clGetPlatformIDs(1, &platform_id, &numPlatForm);
    if(err != CL_SUCCESS || numPlatForm <=0 ){
        printf("Failed to get any plateforms!!!\n");  
        return NULL; 
    }
    //cl_device_id device_id;
    err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, device_id, NULL);
    if(err != CL_SUCCESS ){
        printf("Failed to get GPU Device!!!, Trying GPU...\n");  
        err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_CPU, 1, device_id, NULL);
        if(err != CL_SUCCESS ){
              printf("Failed to get CPU Device!!!\n");  
              return NULL;
        }
    }
    
    context = clCreateContext(NULL, //cl_context_properties = NULL
                            1,  // number of devices in devicelist
                            device_id, //the device_id list
                            NULL, //pointer to error callback func
                            NULL, //the argument pass to callback func
                            &err  //return code
    );
                
    if(err != CL_SUCCESS ){
        printf("Failed to create context!!!\n");  
        return NULL; 
    }

    return context;
}


cl_command_queue CreateCommandQueue(cl_context context, cl_device_id device_id){
    cl_command_queue commndQueue=NULL;
    cl_int err;
    //clCreateCommandQueueWithProperties
    //commndQueue = clCreateCommandQueue(context, device_id,  0, &err);
     commndQueue = clCreateCommandQueueWithProperties(context, device_id,  0, &err);
    if(err != CL_SUCCESS || commndQueue == NULL ){
        printf("Failed to create command queue!!!\n");  
        return NULL; 
    }
    return commndQueue;
}


cl_program CreateProgram(cl_context context, cl_device_id device_id, const char* fileName){
    cl_program program=NULL;
    size_t fileLength;
    cl_int err;
    char* sourceCode = ReadKernelSource(fileName, &fileLength);
    program = clCreateProgramWithSource(context, 1, (const char**)&sourceCode, &fileLength, NULL);
    if(program == NULL ){
        printf("Failed to create program from source code!!!\n");  
        return NULL; 
    }

    err = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    if(CL_SUCCESS != err ){
        printf("Failed to build program !!!\n");  

        char errlog[1024];
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(errlog), errlog, NULL);
        printf("Build error log in kernel:\n %s\n", errlog);
        clReleaseProgram(program);
        return NULL; 
    }

    return program;
}


bool CreateMemoryObject(cl_context context, cl_mem memobj[3], float* a, float* b, int arraySize)
{
    //OpenCL的实现将使用由host_ptr 指向的内存来作为cl_mem对象的存储，将host_ptr 指向的内容缓冲（cache）到对应的设备上,，
    //在kernel执行的过程中就可以使用这些buffer。
    //CL_MEM_USE_HOST_PTR : 主机和设备使用同一块内存，host_ptr指针不能为空，类似将主机上的内存块映射到设备上供设备使用
    //CL_MEM_COPY_HOST_PTR: 在设备端分配内存，并将hsot_ptr指针指向的内存，拷贝到设备内存上去 --,和CL_MEM_ALLOC_HOST_PTR配合使用
    //CL_MEM_ALLOC_HOST_PTR : 应用希望opencl实现能从主机可访问内存中分配内存， 与CL_MEM_USE_HOST_PTR互斥
    //
    memobj[0]=clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(float) * arraySize, a, NULL  ); //CL_MEM_READ_ONLY设备只读，， CL_MEM_HOST_PTR->
    memobj[1]=clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(float) * arraySize, b, NULL  );
    memobj[2]=clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * arraySize, NULL, NULL  );
    if(memobj[0] == NULL || memobj[1] == NULL || memobj[2] == NULL){
         printf("Failed to create memory !!!\n");  
         return false;
    }
    return true;
}


void CleanUp(cl_context context, cl_command_queue queue, cl_program program, cl_kernel kernel, cl_mem memobj[], int memobj_len){
    for (int  i = 0; i < memobj_len; i++)
    {
        if(memobj[i] != 0)
        clReleaseMemObject(memobj[i]);
    }
    if(queue != NULL){
        clReleaseCommandQueue(queue);
    }
    if(kernel != NULL){
        clReleaseKernel(kernel);
    }
    if(program != NULL){
        clReleaseProgram(program);
    }
    if(context != NULL){
        clReleaseContext(context);
    }
}

