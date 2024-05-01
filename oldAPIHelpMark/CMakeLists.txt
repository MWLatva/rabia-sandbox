### Standard cmake stuff: minimum version, project name

cmake_minimum_required(VERSION 3.18)
project(IHT_Rome LANGUAGES CXX VERSION 0.0.0)

### Flags that the user can override

set(LOG_LEVEL "INFO" CACHE STRING "Log level options include TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL, and OFF")
set(CXX_STANDARD 23 CACHE STRING "Uses C++20 or 23")

### Make sure we have the required packages

find_package(Protobuf REQUIRED)
message(STATUS "Using protobuf version ${Protobuf_VERSION}")
# NB: Our custom rule for finding ibverbs and rdmacm prints a message.  It's in
#     the `cmake` subfolder, so we need to add that to the module path.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(RDMA REQUIRED)

### Make sure the LOG_LEVEL is correct

set(LOG_LEVELS "TRACE;DEBUG;INFO;WARN;ERROR;CRITICAL;OFF")
foreach(L ${LOG_LEVELS})
    if("${LOG_LEVEL}" STREQUAL "${L}")
        set(CORRECT_LOG_LEVEL TRUE)
    endif()
endforeach()
if(NOT DEFINED CORRECT_LOG_LEVEL)
    message(FATAL_ERROR "LOG_LEVEL must be one of ${LOG_LEVELS}") 
endif()
message(STATUS "Using LOG_LEVEL=${LOG_LEVEL}")

### Make sure the C++ version is correct

set(CXX_STANDARDS "20;23")
foreach(S ${CXX_STANDARDS})
    if("${CXX_STANDARD}" STREQUAL "${S}")
        set(CORRECT_CXX_STANDARD TRUE)
    endif()
endforeach()
if(NOT DEFINED CORRECT_CXX_STANDARD)
    message(FATAL_ERROR "CXX_STANDARD must be one of ${CXX_STANDARDS}") 
endif()
message(STATUS "Using CXX_STANDARD=${CXX_STANDARD}")
set(CMAKE_CXX_STANDARD ${CXX_STANDARD})

### Declare the subfolders to recurse into before building the executable
add_subdirectory(protos)

### Declare the things that get built to create rome

# NB: This must come before target_compile_definitions(rome ...) and
#     target_link_libraries(rome ...)


### Link with dependencies
find_package(fmt 8.1...<8.2 REQUIRED) # defines fmt::fmt
include_directories(${fmt_INCLUDE_DIRS})
message(STATUS "Using fmt version ${fmt_VERSION}")

find_package(spdlog 1.9...<1.10 REQUIRED) #defines spdlog::spdlog
message(STATUS "Using spdlog version ${spdlog_VERSION}")

# Run a loop and compile separate executables for main, test, and the twosided competitor

set(E3 "iht_twosided")
add_executable(${E3} iht/compete.cc)

foreach(X IN LISTS E3)
    ### Set up additional compiler and linker flags
    target_link_libraries(${X} PRIVATE fmt::fmt)
    target_link_libraries(${X} PRIVATE spdlog::spdlog)
    target_include_directories(${X} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../vendor/spdlog-1.12.0>)
    # NB: -D flag for ROME_LOG_LEVEL
    target_compile_definitions(${X} PUBLIC ROME_LOG_LEVEL=${LOG_LEVEL})
    # Use this to turn on warnings during compilation
    # target_compile_options(${X} PUBLIC -Wall -Wextra -Werror)
    # NB: `protos` is a library we make by recursing into the `protos` subfolder
    target_link_libraries(${X} PUBLIC protos rdma::ibverbs rdma::cm)
endforeach()
