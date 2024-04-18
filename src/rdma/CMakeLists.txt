cmake_minimum_required(VERSION 3.18)

project(rrdma)

#find_package(remus REQUIRED)

add_executable(server node1/node1.cc)
add_executable(client node2/node2.cc)