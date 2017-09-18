cmake_minimum_required(VERSION 3.2)
project(htsp)

set(CMAKE_CXX_STANDARD 11)

find_package(Boost REQUIRED COMPONENTS system thread log)

include_directories(${Boost_INCLUDE_DIR})

# Dynamic Boost Log Library
add_definitions(-DBOOST_LOG_DYN_LINK)

# Library
set(SOURCE_FILES src/HTSPConnection.cpp src/HTSPConnection.h src/HTSMessage.cpp src/HTSMessage.h)
add_library(htsp STATIC ${SOURCE_FILES})
target_link_libraries(htsp ${Boost_LIBRARIES})


# Sample Client
set(CLIENT_SOURCE_FILES client/client.cpp)
add_executable(client ${CLIENT_SOURCE_FILES})
target_link_libraries(client htsp)
