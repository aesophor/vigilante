// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Logger.h"

extern "C" {
#include <execinfo.h>  // backtrace*
#include <fcntl.h>     // open
#include <signal.h>    // signal
#include <stdlib.h>    // exit
#include <unistd.h>    // close
}

#define NUM_STACKTRACE_FUNC 10
#define LOG_FILENAME "vigilante.log"

namespace vigilante {

namespace logger {

void segvHandler(int) {
  void* array[10];
  size_t size = backtrace(array, 10);

  int fd = open(LOG_FILENAME, O_CREAT | O_WRONLY, 0600);
  backtrace_symbols_fd(array + 2, size - 2, fd);
  close(fd);

  exit(EXIT_FAILURE);
}

}  // namespace logger

}  // namespace vigilante
