#include "../Classes/AppDelegate.h"

#include <iostream>
#include <string>
#include <stdexcept>

using std::cerr;
using std::endl;
using cocos2d::Application;

int main(int argc, char* args[]) {
  // Create the application instance
  AppDelegate app;

  try {
    return Application::getInstance()->run();
  } catch (const std::exception& ex) {
    cerr << ex.what() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
