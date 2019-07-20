// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "QuestBook.h"

#include <fstream>
#include <algorithm>
#include <stdexcept>

#include "quest/KillTargetObjective.h"
#include "ui/notification/NotificationManager.h"

using std::string;
using std::vector;
using std::ifstream;
using std::unique_ptr;
using std::unordered_map;
using std::runtime_error;

namespace vigilante {

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


void QuestBook::update(Quest::Objective::Type objectiveType) {
  cocos2d::log("[QuestBook] updating quests");
  for (const auto quest : _inProgressQuests) {
    if (quest->getCurrentStage().objective->getObjectiveType() != objectiveType) {
      continue;
    }

    while (!quest->isCompleted() && quest->getCurrentStage().objective->isCompleted()) {
      quest->advanceStage();

      if (quest->isCompleted()) {
        markCompleted(quest);
      } else {
        NotificationManager::getInstance()->show("New objective: " + quest->getCurrentStage().objective->getDesc());
      }
    }
  }
}


void QuestBook::unlockQuest(Quest* quest) {
  quest->unlock();
}

void QuestBook::startQuest(Quest* quest) {
  auto& qs = _inProgressQuests;
  if (std::find(qs.begin(), qs.end(), quest) != qs.end()) {
    return;
  }

  // Add this quest to _inProgressQuests.
  qs.push_back(quest);

  quest->advanceStage();
  NotificationManager::getInstance()->show("Quest Started: " + quest->getQuestProfile().title);
  NotificationManager::getInstance()->show("New objective: " + quest->getCurrentStage().objective->getDesc());
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

  NotificationManager::getInstance()->show("Quest Completed: " + quest->getQuestProfile().title);

}


void QuestBook::unlockQuest(const string& questJsonFileName) {
  if (_questMapper.find(questJsonFileName) == _questMapper.end()) {
    return;
  }
  unlockQuest(_questMapper[questJsonFileName].get());
}

void QuestBook::startQuest(const string& questJsonFileName) {
  if (_questMapper.find(questJsonFileName) == _questMapper.end()) {
    return;
  }
  startQuest(_questMapper[questJsonFileName].get());
}

void QuestBook::markCompleted(const string& questJsonFileName) {
  if (_questMapper.find(questJsonFileName) == _questMapper.end()) {
    return;
  }
  markCompleted(_questMapper[questJsonFileName].get());
}

} // namespace vigilante
