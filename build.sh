#!/bin/bash

# Check if arguments were provided
if [ -z "$1" ]; then
    echo "❌ Debes proporcionar un archivo C++ para compilar."
    echo "Uso: ./build.sh src/main.cpp [--verbose|-v]"
    exit 1
fi

# Check for verbose flag
VERBOSE=false
for arg in "$@"; do
    if [[ "$arg" == "--verbose" || "$arg" == "-v" ]]; then
        VERBOSE=true
    fi
done

# Ensure the necessary directories exist
mkdir -p build out

# Move to build directory
cd build

# Run CMake with or without verbosity
if [ "$VERBOSE" = true ]; then
    cmake -DSOURCE_FILE="../$1" .. 
    cmake --build . 
else
    cmake -DSOURCE_FILE="../$1" .. > /dev/null 2>&1
    cmake --build . > /dev/null 2>&1
fi

# Extract the executable name
EXECUTABLE_NAME=$(basename "$1" .cpp)

# Move the binary to out/ directory
mv "out/$EXECUTABLE_NAME" ../out/ 2>/dev/null || { echo "❌ Error: No se pudo mover el ejecutable."; exit 1; }

cd ..

# Execute the compiled file and only print its output
./out/$EXECUTABLE_NAME
