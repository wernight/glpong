#!/bin/bash
set -ex

CODEDIR=/code

BUILDDIR=/tmp/glpong
mkdir -p /tmp/glpong
cmake \
    -B $BUILDDIR \
    -DCMAKE_BUILD_TYPE=Debug \
    -S $CODEDIR
cmake --build $BUILDDIR

# Generate the AppDir folder content.
APPDIR=/tmp/GLPong.AppDir
rm -rf ${APPDIR} || true
mkdir -p ${APPDIR}/usr/bin/
cp /code/res/*.png ${APPDIR}
cp $BUILDDIR/glpong ${APPDIR}/usr/bin/

# Generate the AppImage.
# https://docs.appimage.org/packaging-guide/from-source/linuxdeploy-user-guide.html
# https://docs.appimage.org/packaging-guide/manual.html
# The additional dependencies can only be tested by running something like:
linuxdeploy \
    --appdir=${APPDIR} \
    --executable=${BUILDDIR}/glpong \
    --desktop-file=${CODEDIR}/res/GLPong.desktop \
    --icon-file=${CODEDIR}/res/GLPong.png \
    --library=/usr/lib/x86_64-linux-gnu/libOpenGL.so.0 \
    --library=/usr/lib/x86_64-linux-gnu/libGLdispatch.so.0 \
    --output \
    appimage

mv *.AppImage /code/bin/

# Emscripten is to build WebAssembly from C++.
WASMDIR=${CODEDIR}/bin/wasm
source /opt/emscripten/emsdk_env.sh
cd /code
mkdir -p ${WASMDIR}

# We don't include the whole /usr/include because it'd mess up Emscripten imports.
# So we create a sub-set of the includes we want from the system.
INCLUDE_EXTRA_DIR=/tmp/emscripten-extra-include
mkdir -p ${INCLUDE_EXTRA_DIR}
ln -fs /usr/include/stb ${INCLUDE_EXTRA_DIR}/stb
ln -fs /usr/include/glm ${INCLUDE_EXTRA_DIR}/glm

emcc src/*.cpp \
    -I ${INCLUDE_EXTRA_DIR} \
    -s USE_SDL=2 \
    -s USE_REGAL=1 \
    -s STB_IMAGE=1 \
    -s USE_WEBGL2=1 \
    -I /opt/glm \
    -l /opt/glm/build/glm/libglm_static.a \
    -O2 \
    -o ${WASMDIR}/glpong.html \
    --preload-file ${CODEDIR}/res/particle.png@particle.png \
    --preload-file ${CODEDIR}/res/small_blur_star.png@small_blur_star.png \
    --emrun

cd ${WASMDIR}
exec emrun --no_browser --hostname 0.0.0.0 --port 8080 glpong.html
