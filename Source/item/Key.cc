// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "Key.h"

#include "util/JsonUtil.h"

namespace fs = std::filesystem;
using namespace std;

namespace vigilante {

Key::Key(const fs::path& jsonFilePath)
    : MiscItem{jsonFilePath},
      _keyProfile{jsonFilePath} {}

Key::Profile::Profile(const fs::path& jsonFilePath) {
  rapidjson::Document json = json_util::loadFromFile(jsonFilePath);

  targetTmxFilePath = json["targetTmxMapFilePath"].GetString();
  targetPortalId = json["targetPortalId"].GetInt();
}

}  // namespace vigilante
