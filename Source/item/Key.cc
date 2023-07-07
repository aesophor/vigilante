// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Key.h"

#include "util/JsonUtil.h"

using namespace std;

namespace vigilante {

Key::Key(const string& jsonFileName)
    : MiscItem{jsonFileName},
      _keyProfile{jsonFileName} {}

Key::Profile::Profile(const string& jsonFileName) {
  rapidjson::Document json = json_util::parseJson(jsonFileName);

  targetTmxFileName = json["targetTmxMapFileName"].GetString();
  targetPortalId = json["targetPortalId"].GetInt();
}

}  // namespace vigilante
