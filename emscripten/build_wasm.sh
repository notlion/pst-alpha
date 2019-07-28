#!/bin/bash

pushd $(dirname "$0")

BUILD_DIR="build"

mkdir -p ${BUILD_DIR}
pushd ${BUILD_DIR}

cmake ../.. -DCMAKE_TOOLCHAIN_FILE="${EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" $@
make -j4

popd

popd
