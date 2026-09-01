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
  bash -c 'cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && \
  cmake --build build -j$(nproc) && \
  cd build && ./OpenGLProject'

xhost -local:docker
