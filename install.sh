#!/usr/bin/env bash

set -e
set -x

apt update
apt install sudo -y

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

