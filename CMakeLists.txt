#################################################################################
#                                nanocppjson                                    #
#################################################################################

cmake_minimum_required(VERSION 3.18)

project(safelog DESCRIPTION "A C++ json library for embedded systems"
                VERSION 1.0.0
)

# Definitions for Version.h file
add_definitions(-DNANOCPPJSON_MAJOR="${PROJECT_VERSION_MAJOR}")
add_definitions(-DNANOCPPJSON_MINOR="${PROJECT_VERSION_MINOR}")
add_definitions(-DNANOCPPJSON_FIX="${PROJECT_VERSION_PATCH}")

# Build options
include(CMakeLists_Options.txt)

# C/C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Set compiler flags
if(${ENABLE_GCOV_COVERAGE})
    set(COVERAGE_FLAGS      "--coverage")
endif()

# Warnings
set(WARNING_FLAGS "-Wall -Wextra -Wconversion -Werror")

set(COMMON_FLAGS            "${COVERAGE_FLAGS} -finline-functions")
set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG_INIT} ${WARNING_FLAGS} ${COMMON_FLAGS} -Og -g3 -ggdb3 -DDEBUG -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG_INIT} ${WARNING_FLAGS} ${COMMON_FLAGS} -Og -g3 -ggdb3 -DDEBUG -fno-omit-frame-pointer")
set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE_INIT} ${WARNING_FLAGS} ${COMMON_FLAGS} -O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE_INIT} ${WARNING_FLAGS} ${COMMON_FLAGS} -O2 -DNDEBUG")


# Default output directories
if(NOT DEFINED BIN_DIR)
    set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin)
    message("BIN_DIR not defined, using default: ${BIN_DIR}")
endif()
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

# 3rd party libraries
add_subdirectory(3rdparty)

# YaCppRtos sources
add_subdirectory(src)

# Unit tests
if(${BUILD_UNIT_TESTS})
    enable_testing()
    add_subdirectory(tests)
endif()
