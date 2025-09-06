cmake_minimum_required (VERSION 3.10)
project(GLPong LANGUAGES CXX)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

file(GLOB_RECURSE sourceFiles
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)
add_executable(glpong ${sourceFiles})

target_link_libraries(glpong
    PRIVATE
        OpenGL::GL
        OpenGL::GLU
        GLEW::GLEW
        SDL2::SDL2
)

install(TARGETS glpong DESTINATION bin)
install(FILES res/particle.png DESTINATION bin)
install(FILES res/small_blur_star.png DESTINATION bin)
