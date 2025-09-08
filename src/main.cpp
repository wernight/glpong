#include <iostream>

#include "GLPong.h"

int main(int argc, char* argv[]) {
  try {
    GLPong game;
    game.Run();
  } catch (const std::exception& e) {
    std::cerr << "An exception occurred: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}