// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "QuestBook.h"

#include <fstream>
#include <algorithm>
#include <stdexcept>

using std::string;
using std::vector;
using std::ifstream;
using std::unique_ptr;
using std::unordered_map;
using std::runtime_error;

namespace vigilante {

unordered_map<string, unique_ptr<Quest>> QuestBook::_questMapper;

QuestBook::QuestBook(const string& questListFileName) {
  ifstream fin(questListFileName);
  if (!fin.is_open()) {
    throw runtime_error("Failed to open quest list: " + questListFileName);
  }

  string line;
  while (std::getline(fin, line)) {
    _questMapper[line] = unique_ptr<Quest>(new Quest(line));
  }
}


void QuestBook::update(float delta) {
  for (auto quest : _inProgressQuests) {
    // If the objective of current stage has been completed,
    // advance to the next stage.
    if (quest->getCurrentStage().objective->isCompleted()) {
      quest->advanceStage();
    }
   
    // If current stage has been completed, mark as completed.
    if (quest->isCompleted()) {
      markCompleted(quest);
    }
  }
}

void QuestBook::startQuest(Quest* quest) {
  auto& qs = _inProgressQuests;
  if (std::find(qs.begin(), qs.end(), quest) != qs.end()) {
    return;
  }

  // Add this quest to _inProgressQuests.
  qs.push_back(quest);
}

void QuestBook::markCompleted(Quest* quest) {
  auto& qs = _inProgressQuests;
  if (std::find(qs.begin(), qs.end(), quest) == qs.end()) {
    return;
  }

  // Erase this quest from _inProgressQuests,
  // and add it to _completedQuests.
  qs.erase(std::remove(qs.begin(), qs.end(), quest), qs.end());
  _completedQuests.push_back(quest);
}

} // namespace vigilante
