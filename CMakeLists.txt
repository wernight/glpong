cmake_minimum_required (VERSION 3.10)
project(GLPong LANGUAGES CXX)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)

file(GLOB_RECURSE sourceFiles
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)
add_executable(glpong src/main.cpp src/AiPaddle.cpp src/Ball.cpp src/Board.cpp src/Firework.cpp src/GLPong.cpp src/Paddle.cpp src/SceneManager.cpp)

target_link_libraries(glpong
    PRIVATE
        OpenGL::GL
        OpenGL::GLU
        GLEW::GLEW
        SDL2::SDL2
        SDL2_image::SDL2_image
)

install(TARGETS glpong DESTINATION bin)
install(FILES res/particle.png DESTINATION bin)
install(FILES res/small_blur_star.png DESTINATION bin)
