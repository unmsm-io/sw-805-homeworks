cmake_minimum_required(VERSION 3.10)
project(DynamicBuild)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

# Define a variable for dynamic compilation
set(SOURCE_FILE "" CACHE STRING "Source file to compile")

# Create an executable only if a source file is provided
if (SOURCE_FILE)
    get_filename_component(EXECUTABLE_NAME ${SOURCE_FILE} NAME_WE)
    add_executable(${EXECUTABLE_NAME} ${SOURCE_FILE})
endif()
