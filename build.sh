#!/bin/sh

set -ex
SRC_PATH=$(pwd)
BUILD_PATH=${SRC_PATH}/build
INSTALL_PATH=${SRC_PATH}/install

cmake_hoofs()
{
  cmake -B${BUILD_PATH} -H${SRC_PATH} -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH} -DBUILD_SHARED_LIBS=OFF
  cd ${BUILD_PATH} && make -j 8
  make install
}

cmake_posh()
{
  cd ${SRC_PATH}/posh
  cmake -B./build -H./ -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=./install -DBUILD_SHARED_LIBS=ON
  cd ${BUILD_PATH} && make -j 8
#  make install
  cd ${SRC_PATH}
}

main()
{
  cmake_hoofs
#  cmake_posh
}

main @
echo "==== Build Success ===="





