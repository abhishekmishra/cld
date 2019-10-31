FROM ubuntu:18.04

LABEL author="Abhishek Mishra <abhishekmishra3@gmail.com>"

RUN \
    apt-get -y update && \
    apt-get -y upgrade && \
    apt-get -y install build-essential gcc g++ cmake wget git

RUN \
    apt-get -y install curl unzip tar

# Set environment variables.
ENV HOME /root

# Define working directory.
WORKDIR /root

RUN \
    git clone https://github.com/Microsoft/vcpkg.git && \
    cd vcpkg && \
    ./bootstrap-vcpkg.sh && \
    ./vcpkg integrate install

WORKDIR /root/vcpkg

RUN \
    ./vcpkg install json-c libarchive curl cmocka

WORKDIR /root

RUN \
    apt-get -y install make

RUN \
    git clone https://github.com/abhishekmishra/coll.git && \
    cd coll && \
    ./build_linux.sh && \
    cd build && \
    make clean all

WORKDIR /root/coll/build

RUN ./coll_test

WORKDIR /root

RUN gcc --version
RUN ./vcpkg/vcpkg list

RUN \
    git clone https://github.com/abhishekmishra/clibdocker.git && \
    cd clibdocker && \
    ./build_linux.sh && \
    cd build && \
    make clean all
