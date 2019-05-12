FROM debian:stretch

# Package dependencies.
RUN set -x \
 && apt update && apt install -y \
        build-essential \
        cmake \
        curl \
        default-jre \
        libglu1-mesa-dev \
        libsdl1.2-dev \
        libsdl-image1.2-dev \
        nodejs \
        python \
 && apt clean \
 && rm -rf /var/lib/apt/lists/*

# Emscripten is to build WebAssembly from C++.
# https://github.com/emscripten-core/emscripten
RUN set -x \
 && mkdir /opt/emscripten \
 && cd /opt/emscripten \
 && curl -L https://github.com/emscripten-core/emsdk/tarball/master | tar xz --strip-components=1 \
 && ./emsdk install latest \
 && ./emsdk activate latest

# glm
# https://glm.g-truc.net/0.9.9/index.html
RUN set -x \
 && mkdir /opt/glm \
 && cd /opt/glm \
 && curl -L https://github.com/g-truc/glm/archive/0.9.9.5.tar.gz | tar xz --strip-components=1 \
 && mkdir build \
 && cd build \
 && cmake .. \
 && make install

WORKDIR /code
# Port running a basic web server for testing.
EXPOSE 8080
CMD ["/code/make.sh"]
