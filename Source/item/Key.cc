// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Key.h"

#include "util/JsonUtil.h"

using namespace std;

namespace vigilante {

Key::Key(const string& jsonFilePath)
    : MiscItem{jsonFilePath},
      _keyProfile{jsonFilePath} {}

Key::Profile::Profile(const string& jsonFilePath) {
  rapidjson::Document json = json_util::loadFromFile(jsonFilePath);

  targetTmxFilePath = json["targetTmxMapFilePath"].GetString();
  targetPortalId = json["targetPortalId"].GetInt();
}

}  // namespace vigilante
