// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UTIL_STRING_UTIL_H_
#define REQUIEM_UTIL_STRING_UTIL_H_

#include <cstring>
#include <memory>
#include <string>
#include <vector>

namespace requiem {

namespace string_util {

template <typename... Args>
std::string format(const std::string& fmt, Args&&... args);

std::vector<std::string> split(const std::string& s, const char delimiter=' ');
std::vector<std::string> parseArgs(const std::string& s);
bool startsWith(const std::string& s, const std::string& keyword);
bool contains(const std::string& s, const std::string& keyword);
void replace(std::string& s, const std::string& keyword, const std::string& newword);
void strip(std::string& s);
std::string& toUpper(std::string& s);
std::string& toLower(std::string& s);

}  // namespace string_util

template <typename... Args>
std::string string_util::format(const std::string& fmt, Args&&... args) {
  // std::snprintf(dest, n, fmt, ...) returns the number of chars
  // that will be actually written into `dest` if `n` is large enough,
  // not counting the terminating null character.
  const int bufSize
    = 1 + std::snprintf(nullptr, 0, fmt.c_str(), std::forward<Args>(args)...);

  const auto buf = std::make_unique<char[]>(bufSize);
  std::memset(buf.get(), 0x00, bufSize);

  return (std::snprintf(buf.get(), bufSize, fmt.c_str(),
          std::forward<Args>(args)...) > 0) ? std::string(buf.get()) : "";
}

}  // namespace requiem

#endif  // REQUIEM_UTIL_STRING_UTIL_H_
