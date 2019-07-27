// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_LOGGER_H_
#define VIGILANTE_LOGGER_H_

#include <array>
#include <string>
#include <memory>

#include <cocos2d.h>

#define LOG_ERR logger::Severity::ERROR
#define LOG_WARN logger::Severity::WARNING
#define LOG_INFO logger::Severity::INFO

// Example usage: VGLOG(LOG_INFO, __FILE__, "test msg %d", 5);
#define VGLOG(severity, format, ...)\
  cocos2d::log("[%s | %s: %d] " format,\
      (logger::_kSeverityStr[severity].c_str()),\
      (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__),\
      (__LINE__),\
      ##__VA_ARGS__)


namespace logger {

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

} // namespace logger

#endif // VIGILANTE_LOGGER_H_
