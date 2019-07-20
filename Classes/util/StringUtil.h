#ifndef VIGILANTE_STRING_UTIL_H_
#define VIGILANTE_STRING_UTIL_H_

#include <string>
#include <vector>

namespace vigilante {

namespace string_util {

std::vector<std::string> split(const std::string& s, const char delimiter);
bool startsWith(const std::string& s, const std::string& keyword);
bool contains(const std::string& s, const std::string& keyword);
void replace(std::string& s, const std::string& keyword, const std::string& newword);
void strip(std::string& s);
std::string& toUpper(std::string& s);

} // namespace string_util

} // namespace vigilante

#endif // VIGILANTE_STRING_UTIL_H_
