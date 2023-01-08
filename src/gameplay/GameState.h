// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_GAME_STATE_H_
#define VIGILANTE_GAME_STATE_H_

#include <filesystem>

namespace fs = std::filesystem;

namespace vigilante {

class GameState final {
 public:
  explicit GameState(const fs::path& saveFilePath);

  void load();
  void save();

  inline const fs::path& getSaveFilePath() const { return _saveFilePath; }

 private:
  const fs::path _saveFilePath;
};

}  // namespace vigilante

#endif  // VIGILANTE_GAME_STATE_H_
