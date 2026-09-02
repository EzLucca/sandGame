# FROM ubuntu:24.04@sha256:33ceb71981b602c1a7443a53469e4dba065f7503eab3078a2d7a57a2ab987517 
#
# RUN apt-get update && apt-get install --no-install-recommends -y \
#     build-essential \
#     cmake \
#     pkg-config \
#     libglfw3-dev \
#     libglm-dev \
#     && rm -rf /var/lib/apt/lists/*
#
# WORKDIR /workspace
#
# CMD ["bash"]

# ==========================
# Builder
# ==========================
FROM ubuntu:24.04@sha256:33ceb71981b602c1a7443a53469e4dba065f7503eab3078a2d7a57a2ab987517 AS builder

RUN apt-get update && apt-get install --no-install-recommends -y \
    build-essential \
    cmake \
    pkg-config \
    libglfw3-dev \
    libglm-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src

COPY . .

RUN cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-pg" \
    -DCMAKE_EXE_LINKER_FLAGS="-pg" \
    -DCMAKE_C_COMPILER=/usr/bin/gcc \
    -DCMAKE_CXX_COMPILER=/usr/bin/g++ \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON 
RUN cmake --build build -j$(nproc)


# ==========================
# Runtime
# ==========================
FROM ubuntu:24.04@sha256:33ceb71981b602c1a7443a53469e4dba065f7503eab3078a2d7a57a2ab987517 AS runtime

RUN apt-get update && apt-get install --no-install-recommends -y \
    libglfw3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app/build

COPY --from=builder /src/build/OpenGLProject .
COPY --from=builder /src/shaders ../shaders

ENTRYPOINT ["./OpenGLProject"]

