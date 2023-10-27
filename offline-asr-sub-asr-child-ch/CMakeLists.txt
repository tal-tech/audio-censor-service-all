cmake_minimum_required(VERSION 3.10)

set(SERVICE_NAME main)

project(${SERVICE_NAME})

set (CPP_SERVICE_MAJOR_VERSION 0)
set (CPP_SERVICE_MINOR_VERSION 8)
set (CPP_SERVICE_PATCH_VERSION 1)
set (CPP_SERVICE_VERSION ${CPP_SERVICE_MAJOR_VERSION}.${CPP_SERVICE_MINOR_VERSION}.${CPP_SERVICE_PATCH_VERSION})

set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/package")

######################################################################
#全局编译标志
######################################################################

SET (CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,$ORIGIN/../lib" )

SET (CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,$ORIGIN/../lib" )

set(CMAKE_SKIP_BUILD_RPATH TRUE)

set(BUILD_SHARED_LIBS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g")

######################################################################
#全局链接和包含
######################################################################

# link_directories(${CMAKE_SOURCE_DIR}/kafka/cppkafka/lib)
# link_directories(${CMAKE_SOURCE_DIR}/alg/lib)
include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB TAlASR_LIB ${PROJECT_SOURCE_DIR}/lib/lib*.*)

# find_package(Boost REQUIRED COMPONENTS filesystem log system)

#################################################################L#####
#全局编译输出指定
######################################################################

IF(CMAKE_BUILD_TYPE MATCHES Debug)
set(CMAKE_VERBOSE_MAKEFILE ON)
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/debug/lib/${BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/debug/lib/${BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/debug/bin/${BUILD_TYPE})


# #set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/${BUILD_TYPE})
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/${BUILD_TYPE})
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${BUILD_TYPE})


######################################################################
#子工程目录
######################################################################

# add_subdirectory(breakpad)
add_subdirectory(chrome-base)
#add_subdirectory(configer)
#add_subdirectory(curl-tool)
#add_subdirectory(grpc)
# add_subdirectory(ffmpeg)
# add_subdirectory(jsoncpp)
# add_subdirectory(kafka)
# add_subdirectory(datawork-file-trans)
# #add_subdirectory(mysql)
# add_subdirectory(service-common)
# add_subdirectory(service-custom)
# add_subdirectory(service-http-custom)
# #add_subdirectory(service-websocket-custom)
# add_subdirectory(websocket)

######################################################################
#算法sdk编译对象
######################################################################

######################################################################
#http service编译对象
######################################################################

add_executable(main
    main.cpp)
target_link_libraries(main
    base
    #fst
    pthread
    ${TAlASR_LIB}
    )

# target_link_libraries(http_service
#     fst
#     libvad
#     libwebrtc
#     tlvkaldi
#     breakpad_client
#     service-common
#     service-custom
#     service-http-custom
#     ${Boost_LIBRARIES})

######################################################################
#websocket service编译对象
######################################################################

#add_executable(ws_service
#    ws-service-main.cpp)
#
##add_dependencies(ws_service websocketpp)
#
#target_link_libraries(ws_service
#    breakpad_client
#    service-common
#    service-custom
#    service-websocket-custom
#    ${Boost_LIBRARIES})

######################################################################
#程序发布设置
######################################################################

install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ DESTINATION bin)
install(DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/ DESTINATION lib)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/kafka/cppkafka/lib/ DESTINATION lib)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/alg/resource/ DESTINATION bin/res)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/alg/lib/ DESTINATION lib)
#install(DIRECTORY config/ DESTINATION bin/config)