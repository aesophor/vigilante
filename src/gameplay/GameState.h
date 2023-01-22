// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_GAME_STATE_H_
#define VIGILANTE_GAME_STATE_H_

#include <filesystem>

#include <json/document.h>

namespace fs = std::filesystem;

namespace vigilante {

class GameState final {
 public:
  explicit GameState(const fs::path& saveFilePath);

  void save();
  void load();

  inline const fs::path& getSaveFilePath() const { return _saveFilePath; }

 private:
  rapidjson::Value serializeGameMapState() const;
  void deserializeGameMapState(const rapidjson::Value::Object& obj) const;

  rapidjson::Value serializePlayerState() const;
  void deserializePlayerState(const rapidjson::Value::Object& obj) const;

  const fs::path _saveFilePath;
  rapidjson::Document _json;
  rapidjson::Document::AllocatorType& _allocator;
};

}  // namespace vigilante

#endif  // VIGILANTE_GAME_STATE_H_
