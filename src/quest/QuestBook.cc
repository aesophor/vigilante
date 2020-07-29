// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "QuestBook.h"

#include <fstream>
#include <algorithm>
#include <stdexcept>

#include "std/make_unique.h"
#include "quest/KillTargetObjective.h"
#include "ui/quest_hints/QuestHints.h"
#include "util/ds/Algorithm.h"
#include "util/StringUtil.h"
#include "util/Logger.h"

using std::string;
using std::vector;
using std::ifstream;
using std::unique_ptr;
using std::unordered_map;
using std::runtime_error;

namespace vigilante {

QuestBook::QuestBook(const string& questsListFileName) {
  ifstream fin(questsListFileName);
  if (!fin.is_open()) {
    throw runtime_error("Failed to open quest list: " + questsListFileName);
  }

  string line;
  while (std::getline(fin, line)) {
    _questMapper[line] = std::make_unique<Quest>(line);
  }
}


void QuestBook::update(Quest::Objective::Type objectiveType) {
  VGLOG(LOG_INFO, "Updating quests");
  for (const auto quest : _inProgressQuests) {
    if (quest->getCurrentStage().objective->getObjectiveType() != objectiveType) {
      continue;
    }

    while (!quest->isCompleted() && quest->getCurrentStage().objective->isCompleted()) {
      quest->advanceStage();

      if (quest->isCompleted()) {
        markCompleted(quest);
      } else {
        QuestHints::getInstance()->show(quest->getCurrentStage().objective->getDesc());
      }
    }
  }
}


void QuestBook::unlockQuest(Quest* quest) {
  quest->unlock();
}

void QuestBook::startQuest(Quest* quest) {
  // If the quest is already completed or is already in progress, return at once.
  if (quest->isCompleted() ||
      std::find(_inProgressQuests.begin(), _inProgressQuests.end(), quest) != _inProgressQuests.end()) {
    return;
  }

  // Add this quest to _inProgressQuests.
  _inProgressQuests.push_back(quest);

  quest->advanceStage();
  QuestHints::getInstance()->show("Started: " + quest->getQuestProfile().title);
  QuestHints::getInstance()->show(quest->getCurrentStage().objective->getDesc());
}

void QuestBook::markCompleted(Quest* quest) {
  // If `quest` is currently NOT in progress, return at once.
  if (std::find(_inProgressQuests.begin(), _inProgressQuests.end(), quest) == _inProgressQuests.end()) {
    return;
  }

  // If we can get here, then `quest` must be in progress, and we have to
  // mark it as completed. We'll erase this quest from _inProgressQuests,
  // and add it to _completedQuests.
  _inProgressQuests.erase(
      std::remove(_inProgressQuests.begin(), _inProgressQuests.end(), quest), _inProgressQuests.end());

  _completedQuests.push_back(quest);

  QuestHints::getInstance()->show("Completed: " + quest->getQuestProfile().title);
}


void QuestBook::unlockQuest(const string& questJsonFileName) {
  auto it = _questMapper.find(questJsonFileName);
  if (it == _questMapper.end()) {
    return;
  }
  unlockQuest(it->second.get());
}

void QuestBook::startQuest(const string& questJsonFileName) {
  auto it = _questMapper.find(questJsonFileName);
  if (it == _questMapper.end()) {
    return;
  }
  startQuest(it->second.get());
}

void QuestBook::markCompleted(const string& questJsonFileName) {
  auto it = _questMapper.find(questJsonFileName);
  if (it == _questMapper.end()) {
    return;
  }
  markCompleted(it->second.get());
}


vector<Quest*> QuestBook::getAllQuests() const {
  vector<Quest*> allQuests(_inProgressQuests.begin(), _inProgressQuests.end());
  allQuests.insert(allQuests.end(), _completedQuests.begin(), _completedQuests.end());
  return allQuests;
}

const vector<Quest*>& QuestBook::getInProgressQuests() const {
  return _inProgressQuests;
}

const vector<Quest*>& QuestBook::getCompletedQuests() const {
  return _completedQuests;
}

}  // namespace vigilante
