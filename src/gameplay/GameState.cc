// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "GameState.h"

#include <fstream>

using std::endl;
using std::string;
using std::ifstream;
using std::ofstream;

namespace vigilante {

GameState::GameState() {}


void GameState::load() {
  ifstream fin(_filePath);
  fin >> *this;
}

void GameState::save() {
  ofstream fout(_filePath);
  fout << *this;
}


ifstream& operator>>(ifstream& ifs, GameState& state) {
  return ifs;
}

ofstream& operator<<(ofstream& ofs, const GameState& state) {
  return ofs;
}

}  // namespace vigilante
