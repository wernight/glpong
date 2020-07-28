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

RUN set -x \
 && mkdir /opt/regal \
 && cd $_ \
 && curl -L https://github.com/emscripten-ports/regal/archive/version_7.tar.gz | tar xz --strip-components=1 \
 && make -f Makefile.regal \
 && make -f Makefile.glu

WORKDIR /code
# Port running a basic web server for testing.
EXPOSE 8080
CMD ["/code/make.sh"]
