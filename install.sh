#!/usr/bin/env bash

set -e
set -x

sudo apt update
# Installing build dependencies
sudo apt install cmake build-essential -y
# Installing dependencies
sudo apt install libfuse3-dev -y
# Configuring
cmake .
# Building
make
# Binary install
sudo make install

exit 0

