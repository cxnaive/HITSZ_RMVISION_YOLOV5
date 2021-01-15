﻿CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(HITSZ-RM-CV)
SET(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -D_MWAITXINTRIN_H_INCLUDED -O3")
# cuda
find_package(CUDA REQUIRED)
include_directories(/usr/local/cuda/include)
link_directories(/usr/local/cuda/lib64)

#Include
include_directories(${PROJECT_SOURCE_DIR}/energy/include)
include_directories(${PROJECT_SOURCE_DIR}/armor/include)
include_directories(${PROJECT_SOURCE_DIR}/net/include)
include_directories(${PROJECT_SOURCE_DIR}/others/include)
include_directories(${PROJECT_SOURCE_DIR}/config/include)
include_directories(${PROJECT_SOURCE_DIR}/com/include)
find_package(OpenCV 4 REQUIRED)
# find_package(glog REQUIRED)
#Src
file(GLOB_RECURSE sourcefiles 
"others/**/*.h" "others/**/*.cpp"
"energy/**/*.h" "energy/**/*.cpp" 
"net/**/*.cpp" "net/**/*.h" 
"config/**/*.cpp" "config/**/*.h" 
"com/**/*.h"  "com/**/*.cpp" "com/**/*.cc"
"armor/**/*.h" "armor/**/*.cpp"
)

file(GLOB_RECURSE camfiles "others/**/*.h" "others/**/*.cpp" "config/src/rmconfig.cpp" "config/include/rmconfig.h")

cuda_add_library(myplugins SHARED net/src/yololayer.cu)
target_link_libraries(myplugins nvinfer cudart)

add_executable(rmcv main.cpp ${sourcefiles})
add_executable(calibrate calibrate_get_imgs.cpp ${camfiles})
link_directories(${OpenCV_LIBRARY_DIRS})

target_link_libraries(rmcv ${OpenCV_LIBS} -lglog -lgxiapi -lpthread nvinfer cudart myplugins)
target_link_libraries(calibrate  ${OpenCV_LIBS} -lglog -lgxiapi  -lpthread)