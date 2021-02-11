// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Key.h"

#include "util/JsonUtil.h"

using std::string;
using rapidjson::Document;

namespace vigilante {

Key::Key(const string& jsonFileName)
    : MiscItem(jsonFileName),
      _keyProfile(jsonFileName) {}

const Key::Profile& Key::getKeyProfile() const {
  return _keyProfile;
}


Key::Profile::Profile(const string& jsonFileName) {
  Document json = json_util::parseJson(jsonFileName);

  targetTmxFileName = json["targetTmxMapFileName"].GetString();
  targetPortalId = json["targetPortalId"].GetInt();
}

}  // namespace vigilante
