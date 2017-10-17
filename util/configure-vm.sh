#!/bin/bash

# Check for CUDA 8 and try to install
if ! dpkg-query -W cuda-8-0; then
	# Get Cuda 8.0 for Ubuntu 16.0.4
	curl -O http://developer.download.nvidia.com/compute/cuda/repos/ubuntu1604/x86_64/cuda-repo-ubuntu1604_8.0.61-1_amd64.deb
	dpkg -i ./cuda-repo-ubuntu1604_8.0.61-1_amd64.deb
	apt-get update
	apt-get install cuda-8-0 -y
fi
# Let bashrc add CUDA to path on startup
echo 'export PATH=/usr/local/cuda-8.0/bin${PATH:+:${PATH}}' >> ~/.bashrc

# Install Make
sudo apt-get install build-essential

# Install FFMPEG
sudo add-apt-repository ppa:mc3man/trusty-media  
sudo apt-get update  
sudo apt-get install ffmpeg  
sudo apt-get install frei0r-plugins

# Install ImageMagick
sudo apt-get install imagemagick

# Install Numpy Stack
sudo apt-get install python-numpy python-scipy python-matplotlib

# Install Lua
sudo apt-get install lua5.3 liblua5.3-dev

# Install support for perf
sudo apt-get install linux-tools-generic linux-cloud-tools-generic
sudo apt-get install linux-tools-4.10.0-37-generic \
                     linux-cloud-tools-4.10.0-37-generic

# Clean Up
rm -f cuda-repo-ubuntu1604_8.0.61-1_amd64.deb
