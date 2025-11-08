#LinuxLightDipSensor/CMakeLists.txt

# Force cross compiler for BeagleY-AI if not already defined
if(NOT DEFINED CMAKE_C_COMPILER)
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR aarch64)
    set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
    #set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
endif()

# CMake Build Configuration for root of project
cmake_minimum_required(VERSION 3.18)
project(LinuxLightDipSensor VERSION 1.0 DESCRIPTION "Personal project" LANGUAGES C)

#This is for adding debug flag
set(CMAKE_BUILD_TYPE Debug)  

# Compiler options (inherited by sub-folders)
set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Werror -Wpedantic -Wextra)
add_compile_options(-fdiagnostics-color)

# Enable address sanitizer
# (Comment this out to make your code faster)
#add_compile_options(-fsanitize=address)
#add_link_options(-fsanitize=address)

# Enable PThread library for linking
add_compile_options(-pthread)
add_link_options(-pthread)

# What folders to build
add_subdirectory(hal)  
add_subdirectory(app)

