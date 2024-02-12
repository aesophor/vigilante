// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "JsonUtil.h"

#include <fstream>
#include <stdexcept>

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>

#include "util/Logger.h"

using namespace std;

namespace vigilante::json_util {

rapidjson::Document loadFromFile(const fs::path& jsonFilePath) {
  ifstream ifs(jsonFilePath);
  if (!ifs.is_open()) {
    VGLOG(LOG_ERR, "Failed to load json: [%s].", jsonFilePath.c_str());
    return {};
  }

  rapidjson::IStreamWrapper isw(ifs);
  rapidjson::Document doc;
  doc.ParseStream(isw);
  return doc;
}

void saveToFile(const fs::path& jsonFilePath, const rapidjson::Document& json) {
  ofstream ofs(jsonFilePath);
  if (!ofs.is_open()) {
    VGLOG(LOG_ERR, "Failed to open json: [%s].", jsonFilePath.c_str());
    return;
  }

  rapidjson::OStreamWrapper osw(ofs);
  rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
  json.Accept(writer);
}

}  // namespace vigilante::json_util
