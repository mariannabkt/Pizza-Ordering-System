#!/bin/bash

# Check if the C file exists
if [ ! -f "p3190128-pizza.c" ]; then
    echo "Error: p3190128-pizza.c not found!"
    exit 1
fi

# Compile the C file
gcc -o p3190128-pizza.out p3190128-pizza.c -pthread

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Executing..."
    ./p3190128-pizza.out 100 10
else
    echo "Compilation failed!"
fi
