// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_JSON_UTIL_H_
#define VIGILANTE_JSON_UTIL_H_

#include <string>
#include <vector>

#include <json/document.h>

namespace vigilante::json_util {

rapidjson::Document parseJson(const std::string& jsonFileName);

}  // namespace vigilante::json_util

#endif  // VIGILANTE_JSON_UTIL_H_
