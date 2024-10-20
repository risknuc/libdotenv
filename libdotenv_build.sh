#! /bin/bash

error() {
    echo "libdotenv build instruction -> You have not provided any arguments, or the argument is incorrect. Only two options exist:"
    echo "  -c || --check - build with a check to ensure everything works and also run an example"
}

if [ "$1" = "-c" ] || [ "$1" = "--check" ]; then
    echo "=== building with gcc === "
    gcc examples/examples_main.c -o examples/example_run -g -std=c11
    echo "=== checking with valgrind ==="
    valgrind --track-origins=yes --leak-check=full examples/example_run
    echo "=== checking with cppcheck ==="
    cppcheck examples/examples_main.c
    cppcheck libdotenv.h
    echo "=== run executable ==="
    examples/example_run
else
    error
fi
