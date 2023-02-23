# Copyright (c) 2019 - 2021 by Robert Bosch GmbH. All rights reserved.
# Copyright (c) 2020 - 2022 by Apex.AI Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.16)
project(${PLATFORMPRO_NAME})

option(LINUX ON)

if(LINUX)
    set(ICEORYX_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR}/linux/)
elseif(QNX)
    set(ICEORYX_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR}/qnx/)
elseif(APPLE)
    set(ICEORYX_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR}/mac/)
elseif(WIN32)
    set(ICEORYX_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR}/win/)
elseif(UNIX)
    set(ICEORYX_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR}/unix/)
else()
    set(ICEORYX_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR}/unix/)
    message(WARNING "Could not detect supported platform, but I'm feeling lucky today. Maybe its Unix." )
endif()

set(ICEORYX_PLATFORM ${ICEORYX_PLATFORM} CACHE PATH "" FORCE)

# this must be recurse since every platform has potentially different cpp files
include_directories(
        ${ICEORYX_PLATFORM}
)
file ( GLOB_RECURSE ICEORYX_PLATFORM_FILES
        ${ICEORYX_PLATFORM}/*.cpp
        )
#add_library(${PROJECT_NAME} SHARED ${ICEORYX_PLATFORM_FILES})
add_library(${PROJECT_NAME} STATIC ${ICEORYX_PLATFORM_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_STANDARD ${CMAKE_CXX_STANDARD}
        POSITION_INDEPENDENT_CODE ON
        )

target_link_libraries(${PROJECT_NAME} PRIVATE ${ICEORYX_SANITIZER_FLAGS})

target_compile_options(${PROJECT_NAME} PRIVATE ${ICEORYX_WARNINGS} ${ICEORYX_SANITIZER_FLAGS})

if(LINUX)
    target_link_libraries(${PROJECT_NAME}
            PUBLIC
            rt
            pthread
            )
elseif(QNX)
    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            socket
            )
elseif(APPLE)
elseif(WIN32)
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME}
            PUBLIC
            rt
            pthread
            )
endif()

file ( GLOB_RECURSE INSTALL_PLATFORM_FILES
        ${ICEORYX_PLATFORM}/*.hpp
        )
set(PLATFORM_INSTALL ${CMAKE_INSTALL_PREFIX}/hoofs/include/platform)
install(FILES ${INSTALL_PLATFORM_FILES}
        DESTINATION ${PLATFORM_INSTALL}
        )
