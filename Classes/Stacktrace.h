#ifndef VIGILANTE_STACKTRACE_H_
#define VIGILANTE_STACKTRACE_H_

extern "C" {
#include <execinfo.h> // backtrace*
#include <signal.h> // signal
#include <stdlib.h> // exit
#include <unistd.h> // close
#include <fcntl.h> // open
}

namespace vigilante {

namespace segv {

void installHandler(void (*handler)(int), int numStacktraceFunctions=10,
                    const char* logLocation="error.log");

void handle(int sig);

} // namespace segv

} // namespace vigilante

#endif // VIGILANTE_STACKTRACE_H_
