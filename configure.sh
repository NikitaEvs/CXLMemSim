#!/bin/bash

apt-get download libbpf-dev
dpkg -x libbpf-dev_* libbpf
rm -rf libbpf-dev_*
echo "LIBBPF INSTALLATION PATH = `pwd`/libbpf"
