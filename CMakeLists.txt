cmake_minimum_required (VERSION 2.8.11)
project (GLPong)

find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)

include_directories(
	${OPENGL_INCLUDE_DIR}
	${SDL_INCLUDE_DIR}
	${SDLIMAGE_INCLUDE_DIR}
)

add_executable(glpong
	src/AiPaddle.cpp
	src/AiPaddle.h
	src/Ball.cpp
  	src/Ball.h
   	src/Board.cpp
 	src/Board.h
  	src/Firework.cpp
  	src/Firework.h
	src/GLPong.cpp
	src/GLPong.h
   	src/IObject.h
	src/Paddle.cpp
 	src/Paddle.h
  	src/RandomMT.h
 	src/SceneManager.cpp
	src/SceneManager.h
   	src/StdAfx.h
  	src/Vector.h
)

target_link_libraries(glpong
	LINK_PUBLIC
		${OPENGL_LIBRARIES}
		${SDL_LIBRARY}
		${SDLIMAGE_LIBRARY}
)

install(TARGETS glpong DESTINATION bin)
install(FILES res/particle.png DESTINATION bin)
install(FILES res/small_blur_star.png DESTINATION bin)
