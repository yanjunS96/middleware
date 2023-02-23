cmake_minimum_required(VERSION 3.19)
project(study_project)

#message(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
        ${HOOFS_INCDIR}/include/platform
        ${HOOFS_INCDIR}/stl_/adaptor
        ${HOOFS_INCDIR}/stl_/algorithm
        ${HOOFS_INCDIR}/stl_/allocator
        ${HOOFS_INCDIR}/stl_/container
        ${HOOFS_INCDIR}/stl_/functor
        ${HOOFS_INCDIR}/stl_/iterator
)

file(GLOB SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        )

target_link_libraries(
        ${PLATFORMPRO_NAME}
#        ${STLPRO_NAME}
)

add_executable(${PROJECT_NAME}
        ${SOURCE_FILES}
        )

install(TARGETS
        ${PROJECT_NAME}
        DESTINATION ${INSTALL_DIRECTORY}
)
#[[install(FILES
        ./include
        DESTINATION ${CMAKE_INSTALL_PREFIX}
)]]

