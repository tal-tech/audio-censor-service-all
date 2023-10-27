cmake_minimum_required(VERSION 3.10)
project(service-common)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti")
set(SOURCE_FILES
    service-common.cpp
    service-common.h
    base-thread-pool.cpp
    base-thread-pool.h
    custom_thread.h
    custom_thread_delegate.h
    custom_thread_impl.cpp
    custom_thread_impl.h)

add_library(service-common SHARED ${SOURCE_FILES})

target_include_directories(service-common PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

target_link_libraries(service-common kafka-cli configer eureka
    apollo base curl ${Boost_LIBRARIES} datatrans)