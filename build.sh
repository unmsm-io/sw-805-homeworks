#!/bin/bash

# Check if an argument was provided
if [ -z "$1" ]; then
    echo "❌ Debes proporcionar un archivo C++ para compilar."
    echo "Uso: ./build.sh src/main.cpp [--verbose|-v]"
    exit 1
fi

# Detect verbose flag
VERBOSE=false
for arg in "$@"; do
    if [[ "$arg" == "--verbose" || "$arg" == "-v" ]]; then
        VERBOSE=true
    fi
done

# Ensure necessary directories exist
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
    echo "❌ Error: No se pudo mover el ejecutable."
    exit 1
fi

cd ..

# Execute the compiled program
"./out/$EXECUTABLE_NAME"
