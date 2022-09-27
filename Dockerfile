# https://hub.docker.com/_/debian
FROM debian:13

# Package dependencies.
RUN set -x \
 && apt update && apt install -y \
        build-essential \
        cmake \
        curl \
        libglu1-mesa-dev \
        libsdl2-dev \
        libsdl2-image-dev \
 && apt clean \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /code
CMD ["/code/make.sh"]
