// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UTIL_LOGGER_H_
#define REQUIEM_UTIL_LOGGER_H_

#include <array>
#include <memory>
#include <string>

#include <axmol.h>

#define LOG_ERR requiem::logger::Severity::ERROR
#define LOG_WARN requiem::logger::Severity::WARNING
#define LOG_INFO requiem::logger::Severity::INFO

// Example usage: VGLOG(LOG_INFO, __FILE__, "test msg %d", 5);
#define VGLOG(severity, format, ...) \
  ax::print("[%s] [%s: %d] " format,\
      (requiem::logger::_kSeverityStr[severity].c_str()),\
      (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__),\
      (__LINE__),\
      ##__VA_ARGS__)

namespace requiem::logger {

enum Severity {
  ERROR,
  WARNING,
  INFO,
  SIZE
};

const std::array<std::string, Severity::SIZE> _kSeverityStr = {{
  "ERROR",
  "WARNING",
  "INFO"
}};

void segvHandler(int);

}  // namespace requiem::logger

#endif  // REQUIEM_UTIL_LOGGER_H_
