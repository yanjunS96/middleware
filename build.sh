#!/bin/sh

set -ex
SRC_PATH=$(pwd)
BUILD_PATH=${SRC_PATH}/build
INSTALL_PATH=${SRC_PATH}/install

cmake_param()
{
	cmake -B${BUILD_PATH} -H${SRC_PATH} -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH} -DBUILD_SHARED_LIBS=ON
}

cmake_build()
{
  cd ${BUILD_PATH} && make -j 8
  make install
}

main()
{
  cmake_param
  cmake_build
}

main @
echo "==== Build Success ===="





