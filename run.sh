#!/bin/bash

docker run --rm -it \
  --device=/dev/dri \
  -e DISPLAY="$DISPLAY" \
  -v /tmp/.X11-unix:/tmp/.X11-unix \
  -v "$PWD:/workspace" \
  opengl-dev \
  bash -c 'cmake -S . -B build && cmake --build build -j$(nproc) && cd build && ./OpenGLProject'

