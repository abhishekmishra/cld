#!/bin/bash

cd /

SW_HOME="/sw"
export VCPKG_HOME="${SW_HOME}/vcpkg"
CODE_HOME="/code"

# TODO git status check is broken
BUILD_COLL=1
BUILD_CLIBDOCKER=1
BUILD_CLD=1

# vcpkg installation
if [ -d "${VCPKG_HOME}" ]; then
    # looks like vcpkg is installed.
    echo "VCPKG is installed."
else
    cd ${SW_HOME}
    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    ./bootstrap-vcpkg.sh -disableMetrics
    ./vcpkg integrate install
    cd /
fi

# vcpkg packages
${VCPKG_HOME}/vcpkg install curl
${VCPKG_HOME}/vcpkg install libarchive
${VCPKG_HOME}/vcpkg install cmocka
${VCPKG_HOME}/vcpkg install json-c
${VCPKG_HOME}/vcpkg install lua

echo "VCPKG packages installed."

if [ -d "$CODE_HOME/coll" ]; then
    echo "coll is already cloned."
    cd "$CODE_HOME/coll"
    git pull
    cd -
else
    cd "$CODE_HOME"
    git clone https://github.com/abhishekmishra/coll.git
    BUILD_COLL=1
    cd -
fi

if [ -d "$CODE_HOME/coll" ] && [ "$BUILD_COLL" == "1" ]; then
    echo "Start coll build"
    cd "$CODE_HOME/coll"
    rm -fR ./build
    ./build_linux.sh
    ./build/coll_test
    cd -
fi

if [ -d "$CODE_HOME/clibdocker" ]; then
    echo "clibdocker is already cloned."
    cd "$CODE_HOME/clibdocker"
    git pull
    cd -
else
    cd "$CODE_HOME"
    git clone https://github.com/abhishekmishra/clibdocker.git
    BUILD_CLIBDOCKER=1
    cd -
fi

if [ -d "$CODE_HOME/clibdocker" ] && [ "$BUILD_CLIBDOCKER" == "1" ]; then
    echo "Start clibdocker build"
    cd "$CODE_HOME/clibdocker"
    rm -fR ./build
    ./build_linux.sh
    cd -
fi

if [ -d "$CODE_HOME/cld" ]; then
    echo "cld is already cloned."
    cd "$CODE_HOME/cld"
    git pull
    cd -
else
    cd "$CODE_HOME"
    git clone https://github.com/abhishekmishra/cld.git
    cd -
fi

if [ -d "$CODE_HOME/cld" ] && [ "$BUILD_CLD" == "1" ]; then
    echo "Start cld build"
    cd "$CODE_HOME/cld"
    rm -fR ./build
    ./build_linux.sh
    cd -
fi

VALGRIND_OUT=$CODE_HOME/valgrind-out.txt

echo "CLD Docker Test Host is " $CLD_TEST_HOST
echo "CLD Test Command is " $CLD_TEST_COMMAND
echo "Valgrind output file is at " $VALGRIND_OUT

$CODE_HOME/cld/build/cld -H $CLD_TEST_HOST $CLD_TEST_COMMAND

valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=${VALGRIND_OUT} \
         "$CODE_HOME/cld/build/cld" -H $CLD_TEST_HOST $CLD_TEST_COMMAND