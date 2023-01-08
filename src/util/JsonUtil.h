// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_JSON_UTIL_H_
#define VIGILANTE_JSON_UTIL_H_

#include <filesystem>

#include <json/document.h>

namespace fs = std::filesystem;

namespace vigilante::json_util {

rapidjson::Document parseJson(const fs::path& jsonFileName);
void saveToFile(const fs::path& jsonFileName, const rapidjson::Document& json);

}  // namespace vigilante::json_util

#endif  // VIGILANTE_JSON_UTIL_H_
