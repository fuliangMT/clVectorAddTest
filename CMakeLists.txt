# ########################################################################
#   DESC: Compile OpenCL program CMakeLists.txt
#   Author:fuliang.zhang
#   Data:2021.11.10
#   
# ########################################################################

# cmake 最低版本要求# cmake 最低版本要求
cmake_minimum_required(VERSION 3.0.0)

#message( [STATUS|WARNING|AUTHOR_WARNING|FATAL_ERROR|SEND_ERROR]  "message to display" ...)
message("cmake version: ${CMAKE_VERSION}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

if(WIN32)
  message("system: WIN32")
elseif(UNIX AND NOT APPLE AND NOT ANDROID)
  message("system: LINUX")
elseif(APPLE)
  message("system: APPLE")
else()
  message("system: else")
endif()


#批量注释   #[[...]]


set( PRO_NAME   CL_VectorAdd )
project( ${PRO_NAME} )
#设置源文件目录
set( SRC_FILES  oclCommon.cpp  vectorAdd_Test.cpp)
set( EXTRA_FILES    kernelAdd.cl )

#设置编译选项
# -Wno-unused : close unused variable warning
# Compiler flags
#[[
if (CMAKE_COMPILER_IS_GNUCC)
    set (CMAKE_CXX_FLAGS "-D_REETRANT -Wall -Wextra -pedantic -Wno-long-long")
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
   	    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -O0")
	elseif( CMAKE_BUILD_TYPE STREQUAL "Release" )
	    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -O3 -fno-strict-aliasing")
	endif ()
endif (CMAKE_COMPILER_IS_GNUCC)
]]

set( CMAKE_CXX_FLAGS "-Wall  -g -I. ")
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -l OpenCL")

#查找libOpenCL
find_package(OpenCL REQUIRED)
include_directories( ${OpenCL_INCLUDE_DIRS} )

message(STATUS "SOURCE DIR: ${PROJECT_SOURCE_DIR}")
message(STATUS "OpenCL_FOUND: ${OpenCL_FOUND}")
message(STATUS "OpenCL_INCLUDE_DIRS: ${OpenCL_INCLUDE_DIRS}")
message(STATUS "OpenCL_LIBRARIES: ${OpenCL_LIBRARIES}")
message(STATUS "OpenCL_VERSION_STRING: ${OpenCL_VERSION_STRING}")

#设置可执行文件路径，<<<为了读取opencl.cl文件路径建议设置>>>
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

#设置库文件输出目录
#set( ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/../lib )

#指定生成目标
add_executable( ${PRO_NAME}  ${SRC_FILES}  ${EXTRA_FILES})
target_link_libraries( ${PRO_NAME}  ${OpenCL_LIBRARIES})
