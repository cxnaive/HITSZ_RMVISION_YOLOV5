﻿CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(HITSZ-RM-CV)
SET(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(use_u2_camera false)
set(use_dh true)
set(use_hk false)
add_compile_options(-O3)

# OpenCV
find_package(OpenCV 4.5 REQUIRED)
# link_directories(${OpenCV_LIBRARY_DIRS})

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
include_directories(/opt/MVS/include)
include_directories(../tiny-tensorrt)

#Link
link_directories(/opt/MVS/lib/64)
link_directories(/opt/MVS/lib/aarch64)
link_directories(../tiny-tensorrt/build)


if (use_u2_camera)
    message(STATUS "Linking u2 camera lib on linux...")
    message(STATUS "u2_camera_root $ENV{GENICAM_ROOT_V2_3}")
    link_directories($ENV{GENICAM_ROOT_V2_3}/bin/Linux64_x64)
    link_directories($ENV{GENICAM_ROOT_V2_3}/bin/Linux64_x64/GenApi/Generic)
    link_libraries(-ldhgentl -lpthread)
    link_libraries(-ldx_image_proc -lGCBase_gcc40_v2_3 -lGenApi_gcc40_v2_3 -llog4cpp_gcc40_v2_3 -lLog_gcc40_v2_3 -lMathParser_gcc40_v2_3)
endif ()

if (use_dh)
    message(STATUS "Linking DaHeng camera lib on linux...")
    add_definitions("-DUSE_DH")
    link_libraries(-lgxiapi)
endif ()

if (use_hk)
    message(STATUS "Linking HKROBOT camera lib on linux...")
    add_definitions("-DUSE_HK")
    link_libraries(-lMvCameraControl)
endif ()
# Resources

file(GLOB_RECURSE resources "${PROJECT_SOURCE_DIR}/resources/*")
message(STATUS "Copying resources file(s): ${resources}")
file(COPY ${resources} DESTINATION ${CMAKE_BINARY_DIR}/)

# find_package(glog REQUIRED)
#Src
file(GLOB_RECURSE sourcefiles 
"others/**/*.cpp"
"energy/**/*.cpp" 
"net/**/*.cpp"
"config/**/*.cpp"
"com/**/*.cpp" "com/**/*.cc" "com/**/*.c"
"armor/**/*.cpp"
)

file (GLOB_RECURSE calfiles
"others/**/*.cpp"
"config/src/rmconfig.cpp"
)


add_executable(rmcv main.cpp ${sourcefiles})
add_executable(calibrate calibrate_get_imgs.cpp ${calfiles})
#add_executable(mcusend test_mcu_send.cpp ${sourcefiles})


target_link_libraries(rmcv ${OpenCV_LIBS} -lglog -lpthread  libtinytrt.so nvinfer nvinfer_plugin nvparsers nvonnxparser)
target_link_libraries(calibrate  ${OpenCV_LIBS} -lglog -lpthread)
#target_link_libraries(mcusend  ${OpenCV_LIBS} -lglog -lgxiapi -lpthread nvinfer cudart myplugins)
