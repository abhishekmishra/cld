#!/bin/bash

SW_HOME="/sw"
VCPKG_HOME="${SW_HOME}/vcpkg"

# vcpkg installation
if [ -d "${VCPKG_HOME}" ]; then
    # looks like vcpkg is installed.
    echo "VCPKG is installed."
else
    cd ${SW_HOME}
    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    ./bootstrap-vcpkg.sh
    ./vcpkg integrate install
    cd /
fi

# vcpkg packages
${VCPKG_HOME}/vcpkg install curl
${VCPKG_HOME}/vcpkg install libarchive
${VCPKG_HOME}/vcpkg install cmocka
${VCPKG_HOME}/vcpkg install json-c