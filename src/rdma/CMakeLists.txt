cmake_minimum_required(VERSION 3.18)

project(rrdma)

list(APPEND CMAKE_MODULE_PATH "/usr/local/lib/cmake")
list(APPEND CMAKE_PREFIX_PATH "/usr/local/lib/cmake")

find_package(remus REQUIRED)
message(STATUS "Using remus version ${remus_VERSION}")

find_package(Protobuf REQUIRED)
message(STATUS "Using protobuf version ${Protobuf_VERSION}")

set(Protobuf_IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/..)
#protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS metrics.proto colosseum.proto testutil.proto)  
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS message/message.proto)
install(FILES ${PROTO_HDRS} DESTINATION include/protos)

add_library(protos STATIC ${PROTO_SRCS})
add_library(rm::protos ALIAS protos)
target_include_directories(protos PUBLIC 
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>
                            $<INSTALL_INTERFACE:include>)
target_link_libraries(protos PUBLIC protobuf::libprotobuf)


add_executable(server node1/node1.cc)
add_executable(client node2/node2.cc)