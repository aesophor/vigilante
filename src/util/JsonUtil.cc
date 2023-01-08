// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "JsonUtil.h"

#include <fstream>
#include <stdexcept>

#include <json/istreamwrapper.h>
#include <json/ostreamwrapper.h>
#include <json/writer.h>

#include "util/Logger.h"

using namespace std;

namespace vigilante::json_util {

rapidjson::Document parseJson(const fs::path& jsonFileName) {
  ifstream ifs(jsonFileName);
  if (!ifs.is_open()) {
    VGLOG(LOG_ERR, "Failed to load json: [%s].", jsonFileName.c_str());
    return {};
  }

  rapidjson::IStreamWrapper isw(ifs);
  rapidjson::Document doc;
  doc.ParseStream(isw);

  VGLOG(LOG_ERR, "Read json from file: [%s].", jsonFileName.c_str());
  return doc;
}

void saveToFile(const fs::path& jsonFileName, const rapidjson::Document& json) {
  ofstream ofs(jsonFileName);
  if (!ofs.is_open()) {
    VGLOG(LOG_ERR, "Failed to open json: [%s].", jsonFileName.c_str());
    return;
  }

  rapidjson::OStreamWrapper osw(ofs);
  rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
  json.Accept(writer);

  VGLOG(LOG_ERR, "Written json to file: [%s].", jsonFileName.c_str());
}

}  // namespace vigilante::json_util
