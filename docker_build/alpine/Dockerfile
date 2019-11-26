FROM alpine:latest

LABEL author="Abhishek Mishra <abhishekmishra3@gmail.com>"

RUN apk update && \
    apk add --update \
    gcc \
    make \
    cmake \
    valgrind \
    git \
    wget

RUN apk add --update \
    curl \
    bash

# two volumes one for any required software,
# another for the code to be built
VOLUME [ "/sw", "/code" ]

COPY build.sh /root/build.sh

ENTRYPOINT /root/build.sh