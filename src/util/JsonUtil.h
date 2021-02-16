// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_JSON_UTIL_H_
#define VIGILANTE_JSON_UTIL_H_

#include <string>
#include <vector>

#include <json/document.h>

namespace vigilante {

namespace json_util {

rapidjson::Document parseJson(const std::string& jsonFileName);

}  // namespace json_util

}  // namespace vigilante

#endif  // VIGILANTE_JSON_UTIL_H_
