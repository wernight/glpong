#!/bin/bash -ex
mkdir /tmp/glpong
cd /tmp/glpong
cmake -DCMAKE_INSTALL_PREFIX=/code/bin/linux /code
make install

mkdir -p /code/bin/wasm
cd /code/bin
source /opt/emscripten/emsdk_env.sh
em++ ../src/*.cpp -I/usr/local/include -I/opt/regal/include -lSDL -L/opt/regal/lib/linux -lRegal -lRegalGLU -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s STB_IMAGE=1 -O2 -o wasm/glpong.html --preload-file ../res/particle.png@particle.png --preload-file ../res/small_blur_star.png@small_blur_star.png
cd wasm
exec emrun --no_browser --hostname 0.0.0.0 --port 8080 glpong.html
