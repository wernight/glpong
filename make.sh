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