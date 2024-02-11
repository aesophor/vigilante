// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_JSON_UTIL_H_
#define VIGILANTE_JSON_UTIL_H_

#include <filesystem>
#include <list>
#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <rapidjson/document.h>

namespace fs = std::filesystem;

namespace vigilante::json_util {

template <typename T>
inline constexpr bool dependent_false_v = false;

template <typename, template <typename...> typename>
inline constexpr bool is_specialization = false;

template <template <typename...> typename T, typename... Args>
inline constexpr bool is_specialization<T<Args...>, T> = true;


template <typename T>
inline constexpr bool is_pair = is_specialization<T, std::pair>;

template <typename T>
inline constexpr bool is_vector = is_specialization<T, std::vector>;

template <typename T>
inline constexpr bool is_list = is_specialization<T, std::list>;

template <typename T>
inline constexpr bool is_map = is_specialization<T, std::map>;

template <typename T>
inline constexpr bool is_unordered_map = is_specialization<T, std::unordered_map>;

template <typename T>
inline constexpr bool is_set = is_specialization<T, std::set>;

template <typename T>
inline constexpr bool is_unordered_set = is_specialization<T, std::unordered_set>;


template <typename T>
rapidjson::Value
makeJsonObject(rapidjson::Document::AllocatorType& allocator, T&& value) {
  using _T = std::remove_const_t<std::remove_reference_t<T>>;

  if constexpr (std::is_same_v<_T, std::string>) {
    return rapidjson::Value(value.c_str(), value.size(), allocator);
  } else if constexpr (is_map<_T> || is_unordered_map<_T>) {
    rapidjson::Value ret(rapidjson::kObjectType);
    for (auto& [key, val] : value) {
      auto k = makeJsonObject(allocator, key);
      auto v = makeJsonObject(allocator, val);
      ret.AddMember(k, v, allocator);
    }
    return ret;
  } else if constexpr (is_pair<_T>) {
    rapidjson::Value ret(rapidjson::kArrayType);
    auto v1 = makeJsonObject(allocator, value.first);
    auto v2 = makeJsonObject(allocator, value.second);
    ret.PushBack(v1, allocator);
    ret.PushBack(v2, allocator);
    return ret;
  } else if constexpr (is_vector<_T> || is_list<_T> || is_set<_T> || is_unordered_set<_T>) {
    rapidjson::Value ret(rapidjson::kArrayType);
    for (auto& val : value) {
      auto v = makeJsonObject(allocator, val);
      ret.PushBack(v, allocator);
    }
    return ret;
  } else {
    return rapidjson::Value(std::forward<_T>(value));
  }
}

template <typename T>
T extractJsonObject(const rapidjson::Value& val) {
  if constexpr (std::is_same_v<T, bool>) {
    return val.GetBool();
  } else if constexpr (std::is_same_v<T, int>) {
    return val.GetInt();
  } else if constexpr (std::is_same_v<T, float>) {
    return val.GetFloat();
  } else if constexpr (std::is_same_v<T, std::string>) {
    return val.GetString();
  } else if constexpr (is_pair<T>) {
    const auto& arr = val.GetArray();
    return T{
      extractJsonObject<typename T::first_type>(arr[0]),
      extractJsonObject<typename T::second_type>(arr[1])
    };
  } else if constexpr (is_vector<T> || is_list<T>) {
    T ret;
    for (const auto& element : val.GetArray()) {
      ret.emplace_back(extractJsonObject<typename T::value_type>(element));
    }
    return ret;
  } else if constexpr (is_set<T> || is_unordered_set<T>) {
    T ret;
    for (const auto& element : val.GetArray()) {
      ret.insert(extractJsonObject<typename T::value_type>(element));
    }
    return ret;
  } else if constexpr (is_map<T> || is_unordered_map<T>) {
    T ret;
    for (const auto& kv : val.GetObject()) {
      ret.insert(std::make_pair(
          extractJsonObject<typename T::key_type>(kv.name),
          extractJsonObject<typename T::mapped_type>(kv.value)));
    }
    return ret;
  } else {
    return const_cast<rapidjson::Value&>(val).GetObject();
  }
}

template <typename T>
void serializeImpl(rapidjson::Document::AllocatorType& allocator,
                   rapidjson::Value& obj,
                   std::pair<const char*, T> keyVal) {
  obj.AddMember(rapidjson::StringRef(keyVal.first),
                makeJsonObject(allocator, std::forward<T>(keyVal.second)),
                allocator);
}

template <typename T>
void deserializeImpl(const rapidjson::Value& obj,
                     const std::pair<const char*, T*>& keyVal) {
  *(keyVal.second) = extractJsonObject<T>(obj[keyVal.first]);
}

template <typename... KVs>
rapidjson::Value
serialize(rapidjson::Document::AllocatorType& allocator, KVs&&... kvs) {
  rapidjson::Value obj(rapidjson::kObjectType);
  (serializeImpl(allocator, obj, std::forward<KVs>(kvs)), ...);
  return obj;
}

template <typename... KVs>
void deserialize(const rapidjson::Value& obj, KVs... kvs) {
  (deserializeImpl(obj, kvs), ...);
}

rapidjson::Document loadFromFile(const fs::path& jsonFileName);
void saveToFile(const fs::path& jsonFileName, const rapidjson::Document& json);

}  // namespace vigilante::json_util

#endif  // VIGILANTE_JSON_UTIL_H_
