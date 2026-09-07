#!/bin/bash

IMAGE="opengl-dev"
CONTAINER="opengl-profiler"

if ! docker image inspect "$IMAGE" >/dev/null 2>&1; then
  echo "Building development image..."
  docker build --target builder -t "$IMAGE" .
fi

xhost +local:docker

# Clean up any old container.
docker rm -f "$CONTAINER" >/dev/null 2>&1 || true

# Split the current tmux window and remember the new pane's ID.
HTOP_PANE=$(tmux split-window -h -P -F '#{pane_id}' '
  echo "Waiting for OpenGLProject..."

  while true; do
    PID=$(docker top '"$CONTAINER"' -eo pid,comm 2>/dev/null \
      | awk '\''$2 == "OpenGLProject" { print $1; exit }'\'')

    if [ -n "$PID" ]; then
      break
    fi

    sleep 0.2
  done

  echo "Monitoring OpenGLProject (PID $PID)..."
  htop -p "$PID"
')

# Start Docker in the original pane.
docker run --rm -it \
  --name "$CONTAINER" \
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

# Docker/OpenGLProject has exited.
# Close ONLY the htop pane.
if [ -n "$HTOP_PANE" ]; then
    tmux kill-pane -t "$HTOP_PANE" 2>/dev/null || true
fi

xhost -local:docker

