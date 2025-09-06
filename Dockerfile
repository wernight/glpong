# https://hub.docker.com/_/debian
FROM debian:13

# Package dependencies.
RUN set -x \
 && apt update && apt install -y \
        build-essential \
        cmake \
        curl \
        libglew-dev \
        libglm-dev \
        libglu1-mesa-dev \
        libsdl2-dev \
        libstb-dev \
 && apt clean \
 && rm -rf /var/lib/apt/lists/*

# https://docs.appimage.org/packaging-guide/from-source/linuxdeploy-user-guide.html
# https://github.com/AppImage/AppImageKit
ARG LINUXDEPLOY_APPIMAGE_URL=https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
RUN set -eux; \
    curl -Lo /tmp/linuxdeploy-x86_64.AppImage ${LINUXDEPLOY_APPIMAGE_URL}; \
    chmod +x /tmp/linuxdeploy-x86_64.AppImage; \
    mkdir /opt/linuxdeploy; \
    cd /opt/linuxdeploy; \
    /tmp/linuxdeploy-x86_64.AppImage --appimage-extract; \
    ln -s /opt/linuxdeploy/squashfs-root/AppRun /usr/local/bin/linuxdeploy; \
    apt-get update; \
    apt-get install -y \
        file \
        zsync; \
    apt-get clean; \
    rm -rf /var/lib/apt/lists/*

# Emscripten is to build WebAssembly from C++.
# https://github.com/emscripten-core/emscripten/tags
ARG EMSCRIPTEN_VERSION=4.0.14
WORKDIR /opt/emscripten
RUN curl -L https://github.com/emscripten-core/emsdk/archive/refs/tags/$EMSCRIPTEN_VERSION.tar.gz | tar xz --strip-components=1
RUN ./emsdk install $EMSCRIPTEN_VERSION
RUN ./emsdk activate $EMSCRIPTEN_VERSION

# Port running a basec web server for testing.
EXPOSE 8080
WORKDIR /code
CMD ["/code/make.sh"]
