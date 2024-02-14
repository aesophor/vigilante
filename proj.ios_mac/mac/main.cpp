// Copyright (c) 2019-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include <iostream>
#include <string>
#include <stdexcept>

#include "../../Source/AppDelegate.h"
#include "../../Source/util/Logger.h"

int main(int argc, char* args[]) {
  signal(SIGSEGV, &vigilante::logger::segvHandler);

  AppDelegate app;
  try {
    return ax::Application::getInstance()->run();
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << std::endl;
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  return EXIT_SUCCESS;
}
