cmake_minimum_required(VERSION 3.19)
project(hoofs)

set(CMAKE_CXX_STANDARD 17)

set(PLATFORMPRO_NAME reload_platform)

set(SOURCE_FILES
        ./source/logger/mlog_impl.cpp
        ./source/file_read/file_reader.cpp
        )
include_directories(
        ./include
        ./thirdparty/spdlog/include
#        ./thirdparty/yaml-cpp/include
)

add_compile_options(-fPIC)

#link_directories(
#        thirdparty/lib
#)
add_library(${PROJECT_NAME} ${SOURCE_FILES})
#add_library(logger STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}
        spdlog
#        yaml-cpp
#        ${PLATFORMPRO_NAME}
        )
add_executable(test_log
        ./source/logger/test/test_log.cpp
        ${SOURCE_FILES}
        )
target_link_libraries(test_log
        spdlog
#        yaml-cpp
        pthread
        )
#add_definitions(-DBUILD_SHARED_LIBS=OFF)
add_subdirectory(./thirdparty/spdlog)
#add_subdirectory(./thirdparty/yaml-cpp)
#add_subdirectory(./include/platform)
#install(DIRECTORY ./stl_
#        DESTINATION ${CMAKE_INSTALL_PREFIX}/hoofs
#        FILES_MATCHING PATTERN "*.hpp" )
#install(DIRECTORY ./include/logger
#        DESTINATION ${CMAKE_INSTALL_PREFIX}/hoofs
#        FILES_MATCHING PATTERN "*.h"
#        )

#set(INSTALL_LOGGER_FILE
#        ./include/logger/*.h
#        )
install(TARGETS ${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/hoofs/lib
        )
file ( GLOB_RECURSE INSTALL_LOGGER_FILE
        ./include/logger/*.h
        )
install(FILES ${INSTALL_LOGGER_FILE}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/hoofs/include/logger
        )
install(DIRECTORY ./include/cpptoml
        DESTINATION ${CMAKE_INSTALL_PREFIX}/hoofs/include
        FILES_MATCHING PATTERN "*.h"
        )
