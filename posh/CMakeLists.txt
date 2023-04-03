cmake_minimum_required(VERSION 3.19)

project(posh_test)

link_directories(
        ./thirdparty/lib
)

include_directories(
        ./thirdparty/include
        ./thirdparty/include/cpptoml
        ./thirdparty/include/spdlog
        ./thirdparty/include/yaml-cpp/include/
)


include_directories(
        ./source
        ./source/doipserver
        ./source/yaml_parse
        ./source/logger
)

file(GLOB SOURCE_FILES
        ./main.cpp
        ./source/doipserver/*.cpp
        ./source/doipserver/server/*.cpp
        ./source/logger/*.cpp
        )

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
        pthread
        spdlogd
        yaml-cppd
        )

#[[install(TARGETS ${PROJECT_NAME}
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/install
        )]]

#add_subdirectory(./test)