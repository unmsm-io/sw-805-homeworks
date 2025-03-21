cmake_minimum_required(VERSION 3.10)
project(DynamicBuild)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

# Define a variable for dynamic compilation
set(SOURCE_FILE "" CACHE STRING "Source file to compile")
set(ENABLE_OPENMP OFF CACHE BOOL "Enable OpenMP support")
set(ENABLE_PTHREADS OFF CACHE BOOL "Enable Pthreads support")
set(ENABLE_MPI OFF CACHE BOOL "Enable MPI support")

# Create an executable only if a source file is provided
if (SOURCE_FILE)
    get_filename_component(EXECUTABLE_NAME ${SOURCE_FILE} NAME_WE)
    add_executable(${EXECUTABLE_NAME} ${SOURCE_FILE})

    # Enable OpenMP
    if (ENABLE_OPENMP)
        find_package(OpenMP REQUIRED)
        target_link_libraries(${EXECUTABLE_NAME} OpenMP::OpenMP_CXX)
        message(STATUS "✅ OpenMP Enabled")
    endif()

    # Enable Pthreads
    if (ENABLE_PTHREADS)
        target_link_libraries(${EXECUTABLE_NAME} pthread)
        message(STATUS "✅ Pthreads Enabled")
    endif()

    # Enable MPI
    if (ENABLE_MPI)
        find_package(MPI REQUIRED)
        target_include_directories(${EXECUTABLE_NAME} PRIVATE ${MPI_INCLUDE_PATH})
        target_link_libraries(${EXECUTABLE_NAME} ${MPI_CXX_LIBRARIES})
        message(STATUS "✅ MPI Enabled")
    endif()
endif()