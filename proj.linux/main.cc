#include "../Classes/AppDelegate.h"

#include <iostream>
#include <string>
#include <stdexcept>

USING_NS_CC;

int main(int argc, char* args[]) {
  // Create the application instance
  AppDelegate app;

  try {
    return Application::getInstance()->run();
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
