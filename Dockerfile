FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    pkg-config \
    libglfw3-dev \
    libglm-dev \
    libgl1-mesa-dev \
    mesa-utils \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

CMD ["bash"]

