// Copyright (c) 2018-2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_GAMEPLAY_GAME_STATE_H_
#define REQUIEM_GAMEPLAY_GAME_STATE_H_

#include <filesystem>

#include <rapidjson/document.h>

namespace fs = std::filesystem;

namespace requiem {

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

  rapidjson::Value serializePlayerSkills() const;
  void deserializePlayerSkills(const rapidjson::Value& obj) const;

  rapidjson::Value serializePlayerQuestBook() const;
  void deserializePlayerQuestBook(const rapidjson::Value& obj) const;

  rapidjson::Value serializePlayerParty() const;
  void deserializePlayerParty(const rapidjson::Value& obj) const;

  rapidjson::Value serializeInGameTime() const;
  void deserializeInGameTime(const rapidjson::Value& obj) const;

  rapidjson::Value serializeRoomRentalTrackerState() const;
  void deserializeRoomRentalTrackerState(const rapidjson::Value& obj) const;

  rapidjson::Value serializeLatestNpcDialogueTrees() const;
  void deserializeLatestNpcDialogueTrees(const rapidjson::Value& obj) const;

  rapidjson::Value serializeHotkeys() const;
  void deserializeHotkeys(const rapidjson::Value& obj) const;

  const fs::path _saveFilePath;
  rapidjson::Document _json;
  rapidjson::Document::AllocatorType& _allocator;
};

}  // namespace requiem

#endif  // REQUIEM_GAMEPLAY_GAME_STATE_H_
