#!/bin/bash

docker run -it --rm \
    -v=${HOME}/volumes/cld_debian_build/sw/:/sw \
    -v=${HOME}/volumes/cld_debian_build/code/:/code \
    am/cld_debian_build:latest