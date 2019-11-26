#!/bin/bash

docker run -it --rm \
    -v=${HOME}/volumes/cld_alpine_build/sw/:/sw \
    -v=${HOME}/volumes/cld_alpine_build/code/:/code \
    am/cld_alpine_build:latest