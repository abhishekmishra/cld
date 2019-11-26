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
VOLUME [ "/sw", "/code" , "/root/.cmake"]

COPY build.sh /root/build.sh

# correct newlines for when building from windows
RUN apt-get install -y dos2unix
RUN dos2unix /root/build.sh /root/build.sh

ENTRYPOINT /root/build.sh
#ENTRYPOINT /code/cld/build/cld -H http://host.docker.internal:2376/ sys ver