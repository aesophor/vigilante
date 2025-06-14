// Copyright (c) 2018-2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UTIL_AX_UTIL_H_
#define REQUIEM_UTIL_AX_UTIL_H_

#include <concepts>
#include <optional>
#include <string>

#include <axmol.h>
#include <ui/UIImageView.h>

namespace std {

template <>
struct hash<ax::Vec2> {
  size_t operator()(const ax::Vec2& v) const {
    int x = static_cast<int>(std::floor(v.x));
    int y = static_cast<int>(std::floor(v.y));
    return std::hash<int>()(x) ^ (std::hash<int>()(y) << 1);
  }
};

}  // namespace std

namespace requiem::ax_util {

void addChildWithParentCameraMask(ax::Node* parent, ax::Node* child);
void addChildWithParentCameraMask(ax::Node* parent, ax::Node* child, const int localZOrder);
bool setAliasTexParameters(ax::ui::ImageView* imageView);

template <typename T>
requires std::same_as<T, bool>
std::optional<T> extract(const ax::ValueMap& valueMap, const std::string& key) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asBool() : std::optional<T>{std::nullopt};
}

template <typename T>
requires std::same_as<T, int>
std::optional<T> extract(const ax::ValueMap& valueMap, const std::string& key) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asInt() : std::optional<T>{std::nullopt};
}

template <typename T>
requires std::same_as<T, float>
std::optional<T> extract(const ax::ValueMap& valueMap, const std::string& key) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asFloat() : std::optional<T>{std::nullopt};
}

template <typename T>
requires std::same_as<T, int64_t>
std::optional<T> extract(const ax::ValueMap& valueMap, const std::string& key) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asInt() : std::optional<T>{std::nullopt};
}

template <typename T>
requires std::same_as<T, uint64_t>
std::optional<T> extract(const ax::ValueMap& valueMap, const std::string& key) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asUint64() : std::optional<T>{std::nullopt};
}

template <typename T>
requires std::same_as<T, std::string>
std::optional<T> extract(const ax::ValueMap& valueMap, const std::string& key) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asString() : std::optional<T>{std::nullopt};
}

template <typename T>
requires std::same_as<T, bool>
T extract(const ax::ValueMap& valueMap, const std::string& key, const T default_value) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asBool() : default_value;
}

template <typename T>
requires std::same_as<T, int>
T extract(const ax::ValueMap& valueMap, const std::string& key, const T default_value) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asInt() : default_value;
}

template <typename T>
requires std::same_as<T, float>
T extract(const ax::ValueMap& valueMap, const std::string& key, const T default_value) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asFloat() : default_value;
}

template <typename T>
requires std::same_as<T, int64_t>
T extract(const ax::ValueMap& valueMap, const std::string& key, const T default_value) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asInt64() : default_value;
}

template <typename T>
requires std::same_as<T, uint64_t>
T extract(const ax::ValueMap& valueMap, const std::string& key, const T default_value) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asUint64() : default_value;
}

template <typename T>
requires std::same_as<T, std::string>
T extract(const ax::ValueMap& valueMap, const std::string& key, const T default_value) {
  const auto it = valueMap.find(key);
  return it != valueMap.end() ? it->second.asString() : default_value;
}

}  // namespace requiem::ax_util

#endif  // REQUIEM_UTIL_AX_UTIL_H_
