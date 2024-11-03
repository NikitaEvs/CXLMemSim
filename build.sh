#!/bin/bash

LOCAL_PATH=`pwd`
LIBBPF_PATH=$LOCAL_PATH/libbpf
echo "Using LIBBPF installation path = $LIBBPF_PATH"
CORES=8

echo "Build using ${CORES} cores"

mkdir -p build
cd build
cmake -DLIBBPF_PATH=${LIBBPF_PATH} ..
make -j${CORES} all
