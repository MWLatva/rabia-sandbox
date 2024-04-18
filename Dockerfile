# Dockerfile to build a container
# yoinked from 303
FROM ubuntu
#MAINTAINER Michael Spear (spear@lehigh.edu) <- Thanks!

# Initialize repos and upgrade the base system
RUN apt-get update -y
RUN apt-get upgrade -y

# Install additional software needed for development
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y git man curl build-essential screen gdb libssl-dev psmisc python3

#added from remus's dockerfile
RUN apt-get install protobuf-compiler  -y
RUN apt-get install cmake -y
RUN apt-get install ibverbs-utils librdmacm-dev libfmt-dev libspdlog-dev -y
RUN apt-get install doxygen -y

# Change the working directory:
WORKDIR "/root"
