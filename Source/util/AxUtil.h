// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_UTIL_AX_UTIL_H_
#define VIGILANTE_UTIL_AX_UTIL_H_

#include <concepts>
#include <optional>
#include <string>

#include <axmol.h>

namespace vigilante::ax_util {

void addChildWithParentCameraMask(ax::Node* parent, ax::Node* child);
void addChildWithParentCameraMask(ax::Node* parent, ax::Node* child, const int localZOrder);

template <typename T>
requires std::same_as<T, bool>
std::optional<T> extractValueFromValueMap(const ax::ValueMap& valueMap, const std::string& key) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asBool() : std::optional<T>{std::nullopt};
}

template <typename T>
requires std::same_as<T, int>
std::optional<T> extractValueFromValueMap(const ax::ValueMap& valueMap, const std::string& key) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asInt() : std::optional<T>{std::nullopt};
}

template <typename T>
requires std::same_as<T, float>
std::optional<T> extractValueFromValueMap(const ax::ValueMap& valueMap, const std::string& key) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asFloat() : std::optional<T>{std::nullopt};
}

template <typename T>
requires std::same_as<T, int64_t>
std::optional<T> extractValueFromValueMap(const ax::ValueMap& valueMap, const std::string& key) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asInt() : std::optional<T>{std::nullopt};
}

template <typename T>
requires std::same_as<T, uint64_t>
std::optional<T> extractValueFromValueMap(const ax::ValueMap& valueMap, const std::string& key) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asUint64() : std::optional<T>{std::nullopt};
}

template <typename T>
requires std::same_as<T, std::string>
std::optional<T> extractValueFromValueMap(const ax::ValueMap& valueMap, const std::string& key) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asString() : std::optional<T>{std::nullopt};
}

template <typename T>
requires std::same_as<T, bool>
T extractValueFromValueMap(const ax::ValueMap& valueMap, const std::string& key, const T default_value) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asBool() : default_value;
}

template <typename T>
requires std::same_as<T, int>
T extractValueFromValueMap(const ax::ValueMap& valueMap, const std::string& key, const T default_value) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asInt() : default_value;
}

template <typename T>
requires std::same_as<T, float>
T extractValueFromValueMap(const ax::ValueMap& valueMap, const std::string& key, const T default_value) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asFloat() : default_value;
}

template <typename T>
requires std::same_as<T, int64_t>
T extractValueFromValueMap(const ax::ValueMap& valueMap, const std::string& key, const T default_value) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asInt64() : default_value;
}

template <typename T>
requires std::same_as<T, uint64_t>
T extractValueFromValueMap(const ax::ValueMap& valueMap, const std::string& key, const T default_value) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asUint64() : default_value;
}

template <typename T>
requires std::same_as<T, std::string>
T extractValueFromValueMap(const ax::ValueMap& valueMap, const std::string& key, const T default_value) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asString() : default_value;
}

}  // namespace vigilante::ax_util

#endif  // VIGILANTE_UTIL_AX_UTIL_H_
