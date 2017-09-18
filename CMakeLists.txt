cmake_minimum_required(VERSION 3.2)
project(libhtsp)

set(CMAKE_CXX_STANDARD 11)

find_package(Boost REQUIRED COMPONENTS system thread)

add_subdirectory(pvr.hts/lib/libhts)

include_directories(${Boost_INCLUDE_DIR})

set(SOURCE_FILES src/HTSPConnection.cpp src/HTSPConnection.h)
add_library(libhtsp ${SOURCE_FILES})
target_link_libraries(libhtsp hts ${Boost_LIBRARIES})

