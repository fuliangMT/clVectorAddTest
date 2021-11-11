/**
 * @file oclCommon.h
 * @author fuliang.zhang
 * @brief OpenCL 常用函数封装声明
 * @version 0.1
 * @date 2021-11-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once

#include <iostream>
#include <cmath>
#include <stdio.h>
#include <CL/cl.h>
#include <CL/opencl.h>
//OPENCL Error Check

#define CHECK_OCL(call)                                                                         \                                                             
    do{                                                                                         \
        cl_int _err = call;                                                                     \
        if( _err != CL_SUCCESS){                                                                \
            printf("OpenCL error(%d) in File< %s > at line: %d.\n", _err, __FILE__, __LINE__);  \
                    return false;                                                               \
        }                                                                                       \
    }while(0)                                                                         

#define CHECK_OCL_ERROR(err) {                                          \
    if( err != CL_SUCCESS){                                             \
        printf("OpenCL error(%d) at line %d\n", err, __LINE__);         \
        return 0;                                                       \
    }                                                                   \
}

/**
 * @brief read the source code (like)
 * 
 * @param fileName 
 * @param retlength the file length
 * @return char* 
 */
char* ReadKernelSource(const char* fileName, size_t* retLength);


/**
 * @brief 查询设备参数信息clGetDeviceInfo ,可查询包括：CL_DEVICE_TYPE_CPU，CL_DEVICE_TYPE_GPU，CL_DEVICE_TYPE_ACCELERATOR，
 *            CL_DEVICE_TYPE_CUSTOM，CL_DEVICE_TYPE_DEFAULT， CL_DEVICE_TYPE_ALL;
 * 
 * @param devID 
 * @return true 
 * @return false 
 */
bool QueryDeviceInfo(cl_device_id devID);

/**
 * @brief 获取指定平台的info, 可查询包括：CL_PLATFORM_PROFILE, CL_PLATFORM_NAME, CL_PLATFORM_VENDOR,CL_PLATFORM_EXTENSIONS...
 * 
 * @param plateform 
 * @return true 
 * @return false 
 */
bool QueryPlatformInfo(cl_platform_id plateform);
/**
 * @brief Create a Context object
 * 
 * @param device set the value by clGetDeviceIDs()
 * @return cl_context and device_id
 */
cl_context CreateContext(cl_device_id* device_id);


/**
 * @brief Create a Command Queue object
 * 
 * @param context 
 * @param device_id 
 * @return cl_command_queue_info 
 */
cl_command_queue CreateCommandQueue(cl_context context, cl_device_id device_id);


/**
 * @brief Create a Program object
 * 
 * @param context 
 * @param device_id 
 * @param fileName 
 * @return cl_program 
 */
cl_program CreateProgram(cl_context context, cl_device_id device_id, const char* fileName);


/**
 * @brief create buffer
 * 
 * @param context 
 * @param memobj 
 * @param a 
 * @param b 
 * @param arraySize 
 * @return true 
 * @return false 
 */
bool CreateMemoryObject(cl_context context, cl_mem memobj[3], float* a, float* b, int arraySize);

/**
 * @brief release cl resource
 * 
 * @param context 
 * @param queue 
 * @param program 
 * @param kernel 
 * @param memobj 
 * @param memobj_len 
 */
void CleanUp(cl_context context, cl_command_queue queue, cl_program program, cl_kernel kernel, cl_mem memobj[], int memobj_len);


/**
 * @brief 生成随机数
 * 
 * @tparam T short/int/float/double/
 * @param pData 
 * @param dataLength 数组长度
 */
template<typename T>
void initArray(T* pData, int dataLength){
    ///srand((unsigned)  time(NULL));在外部设置随机数种子
    for (int i = 0; i < dataLength; i++)
    {
        if(typeid(pData[0]) == typeid(int) || typeid(pData[0]) == typeid(short)){
            pData[i] = rand() %100;
        }
        if(typeid(pData[0]) == typeid(float)  || typeid(pData[0]) == typeid(double)){
            pData[i] = rand() / float(RAND_MAX);//0.0~1.0
        }
    }
}

/**
 * @brief 数组误差验证，精度1e-6
 * 
 * @tparam T 
 * @param pData1 
 * @param pData2 
 * @param dataLength 数组长度，比较的两个数组长度必须一致
 * @return true 
 * @return false 
 */
template<typename T>
bool CompareArray(T* pData1, T* pData2, int dataLength){
    for (int i = 0; i < dataLength; i++)
    {
        if(fabs( pData1[i] - pData2[i]) >1e-6){
            return false;
        }
    }
    return true;
}

