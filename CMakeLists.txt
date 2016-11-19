cmake_minimum_required(VERSION 3.3)
project(roughDNS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
#include_directories( /usr/local/include/ldns )
#add_custom_target(roughDNS COMMAND make -C ${roughDNS_SOURCE_DIR}
                           #CLION_EXE_DIR=${PROJECT_BINARY_DIR})
set(SOURCE_FILES src/main.cpp src/main.h
        src/argParser.cpp src/argParser.h
        src/server.cpp src/server.h
        src/dnsMsg.cpp src/dnsMsg.h
        src/rr.cpp src/rr.h)
#file(GLOB SOURCE_FILES
#        src/*.cpp
#        src/*.h)

add_executable(roughDNS ${SOURCE_FILES})
target_link_libraries( roughDNS  /usr/local/lib/libldns.so )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/")

include_directories( routhDNS ${BIND_INCLUDE_DIR} )
target_link_libraries( roughDNS ${BIND_LIBRARY} )