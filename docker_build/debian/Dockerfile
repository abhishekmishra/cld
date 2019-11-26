FROM debian:latest

LABEL author="Abhishek Mishra <abhishekmishra3@gmail.com>"

RUN apt-get update && \
    apt-get install -y \
    gcc \
    g++ \
    make \
    cmake \
    valgrind \
    git \
    wget \
    curl \
    bash \
    unzip \
    tar

# two volumes one for any required software,
# another for the code to be built
VOLUME [ "/sw", "/code" ]

COPY build.sh /root/build.sh

ENTRYPOINT /root/build.sh