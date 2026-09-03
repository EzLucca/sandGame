# #!/bin/bash
#
# IMAGE="opengl-dev"
#
# if ! docker image inspect "$IMAGE" >/dev/null 2>&1; then
#   echo "Building development image..."
#   docker build --target builder -t "$IMAGE" .
# fi
#
# xhost +local:docker
#
# docker run --rm -it \
#   --device=/dev/dri \
#   -e DISPLAY="$DISPLAY" \
#   -v /tmp/.X11-unix:/tmp/.X11-unix \
#   -v "$PWD:$PWD" \
#   -w "$PWD" \
#   "$IMAGE" \
#   bash -c '
#     cmake -S . -B build \
#       -DCMAKE_BUILD_TYPE=Debug \
#       -DCMAKE_CXX_FLAGS="-pg" \
#       -DCMAKE_EXE_LINKER_FLAGS="-pg" \
#       -DCMAKE_EXPORT_COMPILE_COMMANDS=ON &&
#
#     cmake --build build -j$(nproc) &&
#
#     cd build &&
#     rm -f gmon.out &&
#
#     ./OpenGLProject &&
#
#     gprof ./OpenGLProject gmon.out > gprof.txt
#   '
#
# xhost -local:docker

#!/bin/bash

IMAGE="opengl-dev"

if ! docker image inspect "$IMAGE" >/dev/null 2>&1; then
  echo "Building development image..."
  docker build --target builder -t "$IMAGE" .
fi

xhost +local:docker

docker run --rm -it \
  --device=/dev/dri \
  -e DISPLAY="$DISPLAY" \
  -v /tmp/.X11-unix:/tmp/.X11-unix \
  -v "$PWD:$PWD" \
  -w "$PWD" \
  "$IMAGE" \
  bash -c '
    set -e

    echo "Building with gprof instrumentation..."

    cmake -S . -B build \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_CXX_FLAGS="-pg" \
      -DCMAKE_EXE_LINKER_FLAGS="-pg" \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

    cmake --build build -j$(nproc)

    cd build

    rm -f gmon.out gprof.txt gprof-flat.txt gprof-callgraph.txt

    echo "Starting OpenGLProject..."
    ./OpenGLProject

    echo "Generating gprof reports..."

    gprof ./OpenGLProject gmon.out > gprof.txt
    gprof -p ./OpenGLProject gmon.out > gprof-flat.txt
    gprof -q ./OpenGLProject gmon.out > gprof-callgraph.txt

    echo ""
    echo "Profiling complete:"
    echo "  build/gprof.txt"
    echo "  build/gprof-flat.txt"
    echo "  build/gprof-callgraph.txt"
  '

xhost -local:docker

