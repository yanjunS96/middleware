#!/bin/sh

set -ex
SRC_PATH=$(pwd)
BUILD_PATH=${SRC_PATH}/build
INSTALL_PATH=${SRC_PATH}/install

clean_all()
{
	rm -rf ${BUILD_PATH}
	rm -rf ${INSTALL_PATH}
}

clean_all

