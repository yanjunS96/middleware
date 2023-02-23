cmake_minimum_required(VERSION 3.19)

#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)

set(INSTALL_DIRECTORY ${CMAKE_INSTALL_PREFIX})
set(PROCESS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(STL_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/stl_)

message(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(hoofs)
add_subdirectory(posh)
