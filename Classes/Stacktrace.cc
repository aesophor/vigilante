#include "Stacktrace.h"

namespace {

int numStacktraceFunctions;
const char* logLocation;

} // namespace

namespace vigilante {

namespace segv {

void installHandler(void (*handler)(int), int numStacktraceFunctions,
                    const char* logLocation) {
  ::numStacktraceFunctions = numStacktraceFunctions;
  ::logLocation = logLocation;
  signal(SIGSEGV, handler);
}

void handle(int sig) {
  void* array[10];
  size_t size = backtrace(array, 10);

  int fd = open(::logLocation, O_CREAT | O_WRONLY, 0600);
  backtrace_symbols_fd(array + 2, size - 2, fd);
  close(fd);

  exit(EXIT_FAILURE);
}

} // namespace segv

} // namespace vigilante
