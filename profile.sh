#!/bin/bash
set -e

echo "Starting OpenGLProject..."

rm -f /app/build/gmon.out
rm -f /reports/gprof.txt
rm -f /reports/gprof-flat.txt
rm -f /reports/gprof-callgraph.txt

# Run the instrumented program.
# gmon.out will be generated when the program exits.
./OpenGLProject

echo "Program finished."

if [ ! -f /app/build/gmon.out ]; then
    echo "ERROR: gmon.out was not generated."
    exit 1
fi

echo "Generating gprof reports..."

gprof ./OpenGLProject ./gmon.out > /reports/gprof.txt

# Flat profile: time spent in each function
gprof -p ./OpenGLProject ./gmon.out > /reports/gprof-flat.txt

# Call graph: who calls what
gprof -q ./OpenGLProject ./gmon.out > /reports/gprof-callgraph.txt

echo ""
echo "================================"
echo "Profiling complete."
echo "Reports:"
echo "  /reports/gprof.txt"
echo "  /reports/gprof-flat.txt"
echo "  /reports/gprof-callgraph.txt"
echo "================================"

