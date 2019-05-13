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

## Windows

Use Visual Studio; possibly generate project files using CMake.

## Linux

Using Docker:

    $ ./build.sh

Alternatively, you may use CMake directly:

    $ mkdir build
    $ cd build_
    $ cmake ..
    $ make install
    $ glpong
