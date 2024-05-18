# Rabia Sandbox: What

This is a space for my work on the c++ version of the Weak-MVC consensus algorithim from Rabia. This version is written with C++ and includes RDMA connections between nodes which is an upgrade versus the previous Golang code which uses TCP connections. We hope that this project will provide a C++ implementation of this algorithim as a tool, but also as an example of the uses of RDMA to speed up an existing algorithim.

## Observations

### protobuf

* We used protobuf to serialize the messages sent across the network. This is a reminant of the Go implementation. This also worked well with the RDMA addition, as the RDMA tool I used was already set up to send Protobuf created objects. 
* Protobuf configuration files can be compiled with the protobuf compiler, which can be done by Cmake

### CMake

* I'd never used Cmake before, but it definitely makes sense for a project of this size. Cmake is like make, but one step up. 
* start with a file called CMakeLists.txt. specify a cmake version, find libraries, and add an executable that you would like to create. this compile with `cmake ./build ` whatever path you specify will be filled with a bunch of garbage files, so make sure you specify a folder with nothing important in it. (you only have to make that mistake once). among the garbage will be a makefile, which you can use to compile your code.
* I have yet to fully figure out how to link other libraries with cmake, you can use the find_package() command, but this only works if the package is in the Cmake path variable (which you can edit in the cmake lists.) I ended up using the path variable to link a few more libraries, but some didnt work becuase a "libraryname_jsonConfig.cmake" file was not found. in this case, you need to compile the library and then this "libraryname_jsonConfig.cmake" will be made. this fixed some of my problems, but some libraries still wouldnt link. At this point I gave up and got some functioning code from another student (thanks Ethan) that properly links the libraries and header files I needed by moving them to a place where cmake can see them instead of cloning the whole library like I was doing (woah).
* I got most of the packages i used through apt-get by running apt-get install. I'll include these in a seperate .sh file, but for now, most of them are in install.sh and sync.py (which is for cloudlab).
* i had to clone and compile nlohmann_json even though i dont know what it is because my code wouldnt compile without it. there is probably a better way to do this, but im a noob so i dont know.

### Remus / Rome? 

* Remus (previously rome) is a set of tools developed by the SSS lab I am a part of. It handles Rdma connections through a connection manager object that Ethan showed me how to use. Oficially, we use a couple header files from the library from back when it was Rome. these are in a folder /rdma/ 
* Connection manager is an important part of our project, as it holds and stores multiple rdma connections at once. this is a critical part of the algorithim's sturcture as it has to send a proposal to every node (include weak mvc picture here at a later date.)

### cloudlab 

* cloudlab is a cloud systems enviroment which the lab has access to. If you are also using cloudlab, you can use the sync.py script to set up the nodes needed for the experiment with the code and dependancies. after doing this, the cloudlab terminal can be used to compile the code, and run on each node. (script to do this coming soon.) Seeing the message I sent with rdma appear on the other node's terminals was a great feeling

## How

* install on clouldlab with sync.py
* build makefiles with cmake ./build
* make executables with make
* (currently) run code on two nodes. one with --server, one with --client
* see a message sent by RDMA yay

## Contributers

* Mark Latvakoski CSE '24
* Alex Clevenger CSE '24