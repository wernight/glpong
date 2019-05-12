#!/bin/sh
set -ex
docker build -t glpong .
docker run --rm -it -v $PWD:/code -v emcache:/root/.emscripten_cache -p 8080:8080 glpong
