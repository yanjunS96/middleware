cmake_minimum_required(VERSION 3.19)
project(hoofs)

set(CMAKE_CXX_STANDARD 17)

set(PLATFORMPRO_NAME reload_platform)

#[[set(SOURCE_FILES
        ./source/logger/mlog_impl.cpp
        ./source/file_read/file_reader.cpp
        ./source/cxx_stl/*.cpp
        )]]
file ( GLOB_RECURSE SOURCE_FILES
        ./source/logger/mlog_impl.cpp
        ./source/file_read/file_reader.cpp
        ./source/cxx_stl/*.cpp
        )
include_directories(
        ./include
        ./thirdparty/spdlog/include
#        ./thirdparty/yaml-cpp/include
)

add_compile_options(-fPIC)
add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}
        spdlog
        )
add_executable(test_log
        ./source/logger/test/test_log.cpp
        ${SOURCE_FILES}
        )
target_link_libraries(test_log
        spdlog
        pthread
        )

add_subdirectory(./thirdparty/spdlog)
#add_subdirectory(./include/platform)

################################ install object ######################################
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
