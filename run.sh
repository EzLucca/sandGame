#!/bin/bash

IMAGE="opengl-app"

if ! docker image inspect "$IMAGE" >/dev/null 2>&1; then
  echo "Docker image '$IMAGE' not found. Building..."
  docker build --target runtime -t "$IMAGE" .
fi

xhost +local:docker

docker run --rm -it \
  --device=/dev/dri \
  -e DISPLAY="$DISPLAY" \
  -v /tmp/.X11-unix:/tmp/.X11-unix \
  "$IMAGE" \

xhost -local:docker
