cmake_minimum_required(VERSION 3.19)

set(HOOFS_INCDIR ${INSTALL_DIRECTORY}/hoofs)

link_directories(
        ${HOOFS_INCDIR}/lib
)

include_directories(
        ${HOOFS_INCDIR}/include
)

#add_subdirectory(./test)