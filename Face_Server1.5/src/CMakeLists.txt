cmake_minimum_required(VERSION 3.0)

project(Face_Server)

#message(${CMAKE_SOURCE_DIR})

FILE( GLOB ALL_SOURCES "*.cpp" "*.c" "*.cc" )
FILE( GLOB ALL_INCLUDES "*.hpp" "*.h" )

set(APP_SRC ${ALL_SOURCES} ${ALL_INCLUDES})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/src")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "release")
endif()

if(NOT MUDUO_PATH)
  set(MUDUO_PATH "/home/$ENV{USER}/software/muduo-master/build/${CMAKE_BUILD_TYPE}-install")
endif()

set(CXX_FLAGS
 -g
 # -DVALGRIND
 # -DMUDUO_STD_STRING
 -Wall
 -Wextra
 # -m32
 #-Werror
 -Wconversion
 -Wno-unused-parameter
 -Wold-style-cast
 -Woverloaded-virtual
 -Wpointer-arith
 -Wshadow
 -Wwrite-strings
 -march=native
 # -MMD
  -std=c++11
 -rdynamic
 )
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

find_path(Muduo_INCLUDE_DIR muduo "${MUDUO_PATH}/include")
find_path(Muduo_LIBRARY_DIR libmuduo_net.a "${MUDUO_PATH}/lib")
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${Muduo_LIBRARY_DIR})
message(STATUS ${Muduo_INCLUDE_DIR})
message(STATUS ${Muduo_LIBRARY_DIR})

include_directories(${Muduo_INCLUDE_DIR})
find_library(muduo_base muduo_base)
find_library(muduo_net muduo_net)
message(STATUS ${muduo_base})
message(STATUS ${muduo_net})


find_package(OpenCV REQUIRED)
find_package(Caffe REQUIRED)
find_package(Boost REQUIRED)
find_package(MySQL REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${Caffe_INCLUDE_DIRS})
add_definitions(${Caffe_DEFINITIONS})
include_directories(${MYSQL_INCLUDE_DIR})
include_directories(/home/llw/software/SeetaFaceEngine/FaceDetection/include)
include_directories(/home/llw/software/dlib-19.1)

LINK_DIRECTORIES(/home/llw/software/SeetaFaceEngine/FaceDetection/build)
LINK_DIRECTORIES(/usr/lib/x86_64-linux-gnu)
LINK_DIRECTORIES(/home/llw/software/dlib-19.1/build/dlib)

add_executable(Face_Server ${APP_SRC})

target_link_libraries(Face_Server ${muduo_net})
target_link_libraries(Face_Server ${muduo_base})
target_link_libraries(Face_Server pthread rt)
target_link_libraries(Face_Server ${OpenCV_LIBS})
target_link_libraries(Face_Server ${Caffe_LIBRARIES})
target_link_libraries(Face_Server ${MYSQL_LIBRARIES})
target_link_libraries(Face_Server libseeta_facedet_lib.so)
target_link_libraries(Face_Server libprotobuf.so)
target_link_libraries(Face_Server libdlib.so)







