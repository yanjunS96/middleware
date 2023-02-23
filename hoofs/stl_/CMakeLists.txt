cmake_minimum_required(VERSION 3.19)
project(${STLPRO_NAME})

# this must be recurse since every platform has potentially different cpp files
file(GLOB HPP_FILES
        ./adaptor/*.hpp
        ./algorithm/*.hpp
        ./allocator/*.hpp
        ./container/*.hpp
        ./functor/*.hpp
        ./iterator/*.hpp
        )
#add_library(${PROJECT_NAME} SHARED ${ICEORYX_PLATFORM_FILES})
add_library(${PROJECT_NAME} STATIC ${HPP_FILES})

#set_target_properties(${PROJECT_NAME} PROPERTIES
#        CXX_STANDARD_REQUIRED ON
#        CXX_STANDARD ${CMAKE_CXX_STANDARD}
#        POSITION_INDEPENDENT_CODE ON
#        )
#
#target_link_libraries(${PROJECT_NAME} PRIVATE ${ICEORYX_SANITIZER_FLAGS})
#
#target_compile_options(${PROJECT_NAME} PRIVATE ${ICEORYX_WARNINGS} ${ICEORYX_SANITIZER_FLAGS})

#target_link_libraries(${PROJECT_NAME}
#        PUBLIC
#        rt
#        pthread
#        )
