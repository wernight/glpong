GLPong
======

Small OpenGL cross-platform game.
Very clean source code.

http://www.beroux.com/france/logiciels/glpong/

Pong was one of the first video games. Score is here counted like in tennis.

2 players play on the same machine:

 - Shift / Ctrl: Controls the left paddle
 - Up / Down: Controls the right paddle

Project files:

- UML.vpp	 Visual Paradigm UML diagram.
- GLPong.sln	Solution for Visual C++ 7
- CMakeLists.txt	CMake for Linux
- make.bat	Simple make for Intel C++
- src/		Game source code
- res/		Ressources (Windows)
- bin/		Compiled files

This game uses OpenGL, GLU and SDL.

# Compilation

## Using Docker

Building the AppImage and WebAssembly using containers:

    $ docker compose build
    $ docker compose run --rm glpong

The binaries will be generated in the `bin/` folder.

To build and start the Emscripten (WebAssembly) web server,
open in browser http://localhost:8080/glpong.html after running:

    $ docker compose build
    $ docker compose run --rm --service-ports glpong ./make.sh --emrun

## Manually using CMake

Alternatively, you may build using CMake directly:

    $ mkdir build
    $ cd build_
    $ cmake ..
    $ make install
    $ glpong
