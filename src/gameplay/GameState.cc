// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "GameState.h"

#include "util/JsonUtil.h"

using namespace std;

namespace vigilante {

GameState::GameState(const fs::path& saveFilePath)
    : _saveFilePath(saveFilePath) {}

void GameState::load() {
  //Document json = json_util::parseJson(_saveFilePath);
}

void GameState::save() {
  rapidjson::Document json;
  json.SetObject();
  rapidjson::Document::AllocatorType& allocator = json.GetAllocator();

  json.AddMember("testkey", 1, allocator);

  json_util::saveToFile(_saveFilePath, json);
}

}  // namespace vigilante
