// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include <iostream>
#include <string>
#include <stdexcept>

#include "../Classes/AppDelegate.h"
#include "../Classes/Stacktrace.h"

using std::cerr;
using std::endl;
using cocos2d::Application;

int main(int argc, char* args[]) {
  vigilante::segv::installHandler(&vigilante::segv::handle);

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
