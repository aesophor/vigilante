// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include <iostream>
#include <string>
#include <stdexcept>

#include "../../src/AppDelegate.h"
#include "../../src/util/Logger.h"

int main(int argc, char* args[]) {
  // Install SIGSEGV handler. See util/Logger.cc
  signal(SIGSEGV, &vigilante::logger::segvHandler);

  // Create the application instance
  AppDelegate app;

  try {
    return cocos2d::Application::getInstance()->run();
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
