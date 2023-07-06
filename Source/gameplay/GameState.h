// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_GAME_STATE_H_
#define VIGILANTE_GAME_STATE_H_

#include <filesystem>

#include <rapidjson/document.h>

namespace fs = std::filesystem;

namespace vigilante {

class GameState final {
 public:
  explicit GameState(const fs::path& saveFilePath)
    : _saveFilePath{saveFilePath},
      _allocator{_json.GetAllocator()} {}

  void save();
  void load();

  inline const fs::path& getSaveFilePath() const { return _saveFilePath; }

 private:
  rapidjson::Value serializeGameMapState() const;
  void deserializeGameMapState(const rapidjson::Value& obj) const;

  rapidjson::Value serializePlayerState() const;
  void deserializePlayerState(const rapidjson::Value& obj) const;

  rapidjson::Value serializePlayerInventory() const;
  void deserializePlayerInventory(const rapidjson::Value& obj) const;

  rapidjson::Value serializePlayerParty() const;
  void deserializePlayerParty(const rapidjson::Value& obj) const;

  const fs::path _saveFilePath;
  rapidjson::Document _json;
  rapidjson::Document::AllocatorType& _allocator;
};

}  // namespace vigilante

#endif  // VIGILANTE_GAME_STATE_H_
