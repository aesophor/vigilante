// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "JsonUtil.h"

#include <fstream>
#include <stdexcept>
#include <sstream>

using namespace std;
using rapidjson::Document;

namespace vigilante::json_util {

Document parseJson(const string& jsonFileName) {
  ifstream fin(jsonFileName);
  if (!fin.is_open()) {
    throw std::runtime_error("Json not found: " + jsonFileName);
  }

  string content;
  string line;
  while (std::getline(fin, line)) {
    content += line;
  }
  fin.close();

  Document json;
  json.Parse(content.c_str());
  return json;
}

}  // namespace vigilante::json_util
