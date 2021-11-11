#include "oclCommon.h"
#include <iostream>
using namespace std;
int main(){

    cl_int errNum;
    cl_platform_id plateform;
    cl_uint num_platform;
    cl_uint numDevice;
    cl_device_id devID;

    errNum = clGetPlatformIDs( 1,           //num_entries: 可以添加到platform的条目数量
                            &plateform,     //platforms:返回找到的ocl平台的列表
                            &num_platform   //num_platforms: 返回可用的ocl的平台数量，如果是NULL，则忽略该参数
                            );
    CHECK_OCL(errNum);
    cout<<"num_platform: "<<num_platform<<endl;

    //获取平台信息
    if( !QueryPlatformInfo(plateform) ){
        cout<<"Run  QueryPlatformInfo Failed!!! "<<endl;
        return 0;
    }

  
    //获取设备信息,设备ID
    errNum = clGetDeviceIDs(plateform,          //指明要查询的平台
                        CL_DEVICE_TYPE_ALL,     //device_type 
                        1,                      //num_entries 
                        &devID,                 //devices 
                        &numDevice              //num_devices 
                        );
    CHECK_OCL(errNum);
    cout<<"num_Device: "<<numDevice<<endl;

    //获取相关设备信息
    if( !QueryDeviceInfo(devID) ){
        cout<<"Run  QueryDeviceInfo Failed!!! "<<endl;
        return 0;
    }


    /**************************  Test VectorAdd Code ************************/
    const char* kernel_file_name = "kernelAdd.cl"; //cl_kernel文件名
    const char* kernel_func_name = "vectorAdd"; //cl_kernel函数名字
    const int arrayLen=10;
    float* h_a = new float[arrayLen];
    float* h_b = new float[arrayLen];
    float* h_c = new float[arrayLen];
    float* result = new float[arrayLen];
  
    srand((unsigned int)  time(NULL));
    initArray<float> (h_a, arrayLen);
    initArray<float> (h_b, arrayLen);
    for (int i = 0; i < arrayLen; i++)
    {
        h_c[i] = h_a[i] + h_b[i];
        printf("h_a:%f  h_b:%f\n", h_a[i], h_b[i]);
    }
    
   

    cl_command_queue commandQueue = 0;
    cl_program program = 0;
    cl_kernel kernel = 0;
    int cntMemObj = 3;
    cl_mem memObjects[cntMemObj] = {0,0,0};//
    //创建上下文
    cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)plateform, 0};
    cl_context context =  clCreateContext(properties, numDevice,  &devID, NULL, NULL, NULL );
    if(context == NULL){
         cout<<"Faield to create Context !!! "<<endl;
         return 0;
    }
    cout<<"\ncreate context done. "<<endl;

    //创建命令队列
    commandQueue = CreateCommandQueue(context, devID);
    if(commandQueue == NULL){
        cout<<"\nRun CreateCommandQueue Failed !!!\n "<<endl;
        CleanUp(context, commandQueue, program, kernel, memObjects,cntMemObj);
        return false;
    }
    //创建OpenCL 程序
    program = CreateProgram(context, devID, kernel_file_name);
    if(program == NULL){
        cout<<"\nRun CreateProgram Failed !!!\n "<<endl;
        return false;
    }
    //创建OpenCL内核
    kernel = clCreateKernel(program, "vectorAdd", NULL);
    if(kernel == NULL){
        cout<<"\nFaield to create kernel !!!\n "<<endl;
        return false;
    }
    

    //创建内存对象
    if( !CreateMemoryObject(context, memObjects, h_a, h_b, arrayLen) ){
        cout<<"\nRun CreateMemoryObject Failed!!!\n "<<endl;
        return false;
    }
    //设置内核对象
    errNum = clSetKernelArg(kernel,             //cl_kernel kernel
                            0,                  //cl_uint arg_index: 内核参数索引 0, 1, 2
                            sizeof(cl_mem),     //size_t arg_size: 内核参数大小，内存对象-sizeof(cl_mem)
                            &memObjects[0]);    //const void * arg_value: 内核参数指针
    errNum |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &memObjects[1]);  
    errNum |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &memObjects[2]);  
    if(errNum != CL_SUCCESS){
        cout<<"\nRun clSetKernelArg Failed!!!\n "<<endl;
        return false;
    }
   
    //执行内核
    size_t global_work_size[1] = {arrayLen};  //全局工作项的大小
    size_t local_work_size[1] = {1}; //work-group 内的item大小                             
    cl_uint num_events_in_wait_list = 0;
    cl_event *event_wait_list = NULL;
    cl_event* event = NULL;
    errNum = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, global_work_size, local_work_size, num_events_in_wait_list, event_wait_list, event);
    if(errNum != CL_SUCCESS){
        cout<<"\nRun clEnqueueNDRangeKernel Failed!!!\n "<<endl;
        return false;
    }

    //将计算结果拷贝回主机
    //---如果blocking_read 为CL_TRUE，即读取命令为阻塞，则clEnqueueReadBuffer()函数在读取缓冲区数据并将其复制到ptr 指向的内存之前不会返回。---//
    errNum = clEnqueueReadBuffer(commandQueue, memObjects[2], CL_TRUE, 0, sizeof(float)*arrayLen, result, num_events_in_wait_list,  event_wait_list, event);
     if(errNum != CL_SUCCESS){
        cout<<"\nRun clEnqueueReadBuffer Failed!!!\n "<<endl;
        return false;
    }


    for (int i = 0; i < arrayLen; i++)
    {
        cout<<"result: idx="<<i<<"  : "<<result[i]<<endl;
    }
    if( !CompareArray(h_c, result, arrayLen)){
        cout<<"\n>>> Test OpenCL VectorAdd Failed!!!\n "<<endl;
    }else{
        cout<<"\n>>> Test OpenCL VectorAdd Success!!!\n "<<endl;
    }

    return 0;
}
