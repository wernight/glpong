CC = g++ -ansi

objects=Firework.o GLPong.o Ball.o Board.o Paddle.o AiPaddle.o SceneManager.o

all : src/StdAfx.h $(objects)
	$(CC) $(objects) -o glpong_linux -L/usr/X11R6/lib -lGLU -lGL -lX11 -lm `sdl-config --cflags --libs`

clean :
	-rm $(objects) glpong_linux

GLPong.o : src/GLPong.h src/GLPong.cpp
	$(CC) -c src/GLPong.cpp

Firework.o : src/Firework.h src/Firework.cpp
	$(CC) -c src/Firework.cpp

Ball.o : src/IObject.h src/Vector.h src/Ball.h src/Ball.cpp
	$(CC) -c src/Ball.cpp

Board.o : src/IObject.h src/Board.h src/Board.cpp
	$(CC) -c src/Board.cpp

Paddle.o : src/IObject.h src/Paddle.h src/Paddle.cpp
	$(CC) -c src/Paddle.cpp

AiPaddle.o : src/IObject.h src/Vector.h src/AiPaddle.h src/AiPaddle.cpp
	$(CC) -c src/AiPaddle.cpp

SceneManager.o : src/IObject.h src/SceneManager.h src/SceneManager.cpp
	$(CC) -c src/SceneManager.cpp
