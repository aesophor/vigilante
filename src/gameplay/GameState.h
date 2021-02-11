// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_GAME_STATE_H_
#define VIGILANTE_GAME_STATE_H_

#include <string>

namespace vigilante {

class GameState {
 public:
  explicit GameState();
  virtual ~GameState() = default;

  const std::string& getFilePath() const;
  void setFilePath(const std::string& filePath);

  friend std::ofstream& operator<<(std::ofstream& os, const GameState& state);
  friend std::ifstream& operator>>(std::ifstream& is, GameState& state);

 private:
  std::string _filePath;
};

}  // namespace vigilante

#endif  // VIGILANTE_GAME_STATE_H_
