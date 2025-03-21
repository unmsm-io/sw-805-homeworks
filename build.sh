#!/bin/bash

# Check if an argument was provided
if [ -z "$1" ]; then
    echo "❌ You must provide a C++ file to compile."
    echo "Usage: ./build.sh src/main.cpp [--openmp] [--pthreads] [--mpi] [--verbose|-v]"
    exit 1
fi

# Detect flags
ENABLE_OPENMP=OFF
ENABLE_PTHREADS=OFF
ENABLE_MPI=OFF
VERBOSE=false

for arg in "$@"; do
    if [[ "$arg" == "--openmp" ]]; then
        ENABLE_OPENMP=ON
    elif [[ "$arg" == "--pthreads" ]]; then
        ENABLE_PTHREADS=ON
    elif [[ "$arg" == "--mpi" ]]; then
        ENABLE_MPI=ON
    elif [[ "$arg" == "--verbose" || "$arg" == "-v" ]]; then
        VERBOSE=true
    fi
done

# Ensure necessary directories exist
mkdir -p build out

# Move to build directory
cd build

# Convert relative path to absolute path
ABS_SOURCE_FILE=$(realpath ../"$1")

# Debugging: print the resolved source file path
echo "🔍 Using source file: $ABS_SOURCE_FILE"

# Run CMake with or without verbosity
if [ "$VERBOSE" = true ]; then
    cmake -DSOURCE_FILE="$ABS_SOURCE_FILE" -DENABLE_OPENMP=$ENABLE_OPENMP -DENABLE_PTHREADS=$ENABLE_PTHREADS -DENABLE_MPI=$ENABLE_MPI ..
    cmake --build .
else
    cmake -DSOURCE_FILE="$ABS_SOURCE_FILE" -DENABLE_OPENMP=$ENABLE_OPENMP -DENABLE_PTHREADS=$ENABLE_PTHREADS -DENABLE_MPI=$ENABLE_MPI .. > /dev/null 2>&1
    cmake --build . > /dev/null 2>&1
fi

# Extract executable name
EXECUTABLE_NAME=$(basename "$1" .cpp)

# Handle Windows .exe extension
if [[ "$(uname -s)" == "MINGW64_NT"* || "$(uname -s)" == "MSYS_NT"* ]]; then
    EXECUTABLE_NAME="$EXECUTABLE_NAME.exe"
fi

# Move executable to out/ directory
if [ -f "out/$EXECUTABLE_NAME" ]; then
    mv "out/$EXECUTABLE_NAME" "../out/"
else
    echo "❌ Error: Could not move the executable."
    exit 1
fi

cd ..

# Execute the compiled program
"./out/$EXECUTABLE_NAME"
