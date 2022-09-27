#!/bin/bash -ex
mkdir -p /tmp/glpong
cd /tmp/glpong
cmake -DCMAKE_INSTALL_PREFIX=/code/bin/linux /code
make install
