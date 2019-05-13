#!/bin/sh
set -ex
docker build -t glpong .
docker run --rm -it -v $PWD:/code glpong
