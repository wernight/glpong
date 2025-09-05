cmake_minimum_required (VERSION 3.10)
project(GLPong LANGUAGES CXX)

find_package(OpenGL REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)

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
    PRIVATE
        OpenGL::GL
        OpenGL::GLU
        SDL2::SDL2
        SDL2_image::SDL2_image
)

install(TARGETS glpong DESTINATION bin)
install(FILES res/particle.png DESTINATION bin)
install(FILES res/small_blur_star.png DESTINATION bin)
