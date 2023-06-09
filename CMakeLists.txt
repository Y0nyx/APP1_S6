cmake_minimum_required(VERSION 3.1)

project(AssetConv VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD          11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

set(CMAKE_BUILD_TYPE            Debug) # Change to RelWithDebInfo or Release

add_library(nanosvg     STATIC  src/nanosvg.c)
add_library(stb_image   STATIC  src/stb_image.c)

add_executable(asset_conv src/asset_conv.cpp)
target_link_libraries(asset_conv nanosvg stb_image pthread)

# Lab
#add_executable(lab_ex1 src/lab_ex1.cpp)
#add_executable(lab_ex2 src/lab_ex2.cpp)
#add_executable(lab_ex3 src/lab_ex3.cpp)
#add_executable(curl_E src/curl_E.cpp)
#add_executable(curl_H src/curl_H.cpp)
add_executable(curls_functions src/curls_functions.cpp)

#target_link_libraries(lab_ex1 pthread)
#target_link_libraries(lab_ex2 pthread)
#target_link_libraries(lab_ex3 pthread)
#target_link_libraries(curl_E pthread)
#target_link_libraries(curl_H pthread)

# Find OpenMP package
find_package(OpenMP)

if(OpenMP_CXX_FOUND)
    target_link_libraries(curls_functions PUBLIC pthread OpenMP::OpenMP_CXX)
else()
    target_link_libraries(curls_functions pthread)
endif()

# Lab - executable for Python part
#add_executable(lab_ex4 src/lab_ex4.cpp)
#target_link_libraries(lab_ex4 pthread)
